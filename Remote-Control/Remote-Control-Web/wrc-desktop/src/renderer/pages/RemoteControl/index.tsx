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

  if (wrc.wrcConnectionState === ConnectionState.DISCONNECTED) {
    wrc.startWRC().catch(e => {
      ipcRenderer.send("notifier", { title: "Start WRC service failed", message: e.toString() });
    });
  }

  props.history.listen((location) => {
    if (location.pathname !== "/control" && wrc.isStart) {
      wrc.reset();
    }
  });

  const [clipboardResult, setClipboardResult] = useState("");
  const updateClipboard = (text: string) => {
    navigator.clipboard.writeText(text).then(() => {
      setClipboardResult("The link has been copied to the clipboard");
    }).catch(() => {
      setClipboardResult("Please copy the link manually" + text);
    });
  }

  const hideWindow = () => {
    ipcRenderer.send("hide");
    ipcRenderer.send("notifier", { title: "Tips", message: "Click the tray icon to restore the window" });
  }

  return (
    <div className={classes.container}>
      <div className={classes.topBar}>
        { !wrc.remoteUID && <IconButton component={AdapterLink} to="/" className={classes.back}>
          <BackIcon />
        </IconButton> }

        { !!wrc.remoteUID && <Button variant="outlined" className={classes.abort} onClick={() => wrc.close()}>Stop Remote Control</Button> }

        <div className={classes.statusContainer}>
          <div className={wrc.wrcConnectionState === ConnectionState.CONNECTED ? classes.statusConnected : classes.statusDisConnected}></div>
          <Typography variant="subtitle2">{!!wrc.remoteUID ? "Remote Control is running" : getStatusText(wrc.wrcConnectionState)}</Typography>
        </div>
      </div>
      <div className={classes.content}>
        <Typography variant="subtitle2" gutterBottom className={classes.topHint}>Enter the following information on the <span onClick={() => openExternalLink(WRC_WEBSITE)}>Agora WRC</span> to control this device</Typography>
        <div className={classes.textContainer}>
          <Typography variant="subtitle1" gutterBottom >CHANNEL ID</Typography>
          <Typography variant="h2" gutterBottom>{wrc.channel}</Typography>
        </div>
        <div className={classes.textContainer}>
          <Typography variant="subtitle1" gutterBottom>PASSWORD</Typography>
          <Typography variant="h2" gutterBottom>{wrc.password || "******"}</Typography>
          <Typography onClick={() => wrc.updatePassword()} variant="body1" className={classes.updatePassword}>UPDATE PASSWORD</Typography>
        </div>

        <div style={{ flex: 1 }}></div>

        <div className={classes.bottomButtons}>
          <Button color="primary" disabled={!wrc.password} onClick={() => updateClipboard(`${WRC_WEBSITE}/#/s/${wrc.channel}?pw=${wrc.password}`)}>Copy remote control link</Button>
          <Button color="secondary" onClick={hideWindow}>Minimized to the system tray</Button>
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