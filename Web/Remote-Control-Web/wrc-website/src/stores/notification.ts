import { observable, action, computed } from "mobx";

export interface WRCNotification {
  level: "warning" | "error" | "info";
  message: string;
  key: string;
}

export class NotificationStore {
  @observable
  public messages: WRCNotification[] = [];

  @computed
  public get currentKey(): string | null {
    if (this.messages.length === 0) {
      return null;
    }

    return this.messages[0].key;
  }

  @action.bound
  public pushMessage(message: Partial<WRCNotification>): void {
    message.key = Math.random().toString().slice(2, -1);
    this.messages.push(message as WRCNotification);
  }

  @action.bound
  public removeMessage(): void {
    this.messages.shift();
  }

}