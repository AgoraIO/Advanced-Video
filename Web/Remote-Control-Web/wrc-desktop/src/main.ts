import { app, BrowserWindow, Tray, ipcMain, shell, globalShortcut } from 'electron';
const notifier = require("node-notifier");
const path = require("path");
// @ts-ignore
const robot = require("robotjs");
const isDev = require("electron-is-dev");

import { WRCDesktopBackend } from "./controller/backend";

// Handle creating/removing shortcuts on Windows when installing/uninstalling.
if (require('electron-squirrel-startup')) { // eslint-disable-line global-require
  app.quit();
}

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let mainWindow: BrowserWindow | null = null;
let tray: Tray | null = null;

const wrcBackend = new WRCDesktopBackend(robot);

const createWindow = () => {
  // Create the browser window.
  mainWindow = new BrowserWindow({
    width: isDev ? 900 : 360,
    height: 580,
    resizable: false,
    autoHideMenuBar: true,
  });

  // @ts-ignore
  mainWindow.loadURL(MAIN_WINDOW_WEBPACK_ENTRY);

  if (isDev) {
    // Open the DevTools.
    mainWindow.webContents.openDevTools();
  }

  // Emitted when the window is closed.
  mainWindow.on('closed', () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    mainWindow = null;
  });
};

function createTray() {
  // @ts-ignore
  const image  = require("./imgs/logo24.png");
  const imagePath = path.resolve(app.getAppPath(), ".webpack", "main", image);
  tray = new Tray(imagePath);
  tray.setToolTip("Agora WRC Desktop");
  tray.on("click", () => {
    if (!mainWindow) {
      createWindow();
      return;
    }
    mainWindow.show();
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on('ready', () => {
  createWindow();
  createTray();
});

// Quit when all windows are closed.
app.on('window-all-closed', () => {
  // On OS X it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== 'darwin') {
    app.quit();
  }
});

app.on('activate', () => {
  // On OS X it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (mainWindow === null) {
    createWindow();
  }
});

ipcMain.on("hide", () => {
  if (mainWindow && tray) {
    mainWindow.hide();
  }
});

ipcMain.on("show", () => {
  if (mainWindow) {
    mainWindow.show();
  }
})

ipcMain.on("notifier", (e: any, message: any) => {
  notifier.notify(message);
});

ipcMain.on("open-external", (e: any, url: any) => {
  shell.openExternal(url);
});

let stopControlEvent: null | any = null;
ipcMain.on("start-control", (e: any) => {
  if (!stopControlEvent) {
    globalShortcut.register("Control+Alt+Q", () => {
      stopControlEvent && stopControlEvent.sender.send("stop-control");
      mainWindow && mainWindow.show();
    });
  }
  stopControlEvent = e;
});
