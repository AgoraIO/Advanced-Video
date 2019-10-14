
import React, { PropsWithChildren, useContext } from 'react';
import { WRCStore } from './stores/wrcStore';
import { useLocalStore } from 'mobx-react-lite';
import { NotificationStore } from './stores/notification';

export interface MobxStore {
  wrc: WRCStore;
  notification: NotificationStore;
}


export const MobxStoreContext = React.createContext<MobxStore | null>(null);


export const MobxStoreProvider = ({ children }: PropsWithChildren<any>) => {
  const wrcStore = useLocalStore(() => new WRCStore());
  const notificationStore = useLocalStore(() => new NotificationStore());
  return <MobxStoreContext.Provider value={{wrc: wrcStore, notification: notificationStore}}>{children}</MobxStoreContext.Provider>;
}

export const useWRCStore = () => {
  const context = useContext(MobxStoreContext);
  if (!context) {
    throw new Error("no context value");
  }

  return context.wrc;
}

export const useNotification = () => {
  const context = useContext(MobxStoreContext);
  if (!context) {
    throw new Error("no context value");
  }

  return context.notification;
}