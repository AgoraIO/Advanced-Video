import React, { useState, FormEvent } from "react";
import { Typography, TextField, Button, useMediaQuery } from "@material-ui/core";
import { useStyles } from "./style";
import { RouteComponentProps } from "react-router";

const Home: React.FC<RouteComponentProps> = (props: RouteComponentProps) => {
  const [channel, setChannel] = useState("");
  const mobileMatch = useMediaQuery('(max-width: 600px)');
  const classes = useStyles({
    mobile: mobileMatch,
  });

  const joinChannel = (e: FormEvent<HTMLFormElement>) => {
    e.preventDefault();
    if (!channel) return;

    props.history.push(`/s/${channel}`);
  }

  return (
    <div className={classes.background}>
      <div className={classes.container}>
        <Typography variant="h2" gutterBottom>Agora WRC</Typography>
        <form onSubmit={joinChannel} className={classes.inputContainer} id="channel">
          <TextField
            className={classes.input}
            label="CHANNEL NAME"
            value={channel}
            onChange={(e) => setChannel(e.target.value.toLocaleUpperCase())}
            variant="outlined"
            placeholder="input channel to start remote control"
          />
          <Button type="submit" color="primary" variant="outlined" form="channel">START</Button>
        </form>
      </div>
    </div>
  );
};

export default Home;