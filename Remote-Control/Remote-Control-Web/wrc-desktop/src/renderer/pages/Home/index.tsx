import React from "react";
import Grid from "@material-ui/core/Grid";
import Typography from '@material-ui/core/Typography';
import Button from '@material-ui/core/Button';
import { Link, LinkProps } from 'react-router-dom';
import { useStyles } from "./style";
import { ipcRenderer } from "electron";
import { WRC_WEBSITE } from "../../constant";

const AdapterLink = React.forwardRef<HTMLAnchorElement, LinkProps>((props, ref) => <Link innerRef={ref} {...props} />);

const Home: React.FC = () => {
  const classes = useStyles();

  const openExternal = () => {
    ipcRenderer.send("hide");
    ipcRenderer.send("notifier", { title: "提示", message: "点击托盘图标恢复窗口" });
    ipcRenderer.send("open-external", WRC_WEBSITE);
  };
  return (
    <Grid
      direction="column"
      container
      alignItems="center"
      justify="center"
      alignContent="stretch"
      className={classes.container}
    >
      <Grid item>
        <Typography className={classes.title} variant="h3" align="center">Agora WRC Desktop</Typography>
      </Grid>
      <Grid item>
        <Button className={classes.button} component={AdapterLink} to="/control" variant="contained" color="primary">作为受控设备</Button>
      </Grid>
      <Grid item>
        <Button className={classes.button} onClick={openExternal} variant="contained" color="primary">控制其他设备</Button>
      </Grid>
    </Grid>
  );
}

export default Home;