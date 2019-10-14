import { makeStyles } from "@material-ui/core/styles";
import { yellow, green } from "@material-ui/core/colors";


export const useStyles = makeStyles(theme => ({
  container: {
    width: "100vw",
    height: "100vh",
    position: "absolute",
    top: 0,
    left: 0,
  },
  statusContainer: {
    display: "flex",
    alignItems: "center",
    padding: "6px 12px",
    position: "absolute",
    top: 10,
    right: 10,
    borderRadius: 6,
    background: `rgba(0, 0, 0, .5)`,
    zIndex: 1,
    color: "#fff",
    '& div': {
      width: 8, height: 8, borderRadius: "50%",
      marginRight: 8,
    },
  },
  RED: {
    background: theme.palette.error.main,
  },
  YELLOW: {
    background: yellow[500],
  },
  GREEN: {
    background: green[500],
  },

}));