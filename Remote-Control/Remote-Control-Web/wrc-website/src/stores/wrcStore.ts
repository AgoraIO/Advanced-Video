import AgoraRTM from "agora-rtm-sdk";
import AgoraWRC from "../controller";
import { observable, action, runInAction } from "mobx";
import { InternalError, AGORA_APP_KEY } from "../constant";

// @ts-ignore
const AgoraRTC = require("agora-rtc-sdk");

export enum WRCState {
  DISCONNECTED = "DISCONNECTED",
  CONNECTING = "CONNECTING",
  CONNECTED = "CONNECTED",
  DISCONNECTING = "DISCONNECTING",
}

export enum WRCRemoteState {
  IDLE = "IDLE",
  FETCHING_REMOTE = "FETCHING_REMOTE",
  REMOTE_IS_BUSY = "REMOTE_IS_BUSY",
  REQUEST_PASSWORD = "REQUEST_PASSWORD",
  WAITING_RESPONSE = "WAITING_RESPONSE",
  REJECT = "REJECT",
  ACCEPTED = "ACCEPTED",
}

export enum WRCStatusLevel {
  RED = "RED",
  YELLOW = "YELLOW",
  GREEN = "GREEN",
}

export class WRCStore {
  @observable
  public connectionState: WRCState = WRCState.DISCONNECTED;
  @observable
  public remoteState: WRCRemoteState = WRCRemoteState.IDLE;

  public onRemoteClose?: () => any;

  private wrc: AgoraWRC;
  private remoteStream?: any;

  public constructor() {
    this.wrc = new AgoraWRC(AgoraRTC, AgoraRTM, AGORA_APP_KEY);

    this.handleEvents();
  }

  public joinChannel(channel: string, id: string): Promise<void> {
    return new Promise((resolve, reject) => {
      runInAction(() => {
        this.remoteState = WRCRemoteState.FETCHING_REMOTE;
      });
      let remoteStreams: Map<string | number, any> = new Map();
      let targetRemoteId: null | string | number = null;
      let handleStreamAdd: any;
      /**
       * 当收到远端的流时 resolve，表示远端现在可以被订阅
       */
      const streamAddReadyPromise: Promise<any> = new Promise(resolve => {
        handleStreamAdd = (e: any) => {
          if (!e.stream) return;
          const id = e.stream.getId();
          remoteStreams.set(id, e.stream);
          if (targetRemoteId !== null && id === targetRemoteId) {
            resolve(e.stream);
            return;
          }
        };
        this.wrc.agoraRTCClient.on("stream-added", handleStreamAdd);
      });

      /**
       * 当收到远端的 WRC Ready 时 resolve，表示远端现在可以受控
       */
      const getRemoteUIDReadyPromise: () => Promise<any> = () => new Promise((resolve, reject) => {
        this.wrc.getRemoteTargetUID().then(id => {
          console.log("get remote target uid", id);
          if (id === null) {
            this.remoteState = WRCRemoteState.REMOTE_IS_BUSY;
            reject(InternalError.REMOTE_IS_BUSY);
            return;
          }
          targetRemoteId = id;
          const stream = remoteStreams.get(id);
          if (stream) {
            resolve(stream);
          }
        });
      });

      this.wrc.joinChannel(channel).then((uid: number) => {
        /**
         * 当远端既可以订阅也可以被控制时，开始远程控制
         */
        Promise.all([Promise.race([streamAddReadyPromise, getRemoteUIDReadyPromise()])]).then(([res]) => {
          this.remoteStream = res;
          this.wrc.agoraRTCClient.off("stream-added", handleStreamAdd);
          this.wrc.startRemoteControl(this.remoteStream, id).then(() => {
            runInAction(() => {
              this.remoteState = WRCRemoteState.REQUEST_PASSWORD;
            });
            resolve();
          }).catch(reject);
        }).catch(reject);
      });
    });
  }

  public async sendRemoteControlRequest(msg: string): Promise<boolean> {
    runInAction(() => {
      this.remoteState = WRCRemoteState.WAITING_RESPONSE;
    });
    const result = await this.wrc.sendRemoteControlRequest(msg);
    if (!result) {
      runInAction(() => {
        this.remoteState = WRCRemoteState.REJECT;
      });
      return result;
    };

    runInAction(() => {
      this.remoteState = WRCRemoteState.ACCEPTED;
    });

    // todo
    return result;
  }

  public getStatusText(): [string, WRCStatusLevel] {
    switch (this.connectionState) {
      case WRCState.DISCONNECTING:
      case WRCState.DISCONNECTED:
        return ["已断开", WRCStatusLevel.RED];
      case WRCState.CONNECTING:
        return ["正在连接中", WRCStatusLevel.RED];
    }

    switch (this.remoteState) {
      case WRCRemoteState.FETCHING_REMOTE:
        return ["正在获取画面", WRCStatusLevel.YELLOW];
      case WRCRemoteState.REMOTE_IS_BUSY:
        return ["远端无法响应", WRCStatusLevel.RED];
      case WRCRemoteState.WAITING_RESPONSE:
      case WRCRemoteState.REQUEST_PASSWORD:
        return ["等待验证密码", WRCStatusLevel.YELLOW];
      case WRCRemoteState.ACCEPTED:
        return ["已就绪", WRCStatusLevel.GREEN];
      case WRCRemoteState.REJECT:
        return ["密码错误", WRCStatusLevel.RED];
    }

    return ["", WRCStatusLevel.RED];
  }

  @action.bound
  public leave(): void  {
    this.wrc.leaveChannel();
    this.connectionState = WRCState.DISCONNECTED;
    this.remoteState = WRCRemoteState.IDLE;
    this.remoteStream = undefined;
  }

  @action.bound
  private updateConnectionState(state: WRCState): void {
    this.connectionState = state;
  }

  private handleEvents(): void {
    this.wrc.agoraRTCClient.on("connection-state-change", (e: any) => {
      this.updateConnectionState(e.curState);
    });

    this.wrc.on("remote-close", () => {
      if (this.onRemoteClose) {
        this.onRemoteClose();
      }
    })
  }
}
