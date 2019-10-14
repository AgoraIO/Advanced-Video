import React from "react";
import clsx from "clsx";
import CheckCircleIcon from '@material-ui/icons/CheckCircle';
import ErrorIcon from '@material-ui/icons/Error';
import InfoIcon from '@material-ui/icons/Info';
import CloseIcon from '@material-ui/icons/Close';
import WarningIcon from '@material-ui/icons/Warning';
import { SnackbarContent, Icon, IconButton, Snackbar } from "@material-ui/core";
import { useStyles } from "./style";

const variantIcon = {
  success: CheckCircleIcon,
  warning: WarningIcon,
  error: ErrorIcon,
  info: InfoIcon,
};


interface Props {
  className?: string;
  message?: string;
  onClose?: () => void;
  type: keyof typeof variantIcon;
  open: boolean;
}

const SnackbarWrapper: React.FC<Props> = (props: Props) => {
  const classes = useStyles();
  const Icon = variantIcon[props.type];
  return (
    <Snackbar
      anchorOrigin={{ vertical:"bottom", horizontal: "right" }}
      open={props.open}
      onClose={props.onClose}
      autoHideDuration={4000}
    >
      <SnackbarContent
        className={clsx(classes[props.type], props.className)}
        message={
          <span className={classes.message}>
            <Icon className={clsx(classes.icon, classes.iconVariant)} />
            {props.message}
          </span>
        }
        action={[
          <IconButton key="close" color="inherit" onClick={props.onClose}>
            <CloseIcon className={classes.icon}/>
          </IconButton>
        ]}
      />
    </Snackbar>
  );
}

export default SnackbarWrapper;