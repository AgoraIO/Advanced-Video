import AgoraRTM from 'agora-rtm-sdk';
import EventEmitter from 'events';
import { ipcRenderer } from "electron";
import { WRCMessage, WRCMessageType, WRCRequestControl, WRCDeviceInfo, WRCPong } from '../types'
import { WRCIpcMessage, WRCIpcMessageType } from './ipc_types'
import { randomString } from '../renderer/utils';

/**
 * WRC 服务在 renderer process 的控制器
 * 主要任务有 2 个，分别由 2 个 SDK 完成
 * - 采集和发布屏幕共享的流，由 AgoraRTC 完成
 * - 接收和响应远端的控制信令，由 AgoraRTM 完成
 */
export class WRCDesktopClient extends EventEmitter {
  public agoraRTCClient: any;
  public agoraRTMClient: ReturnType<typeof AgoraRTM.createInstance>;
  public rtmChannel?: any;

  public uid?: string | number;
  public currentRemoteUID?: string | number;
  public isPublished: boolean = false;

  private deviceInfo?: WRCDeviceInfo;

  /**
   * 每当远端希望控制这台设备时，都会走入这个回调，回调 true 表示同意被这台设备控制，默认实现为永远为 true 
   */
  public onRemoteRequest: (
    uid: string | number,
    msg: string,
    callback: (result: boolean) => any
  ) => any = (uid, msg, cb) => {
    cb(true);
  }

  private agoraRTC: any;
  private localStream?: any;
  private cname?: string;

  public constructor(agora: any, rtm: typeof AgoraRTM, key: string) {
    super();

    this.agoraRTC = agora;
    this.agoraRTCClient = this.agoraRTC.createClient({ mode: 'rtc', codec: 'h264' });
    this.agoraRTCClient.init(key);

    this.agoraRTMClient = rtm.createInstance(key, { logFilter: AgoraRTM.LOG_FILTER_DEBUG });
    this.agoraRTMClient.on('ConnectionStateChanged', console.error);
    this.agoraRTMClient.on('MessageFromPeer', console.warn);

    /** 通过 electron 的 ipc 模块接收来自 main process 的信息，转换成内部事件方便处理 */
    ipcRenderer.on("wrc_message", (event: any, message: any) => {
      if (message.rpcid) {
        this.emit(`ipc-receive-${message.rpcid}`, message);
      } else {
        this.emit("ipc-receive");
      }
    })

    /**
     * 这里以 AgoraRTC 的服务作为远端是否还在线的判断基础
     */
    this.agoraRTCClient.on('peer-leave', (e: any) => {
      const uid = e.uid;
      if (uid === this.currentRemoteUID) {
        this.currentRemoteUID = undefined;
        this.close();
        this.emit('remote-close');
      }
    });
  }

  /**
   * 开启远程控制服务，主要是发布屏幕共享流和开启 RTM 服务
   */
  public async start(
    channel: string,
    uid?: number | string,
    withAudio?: boolean
  ): Promise<any> {
    this.cname = channel;
    await this.agoraRTMClient.login({ uid: `${channel}-desktop` });
    /**
     * 加入 RTM 频道
     */
    this.rtmChannel = this.agoraRTMClient.createChannel(this.cname);
    const rtmPromise = this.rtmChannel.join();
    /**
     * 加入 RTC 频道
     */
    const joinPromise: Promise<string | number> = new Promise((resolve, reject) => {
      this.agoraRTCClient.join(null, channel, uid, resolve, reject);
    });
    /**
     * 采集本地的屏幕共享
     */
    const localStreamPromise = new Promise((resolve, reject) => {
      return new Promise((resolve, reject) => {
        this.agoraRTC.getScreenSources((err: any, sources: any[]) => {
          if (!!err) {
            return reject(err);
          }
          const entireScreen = sources.find(source => source.name.toLowerCase() === "entire screen");
          if (entireScreen) {
            resolve(entireScreen.id);
            return;
          }
          const screens = sources.filter(source => source.id.startsWith("screen"));
          if (screens.length >= 1) {
            resolve(screens[0].id);
            return;
          }
          reject("NO_SCREEN");
        })
      }).then((id: any) => {
        const localStream = this.agoraRTC.createStream({
          audio: !!withAudio,
          video: false,
          screen: true,
          sourceId: id
        });
        localStream.setScreenProfile('1080p_1');
        localStream.init(() => resolve(localStream), reject);
      });
    });

    /** 当 RTM/RTC 都加入成功，且屏幕共享也采集完毕，就可以继续接下来的流程然后发布了 */
    const res = await Promise.all([joinPromise, localStreamPromise, rtmPromise]);
    this.uid = res[0];
    this.localStream = res[1];

    /** 向 main process 请求获取当前的设备信息 */
    const response = await this.requestElectronMain({ type: WRCIpcMessageType.DEVICE_INFO });
    const size = response.payload.size;
    const platform = response.payload.platform;
    this.deviceInfo = {
      width: size.width,
      height: size.height,
      platform,
    };

    /** 接收来自控制端的信令，传到 `handleWRCMessage` 里处理 */
    this.rtmChannel.on("ChannelMessage", (data: any) => {
      this.handleWRCMessage(data.text)
    });

    /** 发布屏幕共享流，WRC 服务启动结束 */
    const publishPromise = new Promise((resolve, reject) => {
      const handlePublished = () => {
        this.isPublished = true;
        resolve();
        this.agoraRTCClient.off("stream-published", handlePublished);
      }
      this.agoraRTCClient.on('stream-published', handlePublished);
      this.agoraRTCClient.publish(this.localStream, reject);
    });

    return await publishPromise;
  }

