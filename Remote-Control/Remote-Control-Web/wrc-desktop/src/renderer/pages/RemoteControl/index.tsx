import React, { useState } from "react";
import { Link, LinkProps, RouteComponentProps } from 'react-router-dom';
import Button from "@material-ui/core/Button";
import BackIcon from "@material-ui/icons/ArrowBack";
import IconButton from "@material-ui/core/IconButton";
import Typography from "@material-ui/core/Typography";
import Snackbar from '@material-ui/core/Snackbar';

import { useWRCStore } from "../../store";
import { observer } from "mobx-react";
import { ConnectionState } from "../../stores/wrcStore";
import { useStyles } from "./style";
import { getStatusText, openExternalLink } from "../../utils";
import { ipcRenderer } from "electron";
import { WRC_WEBSITE } from "../../constant";


const AdapterLink = React.forwardRef<HTMLAnchorElement, LinkProps>((props, ref) => (
  <Link innerRef={ref as any} {...props} />
));

const RemoteControl: React.FC<RouteComponentProps> = observer((props: RouteComponentProps) => {
  const classes = useStyles(useStyles);
  const wrc = useWRCStore();

  /** 
   * 如果 WRC 服务没有启动，启动 WRC 服务开启屏幕共享并准备接收远程控制
   */
  if (wrc.wrcConnectionState === ConnectionState.DISCONNECTED) {
    wrc.startWRC().catch(e => {
      ipcRenderer.send("notifier", { title: "启动失败", message: e.toString() });
    });
  }

  /**
   * 如果离开了这个页面，关闭 WRC 服务
   */
  props.history.listen((location) => {
    if (location.pathname !== "/control" && wrc.isStart) {
      wrc.reset();
    }
  });

  /**
   *  处理剪贴板
   */
  const [clipboardResult, setClipboardResult] = useState("");
  const updateClipboard = (text: string) => {
    navigator.clipboard.writeText(text).then(() => {
      setClipboardResult("链接已经复制到剪贴板");
    }).catch(() => {
      setClipboardResult("请手动复制链接 " + text);
    });
  }

  /**
   * 通知 main process 显示/隐藏窗口
   */
  const hideWindow = () => {
    ipcRenderer.send("hide");
    ipcRenderer.send("notifier", { title: "提示", message: "点击托盘图标恢复窗口" });
  }

  return (
    <div className={classes.container}>
      <div className={classes.topBar}>
        { !wrc.remoteUID && <IconButton component={AdapterLink} to="/" className={classes.back}>
          <BackIcon />
        </IconButton> }

        { !!wrc.remoteUID && <Button variant="outlined" className={classes.abort} onClick={() => wrc.close()}>断开控制</Button> }

        <div className={classes.statusContainer}>
          <div className={wrc.wrcConnectionState === ConnectionState.CONNECTED ? classes.statusConnected : classes.statusDisConnected}></div>
          <Typography variant="subtitle2">{!!wrc.remoteUID ? "正在被远端控制" : getStatusText(wrc.wrcConnectionState)}</Typography>
        </div>
      </div>
      <div className={classes.content}>
        <Typography variant="subtitle2" gutterBottom className={classes.topHint}>在 <span onClick={() => openExternalLink(WRC_WEBSITE)}>Agora WRC</span> 中输入以下信息，远程控制这台设备</Typography>
        <div className={classes.textContainer}>
          <Typography variant="subtitle1" gutterBottom >CHANNEL ID</Typography>
          <Typography variant="h2" gutterBottom>{wrc.channel}</Typography>
        </div>
        <div className={classes.textContainer}>
          <Typography variant="subtitle1" gutterBottom>PASSWORD</Typography>
          <Typography variant="h2" gutterBottom>{wrc.password || "******"}</Typography>
          <Typography onClick={() => wrc.updatePassword()} variant="body1" className={classes.updatePassword}>更新 PASSWORD</Typography>
        </div>

        <div style={{ flex: 1 }}></div>

        <div className={classes.bottomButtons}>
          <Button color="primary" disabled={!wrc.password} onClick={() => updateClipboard(`${WRC_WEBSITE}/#/s/${wrc.channel}?pw=${wrc.password}`)}>复制远程控制链接</Button>
          <Button color="secondary" onClick={hideWindow}>最小化到系统托盘</Button>
        </div>
      </div>
      <div className={classes.bottom}>
        <Typography variant="body2" align="right">Agora WRC Desktop</Typography>
      </div>

      <Snackbar
        open={clipboardResult !== ""}
        autoHideDuration={2000}
        onClose={() => setClipboardResult("")}
        message={clipboardResult}
      />
    </div>
  );
});

export default RemoteControl;