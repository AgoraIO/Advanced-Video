import { makeStyles } from "@material-ui/core/styles";
import { green, red } from "@material-ui/core/colors";
export const useStyles = makeStyles(theme => ({
  container: {
    padding: "10px 20px",
    height: "100vh",
    boxSizing: "border-box",
    display: "flex",
    flexDirection: "column",
  },
  topBar: {
    display: "flex",
    justifyContent: "space-between",
    alignItems: "center",
    marginBottom: 10,
  },
  statusContainer: {
    display: "flex",
    alignItems: "center",
    '& div': {
      width: 8, height: 8, borderRadius: "50%",
      marginRight: 8,
    },
  },
  statusConnected: {
    background: green[500],
  },
  statusDisConnected: {
    background: theme.palette.error.main,
  },
  topHint: {
    opacity: 0.6,
    marginBottom: 10,
    '& span': {
      fontWeight: "bold",
      textDecoration: "underline",
      cursor: "pointer",
    }
  },
  content: {
    display: "flex",
    flexDirection: "column",
    padding: "0 10px",
    flex: 1,
  },
  back: {
    transform: "translateX(-14px)",
  },
  textContainer: {
    display: "flex",
    flexDirection: "column",
    alignItems: "flex-start",
    '& h2': {
      fontFamily: "monospace",
    },
    width: "100%",
  },
  updatePassword: {
    cursor: "pointer",
    fontSize: 12,
    textDecoration: "underline",
    marginLeft: "calc(100% - 100px)",
    opacity: 0.65,
  },
  bottomButtons: {
    display: "flex",
    flexDirection: "column",
    '& button': {
      marginBottom: 8,
    },
  },
  bottom: {
    marginTop: 40,
    width: "100%",
    '& p': {
      fontSize: '12px',
      opacity: 0.6,
    },
  },
  abort: {
    color: theme.palette.error.main,
    border: "1px solid " + red[500],
  },
}));