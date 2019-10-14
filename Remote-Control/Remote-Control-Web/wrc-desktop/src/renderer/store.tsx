
import React, { PropsWithChildren, useContext } from 'react';
import { WRCStore } from './stores/wrcStore';
import { useLocalStore } from 'mobx-react-lite';

export interface MobxStore {
  wrc: WRCStore;
}


export const MobxStoreContext = React.createContext<MobxStore | null>(null);


export const MobxStoreProvider = ({ children }: PropsWithChildren<any>) => {
  const wrcStore = useLocalStore(() => new WRCStore());
  return <MobxStoreContext.Provider value={{wrc: wrcStore}}>{children}</MobxStoreContext.Provider>;
}

export const useWRCStore = () => {
  const context = useContext(MobxStoreContext);
  if (!context) {
    throw new Error("no context value");
  }

  return context.wrc;
}