// Import here Polyfills if needed. Recommended core-js (npm i -D core-js)
// import "core-js/fn/array.find"
// ...
import {
  WRCMessage,
  WRCMessageType,
  WRCRequestControl,
  WRCDeviceInfo,
  WRCMouseMoveEvent,
  WRCMouseStateEvent,
  WRCMouseScrollEvent,
  WRCKeyboardEvent,
} from '../types'
import AgoraRTM from 'agora-rtm-sdk'
import { EventEmitter } from 'events'
import { MouseRemoteControl } from './mouse'
import { KeyboardRemoteControl } from './keyboard'

export default class AgoraWRC extends EventEmitter {
  public agoraRTCClient: any;
  public agoraRTMClient: ReturnType<typeof AgoraRTM.createInstance>;

  private agoraRTC: any;
  private rtmChannel?: any;
  private remoteUID?: string | number
  private uid?: string | number
  private cname?: string
  private remoteStream?: any
  private playerElement?: HTMLVideoElement
  private remoteDeviceInfo?: WRCDeviceInfo

  private mouseController?: MouseRemoteControl
  private keyboardController?: KeyboardRemoteControl
  public constructor(rtc: any, private rtm: typeof AgoraRTM, key: string) {
    super();

    this.agoraRTC = rtc;
    this.agoraRTCClient = rtc.createClient({ mode: "rtc", coedc: "h264" });
    this.agoraRTCClient.init(key);
    this.agoraRTCClient.setClientRole("host");

    this.agoraRTMClient = rtm.createInstance(key, { logFilter: AgoraRTM.LOG_FILTER_DEBUG });
    this.agoraRTMClient.on('ConnectionStateChanged', console.error);
    this.agoraRTMClient.on('MessageFromPeer', console.warn);
    /**
     * 这里以 AgoraRTC 的服务作为远端是否还在线的判断基础
     */
    this.agoraRTCClient.on('peer-leave', (e: any) => {
      if (e.uid === this.remoteUID) {
        this.cleanRemote()
        this.emit('remote-close')
      }
    });
  }

  /**
   * 同时加入 RTC 和 RTM 的频道
   */
  public joinChannel(channel: string, uid?: number | string): Promise<number> {
    return new Promise((resolve, reject) => {
      this.cname = channel;

      this.agoraRTCClient.join(null, channel, uid, (uid: number) => {
        this.agoraRTMClient.login({ uid: uid.toString() }).then(() => {
          this.rtmChannel = this.agoraRTMClient.createChannel(channel);

          return this.rtmChannel.join();
        }).then(() => {
          /**
           * 将通过 RTM 收到的来自受控端的信息传入 handleWRCMessage 处理
           */
          this.rtmChannel.on('ChannelMessage', (data: any, uid: string) => {
            if (uid === `${this.cname}-desktop`) {
              this.handleWRCMessage(data.text);
            }
          })
          resolve(uid);
        }).catch(reject);
      }, reject);
    });
  }

  public leaveChannel(): void {
    this.agoraRTCClient.leave();
    this.rtmChannel.leave();
    this.agoraRTMClient.logout();

    this.rtmChannel = undefined;
  }

  /**
   * 返回 null 代表对端没有 ready
   */
  public async getRemoteTargetUID(): Promise<number | string | null> {
    if (!this.rtmChannel || !this.cname) {
      throw new Error("not join channel");
    }

    const rpcid = Date.now().toString();
    const message = await this.sendWRCMessage<any>({
      type: WRCMessageType.PING,
      rpcid,
    });
    return message.payload.uid;
  }

  public startRemoteControl(stream: any, id: string): Promise<any> {
    this.playerElement = document.getElementById(id) as HTMLVideoElement
    return new Promise((resolve, reject) => {
      this.remoteUID = stream.getId();
      this.agoraRTCClient.subscribe(stream, reject);
      const subscribedHandler = (e: any) => {
        this.agoraRTCClient.off("stream-subscribed", subscribedHandler);
        console.log('stream subscribed!');
        this.remoteStream = e.stream;
        resolve(e.stream);
      };
      this.agoraRTCClient.on('stream-subscribed', subscribedHandler);
      this.uid = this.agoraRTCClient.joinInfo.uid;
      console.log('self uid', this.uid);
    })
  }

