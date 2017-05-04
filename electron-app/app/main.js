const electron = require('electron')
// Module to control application life.
const app = electron.app

// Module to create native browser window.
const BrowserWindow = electron.BrowserWindow

const path = require('path')
const url = require('url')

const Tray = electron.Tray
const Menu = electron.Menu
const globalShortcut  = electron.globalShortcut

let tray = null
let mainWindow = null

var force_quit = false;

var menu = Menu.buildFromTemplate([
{
    label: 'Ubuntu',
    submenu: [
        {label: 'About App', selector: 'orderFrontStandardAboutPanel:'},
        {
            label: 'Quit',
            accelerator: 'CmdOrCtrl+Q',
            click: () => { force_quit = true; app.quit(); }
        }
    ]
}]);

app.on('ready', () => {

  // Set the application menu
  Menu.setApplicationMenu(menu);

  // Create the browser window.
  mainWindow = new BrowserWindow({width: 800, height: 600});

  // Hide on close
  mainWindow.on('close', function(e){
      if(!force_quit){
          e.preventDefault();
          mainWindow.hide();
      }
  });

  // And on all-windows-closed
  app.on('before-quit', function (e) {
      // Handle menu-item or keyboard shortcut quit here
      if(!force_quit){
          e.preventDefault();
          mainWindow.hide();
      }
  });

  app.on('activate', () => {
      mainWindow.show();
  });

  // and load the index.html of the app.
  mainWindow.loadURL(url.format({
    pathname: path.join(__dirname, 'index.html'),
    protocol: 'file:',
    slashes: true
  }))

  tray = new Tray(path.join(__dirname, 'trayicon.png'));
  const contextMenu = Menu.buildFromTemplate([
    {label: 'Show window', type: 'normal', click: () => { mainWindow.show(); }},
    {label: 'Quit', type: 'normal', click: () => { force_quit=true; app.quit(); }}
  ]);
  tray.setToolTip('This is my application.');
  tray.setContextMenu(contextMenu);

  globalShortcut.register('CmdOrCtrl+Alt+U', () => {
    mainWindow.show();
  })

  tray.on('click', () => {
    mainWindow.show();
  });

  tray.on('double-click', () => {
    mainWindow.show();
  });
});