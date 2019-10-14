import React, { useState } from 'react';
import { RouteComponentProps } from 'react-router';
import { observer } from "mobx-react";
import { WRCState, WRCRemoteState } from '../../stores/wrcStore';
import { useStyles } from './style';
import Button from '@material-ui/core/Button';
import TextField from '@material-ui/core/TextField';
import Dialog from '@material-ui/core/Dialog';
import DialogActions from '@material-ui/core/DialogActions';
import DialogContent from '@material-ui/core/DialogContent';
import DialogContentText from '@material-ui/core/DialogContentText';
import DialogTitle from '@material-ui/core/DialogTitle';
import Typography from '@material-ui/core/Typography';
import { useWRCStore, useNotification } from '../../store';
import { withTimeout } from '../../utils';
import { InternalError } from '../../constant';

const screenfull = require("screenfull");

interface MatchParams {
  channel: string;
}

interface Props extends RouteComponentProps<MatchParams> {
}

const RemoteControl: React.FC<Props> = (props: Props) => {
  const params = new URLSearchParams(props.location.search);
  const classes = useStyles();
  const wrc = useWRCStore();
  const notification = useNotification();

  // 开始加入 channel
  const joinAndStart = async () => {
    try {
      await withTimeout(wrc.joinChannel(props.match.params.channel, "remote"), 5000);
      setShowPassword(true);
    } catch (e) {
      if (e === InternalError.TIMEOUT) {
        notification.pushMessage({
          level: "warning", message: "操作超时，请检查网络连接或者频道号",
        });
      } else if (e === InternalError.REMOTE_IS_BUSY) {
        notification.pushMessage({
          level: "warning", message: "远端当前无法响应",
        });
      } else {
        notification.pushMessage({
          level: "warning", message: e.toString(),
        });
      }
      return;
    }
  }
  if (wrc.connectionState === WRCState.DISCONNECTED && wrc.remoteState === WRCRemoteState.IDLE) {
    joinAndStart();
  }

  // 处理密码输入
  const [showPassword, setShowPassword] = useState(false);
  const [password, setPassword] = useState(params.get("pw") || "");
  const sendPassword = async (pw?: string) => {
    setShowPassword(false);
    const result = await wrc.sendRemoteControlRequest(pw || password);
    setPassword("");
    if (!result) {
      setShowPassword(true);
      notification.pushMessage({
        level: "warning", message: "密码错误，或者远端当前无法响应",
      });
    } else {
      if (screenfull.enabled) {
        screenfull.request();
      }
    }
  }
  const [statusText, statusLevel] = wrc.getStatusText();

  // 处理远端断开
  wrc.onRemoteClose = () => {
    notification.pushMessage({
      level: "error", message: "远端断开了连接",
    });
    props.history.push("/");
  };
  props.history.listen((location) => {
    if (location.pathname === "/" && wrc.remoteState !== WRCRemoteState.IDLE) {
      wrc.leave();
    }
  })

  return (
    <div>
      <div className={classes.statusContainer}>
        <div className={classes[statusLevel]}></div>
        <Typography variant="body2">{statusText}</Typography>
      </div>
      <div className={classes.container} id="remote"></div>

      <Dialog open={showPassword}>
        <DialogTitle>密码验证</DialogTitle>
        <DialogContent>
          <DialogContentText>请输入远端机器上的连接密码完成连接</DialogContentText>
          <TextField
            value={password}
            onChange={e => setPassword(e.target.value.toLocaleUpperCase())}
            autoFocus margin="dense" label="PASSWORD" type="text" fullWidth
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => sendPassword()} color="primary">完成</Button>
        </DialogActions>
      </Dialog>
    </div>
  );
};

export default observer(RemoteControl);