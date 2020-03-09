export enum WRCMessageType {
  REQUEST_CONTROL = "wrc_request_control",
  REQUEST_CONTROL_ACCEPTED = "wrc_request_control_accepted",
  REQUEST_CONTROL_REJECTED = "wrc_request_control_rejected",
  WRC_CLOSE = "wrc_close",
  MOUSE_MOVE_EVENT = "wrc_mouse_move_event",
  MOUSE_STATE_EVENT = "wrc_mouse_state_event",
  MOUSE_SCROLL_EVENT = "wrc_mouse_scroll_event",
  KEYBOARD_EVENT = "wrc_keyboard_event",
  PING = "wrc_ping",
  PONG = "wrc_pong",
}

export enum WRCPlatform {
  WIN32 = "win32",
  DARWIN = "darwin",
}

export interface WRCPong {
  status: "ready" | "busy";
  uid: number | string;
}

export interface WRCMessage<T> {
  type: WRCMessageType;
  wrc_version: string;
  payload?: T;
  rpcid?: string;
  uid?: string | number;
  targetUID?: string | number;
}

export interface WRCDeviceInfo {
  width: number;
  height: number;
  platform: string;
}

export interface WRCRequestControl {
  msg?: string;
}


export interface WRCMouseMoveEvent {
  x: number;
  y: number;
}
export interface WRCMouseStateEvent {
  button: "left" | "middle" | "right";
  down: "down" | "up";
}

export interface WRCMouseScrollEvent {
  x: number;
  y: number;
}

export interface WRCKeyboardEvent {
  key: string;
  state: "down" | "up";
}