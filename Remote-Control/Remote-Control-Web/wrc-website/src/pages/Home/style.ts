import { makeStyles } from "@material-ui/core/styles";

export const useStyles = makeStyles(theme => ({
  background: {
    display: "flex",
    alignItems: "center",
    justifyContent: "center",
    width: "100vw",
    height: "100vh",
    padding: 20,
    boxSizing: "border-box",
  },
  container: {
    display: "flex",
    flexDirection: "column",
    alignItems: "center",
    maxWidth: "100%",
  },
  inputContainer: {
    display: "flex",
    maxWidth: "100%",
    flexWrap: (props: any) => { return props.mobile ? "wrap" : undefined },
    justifyContent: "center",
  },
  input: {
    width: 400,
    maxWidth: "100%",
    marginRight: 14,
    marginBottom: (props: any) => props.mobile ? "20px" : undefined,
  },
}));