  public sendRemoteControlRequest(msg: string): Promise<boolean> {
    return new Promise((resolve, reject) => {
      this.sendWRCMessage<WRCRequestControl>({
        type: WRCMessageType.REQUEST_CONTROL,
        payload: {
          msg
        },
        rpcid: Date.now().toString()
      }).then((message: WRCMessage<any>) => {
        if (message.type === WRCMessageType.REQUEST_CONTROL_ACCEPTED) {
          this.remoteDeviceInfo = message.payload
          if (this.playerElement) {
            this.remoteStream.play(this.playerElement.id, { fit: 'contain', muted: true });
            this.controlMouse()
            resolve(true)
            return
          } else {
            reject("no player element");
          }
        }

        if (message.type === WRCMessageType.REQUEST_CONTROL_REJECTED) {
          this.remoteDeviceInfo = undefined
          resolve(false)
          return
        }
      });
    })
  }

  private cleanRemote(): void {
    if (this.remoteStream) {
      this.remoteStream.stop()
    }
    this.remoteStream = undefined
    this.remoteUID = undefined
    this.playerElement = undefined
    this.mouseController = undefined
    if (this.keyboardController) {
      this.keyboardController.stop()
    }
    this.keyboardController = undefined
  }

  private controlMouse(): void {
    if (this.playerElement && this.remoteDeviceInfo) {
      console.log('control mouse', this.playerElement)
      const videoElement = this.playerElement.querySelector('video')
      if (!videoElement) return
      console.log('create control ')
      this.mouseController = new MouseRemoteControl(
        videoElement,
        this.remoteDeviceInfo,
        (e: WRCMouseMoveEvent) => {
          console.log(e.x, e.y)
          this.sendWRCMessage<WRCMouseMoveEvent>({
            type: WRCMessageType.MOUSE_MOVE_EVENT,
            uid: this.uid,
            payload: e
          })
        },
        (e: WRCMouseStateEvent) => {
          this.sendWRCMessage<WRCMouseStateEvent>({
            type: WRCMessageType.MOUSE_STATE_EVENT,
            uid: this.uid,
            payload: e
          })
        },
        (e: WRCMouseScrollEvent) => {
          this.sendWRCMessage<WRCMouseScrollEvent>({
            type: WRCMessageType.MOUSE_SCROLL_EVENT,
            uid: this.uid,
            payload: e
          })
        }
      )
      this.keyboardController = new KeyboardRemoteControl((e: WRCKeyboardEvent) => {
        this.sendWRCMessage<WRCKeyboardEvent>({
          type: WRCMessageType.KEYBOARD_EVENT,
          uid: this.uid,
          payload: e
        })
      })
    }
  }

  private sendWRCMessage<T>(message: Partial<WRCMessage<T>>): Promise<WRCMessage<any>> {
    message.wrc_version = 'beta'
    message.uid = this.uid

    this.rtmChannel.sendMessage({ text: JSON.stringify(message) });
    console.log('wrc: send', message)
    if (message.rpcid) {
      return new Promise(resolve => {
        this.once(`wrc-message-${message.rpcid}`, (message: WRCMessage<any>) => {
          resolve(message);
        });
      })
    } else {
      return Promise.resolve(message as WRCMessage<any>);
    }
  }

  private handleWRCMessage = (data: string) => {
    const message: WRCMessage<any> = JSON.parse(data)
    this.emit('wrc-message', message)
    if (message.rpcid) {
      this.emit(`wrc-message-${message.rpcid}`, message);
    }

    console.log('wrc: receive', message)
    if (message.type === WRCMessageType.WRC_CLOSE) {
      console.log('receive wrc close')
      this.cleanRemote()
      this.emit('remote-close')
    }
  }
}
