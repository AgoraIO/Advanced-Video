/**
 * 因为 rendererprocess 和 mainprocess 需要通过通信来完成远程控制（renderer 下发控制信令，main 完成实际的控制操作）
 * 这里定义了 IPC 的通信格式
 */
export enum WRCIpcMessageType {
  DEVICE_INFO = 'device_info',
  MOUSE_MOVE = 'mouse_move',
  MOUSE_TOGGLE = 'mouse_toggle',
  MOUSE_SCROLL = 'mouse_scroll',
  KEYBOARD = 'keyboard'
}

export interface WRCIpcMessage {
  type: WRCIpcMessageType;
  /**
   * 如果提供了 rpcid，说明等待对方线程相应
   */
  rpcid?: string;
  payload?: any;
  /**
   * 放置 electron 的原生事件对象，用于主线程回复
   */
  electronEvent?: any;
}
