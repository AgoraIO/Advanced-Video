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
    if (this.remoteUID || msg !== this.password) {
      cb(false);
      return;
    }
    cb(true);
    this.remoteUID = uid;

    ipcRenderer.send("start-control");
    ipcRenderer.send("notifier", {
      title: "The remote end starts to control this device",
      message: "Tap Ctrl + Alt + Q to stop remote control",
    });
    ipcRenderer.send("hide");
  }


  private handleWRCEvents(): void {
    this.wrcClient.agoraRTCClient.on("connection-state-change", (e: any) => {
      this.updateConnectionState(e.curState);
    });

    this.wrcClient.on("remote-close", () => {
      console.log("remote close");
      ipcRenderer.send("show");
      ipcRenderer.send("notifier", {
        title: "Remote Control is Over",
        message: "remote disconnected",
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