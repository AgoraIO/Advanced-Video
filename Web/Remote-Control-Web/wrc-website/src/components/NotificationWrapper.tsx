import React, { PropsWithChildren } from "react";
import { useNotification } from "../store";
import { observer } from "mobx-react-lite";
import SnackbarWrapper from "./SnackbarWrapper";

const NotificationWrapper: React.FC<PropsWithChildren<any>> = (props: PropsWithChildren<any>) => {
  const notification = useNotification();
  return (
    <div>
      {props.children}
      {notification.messages.map(message => (
        <SnackbarWrapper
          key={message.key}
          open={message.key === notification.currentKey}
          type={message.level}
          onClose={notification.removeMessage}
          message={message.message}
        />
      ))}
    </div>
  );
}

export default observer(NotificationWrapper);