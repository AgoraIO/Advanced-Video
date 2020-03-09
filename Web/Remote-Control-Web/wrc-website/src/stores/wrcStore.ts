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
       * when this promise resolved, it means the remote stream can be subscribed now.
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
       * when this promise resolve, it means the remote side can accept control signal
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
         * when the remote side can be subscribed and accept control signal, start remote control
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
        return ["DISCONNECTED", WRCStatusLevel.RED];
      case WRCState.CONNECTING:
        return ["CONNECTING", WRCStatusLevel.RED];
    }

    switch (this.remoteState) {
      case WRCRemoteState.FETCHING_REMOTE:
        return ["WAITING REMOTE DATA", WRCStatusLevel.YELLOW];
      case WRCRemoteState.REMOTE_IS_BUSY:
        return ["NO RESPONSE FROM REMOTE", WRCStatusLevel.RED];
      case WRCRemoteState.WAITING_RESPONSE:
      case WRCRemoteState.REQUEST_PASSWORD:
        return ["CHECKING PASSWORD", WRCStatusLevel.YELLOW];
      case WRCRemoteState.ACCEPTED:
        return ["READY", WRCStatusLevel.GREEN];
      case WRCRemoteState.REJECT:
        return ["PASSWORD ERROR", WRCStatusLevel.RED];
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
