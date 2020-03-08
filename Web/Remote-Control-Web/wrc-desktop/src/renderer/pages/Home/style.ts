import { makeStyles } from "@material-ui/core/styles";

export const useStyles = makeStyles(theme => ({
  container: {
    height: "100vh",
  },
  title: {
    marginBottom: 80,
  },
  button: {
    width: 250,
    height: 40,
    margin: "10px 0",
  }
}));