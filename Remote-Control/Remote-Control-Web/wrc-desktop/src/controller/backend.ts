import EventEmitter from 'events'
import RobotCore from './robot'
import { WRCIpcMessage, WRCIpcMessageType } from './ipc_types'
import { ipcMain } from "electron";
import { WRCMouseMoveEvent, WRCMouseStateEvent, WRCMouseScrollEvent, WRCKeyboardEvent } from '../types'

/**
 * WRC 服务在 main process 的控制器
 * 主要任务有 2 个
 * - 响应 renderer process 的请求，比如查询设备信息
 * - 接收 renderer process 的命令，调用 robotjs 完成远程控制
 */
export class WRCDesktopBackend extends EventEmitter {
  private robot: RobotCore;
  public constructor(robot: any) {
    super()
    this.robot = new RobotCore(robot)

    /** 接收 Render Process 的消息，传递给 `handleIpcMessage` */
    ipcMain.on("wrc_message", (e: any, arg: any) => {
      arg.electronEvent = e;
      this.handleIpcMessage(arg);
    });
  }

  /**
   * 向 Renderer Process 响应消息
   */
  private sendIpcMessage(message: WRCIpcMessage): void {
    if (!message.electronEvent) return;
    const event = message.electronEvent;
    message.electronEvent = undefined;

    event.sender.send("wrc_message", message);
  }

  private handleIpcMessage(message: WRCIpcMessage): void {
    switch (message.type) {
      case WRCIpcMessageType.DEVICE_INFO: {
        const payload = {
          size: this.robot.getScreenSize(),
          platform: this.robot.getPlatform()
        }
        this.sendIpcMessage({
          type: WRCIpcMessageType.DEVICE_INFO,
          rpcid: message.rpcid,
          payload,
          electronEvent: message.electronEvent
        })
        break
      }
      case WRCIpcMessageType.MOUSE_MOVE: {
        const payload: WRCMouseMoveEvent = message.payload
        this.robot.mouseMove(payload.x, payload.y)
        break
      }
      case WRCIpcMessageType.MOUSE_TOGGLE: {
        const payload: WRCMouseStateEvent = message.payload
        this.robot.toggleMouseState(payload.button, payload.down)
        break
      }
      case WRCIpcMessageType.MOUSE_SCROLL: {
        const payload: WRCMouseScrollEvent = message.payload
        this.robot.mouseScroll(payload.x, payload.y)
        break
      }
      case WRCIpcMessageType.KEYBOARD: {
        const payload: WRCKeyboardEvent = message.payload
        this.robot.toggleKey(payload.key, payload.state)
        break
      }

      default: {
        break
      }
    }
  }
}