  /** 关闭和远端的链接，但是不影响本地 WRC 服务 */
  public close(): void {
    this.sendWRCMessage({
      type: WRCMessageType.WRC_CLOSE,
      uid: this.uid,
      targetUID: this.currentRemoteUID,
    });
    if (this.currentRemoteUID) {
      this.currentRemoteUID = undefined;
    }
  }

  /** 停止本地的 WRC 服务 */
  public stop(): void {
    this.close();
    this.isPublished = false;
    if (this.localStream) {
      this.localStream.close();
    }
    this.agoraRTCClient.leave();
    this.rtmChannel.leave();
    this.agoraRTMClient.logout();

    this.rtmChannel = undefined;
  }

  private handleNewRemoteRequest(uid: string | number, payload: WRCRequestControl, rpcid?: string): void {
    console.log("handle new remote request", uid);
    this.onRemoteRequest(uid, payload.msg || '', result => {
      console.log('on Remote Request', result)
      if (!result) {
        this.sendWRCMessage({
          type: WRCMessageType.REQUEST_CONTROL_REJECTED,
          rpcid,
        })
        return
      }

      if (this.currentRemoteUID) {
        console.log("send close from new remote request");
        this.sendWRCMessage({
          type: WRCMessageType.WRC_CLOSE,
          targetUID: this.currentRemoteUID,
        });
      }

      this.currentRemoteUID = uid;
      this.sendWRCMessage({
        type: WRCMessageType.REQUEST_CONTROL_ACCEPTED,
        payload: this.deviceInfo,
        rpcid
      })
    })
  }

  /** 通过 electron 的 ipc 模块，向 main process 请求，并期望得到 main process 的响应*/
  private requestElectronMain(message: WRCIpcMessage): Promise<WRCIpcMessage> {
    return new Promise(resolve => {
      const rpcid = randomString(7);
      message.rpcid = rpcid;
      this.on(`ipc-receive-${rpcid}`, resolve);

      ipcRenderer.send("wrc_message", message);
    });
  }

  /** 通过 ipc 向 main process 发送控制命令 */
  private handleControlEvent(message: WRCMessage<any>): void {
    switch (message.type) {
      case WRCMessageType.MOUSE_MOVE_EVENT: {
        ipcRenderer.send("wrc_message", {
          type: WRCIpcMessageType.MOUSE_MOVE,
          payload: message.payload,
        });
        break
      }
      case WRCMessageType.MOUSE_STATE_EVENT: {
        ipcRenderer.send("wrc_message", {
          type: WRCIpcMessageType.MOUSE_TOGGLE,
          payload: message.payload
        })
        break
      }
      case WRCMessageType.MOUSE_SCROLL_EVENT: {
        ipcRenderer.send("wrc_message", {
          type: WRCIpcMessageType.MOUSE_SCROLL,
          payload: message.payload
        })
        break
      }
      case WRCMessageType.KEYBOARD_EVENT: {
        ipcRenderer.send("wrc_message", {
          type: WRCIpcMessageType.KEYBOARD,
          payload: message.payload
        })
        break
      }
      default: {
        break
      }
    }
  }

  /** 通过 RTM 在频道内广播消息 */
  private sendWRCMessage<T>(message: Partial<WRCMessage<T>>): void {
    message.wrc_version = 'beta';
    message.uid = this.uid;
    this.rtmChannel && this.rtmChannel.sendMessage({ text: JSON.stringify(message) });
  }

  /** 处理远端的消息 */
  private handleWRCMessage = (data: string) => {
    const message: WRCMessage<any> = JSON.parse(data);

    switch (message.type) {
      case WRCMessageType.PING: {
        if (!this.isPublished || !this.uid) break;
        if (this.currentRemoteUID) {
          this.sendWRCMessage<WRCPong>({
            type: WRCMessageType.PONG,
            rpcid: message.rpcid,
            payload: { status: "busy", uid: this.uid }
          });
        } else {
          this.sendWRCMessage<WRCPong>({
            type: WRCMessageType.PONG,
            rpcid: message.rpcid,
            payload: { status: "ready", uid: this.uid }
          });
        }
        break;
      }
      case WRCMessageType.WRC_CLOSE: {
        if (this.currentRemoteUID) {
          this.currentRemoteUID = undefined;
          this.emit("remote-close");
        }
      }
      case WRCMessageType.REQUEST_CONTROL: {
        if (!message.uid) break;
        this.handleNewRemoteRequest(message.uid, message.payload, message.rpcid)
        break
      }
      case WRCMessageType.KEYBOARD_EVENT:
      case WRCMessageType.MOUSE_SCROLL_EVENT:
      case WRCMessageType.MOUSE_STATE_EVENT:
      case WRCMessageType.MOUSE_MOVE_EVENT: {
        if (!message.uid || message.uid !== this.currentRemoteUID) break;
        this.handleControlEvent(message)
        break
      }
      default: {
        break
      }
    }
  }
}
