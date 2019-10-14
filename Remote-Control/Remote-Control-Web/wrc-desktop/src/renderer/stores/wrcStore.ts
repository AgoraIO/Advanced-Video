import { observable, action, runInAction } from "mobx";
import { WRCDesktopClient } from "../../controller";
import AgoraRTM from "agora-rtm-sdk";
import { randomString } from "../utils";
import { AGORA_APP_KEY } from "../constant";
import { ipcRenderer } from "electron";

// @ts-ignore
const AgoraRTC = require("agora-rtc-sdk");

export enum ConnectionState {
  DISCONNECTED = "DISCONNECTED",
  CONNECTING = "CONNECTING",
  CONNECTED = "CONNECTED",
  DISCONNECTING = "DISCONNECTING",
}

/**
 * 管理 WRC 服务相关状态的 Store，用于触发一些操作 WRC 的 action 以及同步 WRC 服务的当前状态
 */
export class WRCStore {
  @observable
  public channel: string;
  @observable
  public wrcConnectionState: ConnectionState = ConnectionState.DISCONNECTED;
  @observable
  public remoteUID?: string | number;
  @observable
  public password?: string;
  @observable
  public isStart?: boolean = false;

  private wrcClient: WRCDesktopClient;

  public constructor() {
    this.channel = randomString();

    this.wrcClient = new WRCDesktopClient(AgoraRTC, AgoraRTM, AGORA_APP_KEY);

    this.wrcClient.onRemoteRequest = this.handleRemoteRequest;
    this.handleWRCEvents();
  }

  /**
   * 开始 WRC 服务，分为 2 个过程
   */
  @action.bound
  public async startWRC(): Promise<void> {
    this.channel = randomString();
    runInAction(() => {
      this.wrcConnectionState = ConnectionState.CONNECTING;
      this.isStart = true;
    });
    await this.wrcClient.start(this.channel, undefined, false);
    runInAction(() => {
      if (!this.password) {
        this.password = randomString();
      }
    });
  }

  @action.bound
  public updateConnectionState(state: ConnectionState): void {
    this.wrcConnectionState = state;
  }

  @action.bound
  public updatePassword(): void {
    if (!this.password) return;
    this.password = randomString();
  }

  @action.bound
  public close(): void {
    this.wrcClient.close();
    this.remoteUID = undefined;
  }

  @action.bound
  public reset(): void {
    this.isStart = false;
    this.wrcClient.stop();
    this.remoteUID = undefined;
    this.wrcConnectionState = ConnectionState.DISCONNECTED;
    this.password = undefined;
  }


  @action.bound
  private handleRemoteRequest(uid: string | number, msg: string, cb: (result: boolean) => any): any {
    /**
     * 不允许抢占控制权限
     */
    if (this.remoteUID || msg !== this.password) {
      cb(false);
      return;
    }
    cb(true);
    this.remoteUID = uid;

    ipcRenderer.send("start-control");
    ipcRenderer.send("notifier", {
      title: "远端开始控制这台设备",
      message: "按下 Ctrl + Alt + Q 中止远程控制",
    });
    ipcRenderer.send("hide");
  }


  private handleWRCEvents(): void {
    /**  以 RTC 的链接状态来决定 WRC 服务的链接状态 */
    this.wrcClient.agoraRTCClient.on("connection-state-change", (e: any) => {
      this.updateConnectionState(e.curState);
    });

    this.wrcClient.on("remote-close", () => {
      console.log("remote close");
      ipcRenderer.send("show");
      ipcRenderer.send("notifier", {
        title: "远程控制结束",
        message: "远端已经断开连接",
      });
      this.remoteUID = undefined;
    });

    ipcRenderer.on("stop-control", () => {
      if (!this.remoteUID) return;
      console.log("receive stop control");
      this.close();
    });
  }
}