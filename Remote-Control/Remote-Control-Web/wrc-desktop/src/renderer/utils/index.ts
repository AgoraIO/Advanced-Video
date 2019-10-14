import { ConnectionState } from "../stores/wrcStore";
import { shell } from "electron";

export function randomString(length: number = 7): string {
  return Math.random().toString(35).substr(2, length).toLocaleUpperCase();
}

export function getStatusText(status: ConnectionState): string {
  switch (status) {
    case ConnectionState.CONNECTED:
      return "已就绪";
    case ConnectionState.CONNECTING:
      return "正在建立连接";
    case ConnectionState.DISCONNECTED:
      return "已断开";
    case ConnectionState.DISCONNECTING:
      return "断开连接中";
  }
}

export function openExternalLink(link: string): void {
  shell.openExternal(link);
}