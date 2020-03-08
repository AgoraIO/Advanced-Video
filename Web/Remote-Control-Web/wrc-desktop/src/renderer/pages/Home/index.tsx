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
    ipcRenderer.send("notifier", { title: "Tips", message: "Click the tray icon to restore the window" });
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
        <Button className={classes.button} component={AdapterLink} to="/control" variant="contained" color="primary">As a controlled device</Button>
      </Grid>
      <Grid item>
        <Button className={classes.button} onClick={openExternal} variant="contained" color="primary">Control other device</Button>
      </Grid>
    </Grid>
  );
}

export default Home;