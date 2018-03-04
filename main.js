const {app, BrowserWindow, ipcMain} = require("electron");
const path = require("path");
const url  = require("url");
let $      = require("jquery");

// Keep a global reference of the window object, if you don't, the window will
// be closed automatically when the JavaScript object is garbage collected.
let win;

// function KeyDownFn(evt){
//   if(evt.keyCode == 73 && evt.ctrlKey && evt.shiftKey)
//     evt.preventDefault();
// }

function createWindow(){
  // Create the browser window.
  win = new BrowserWindow({width: 800, height: 600});

  // and load the index.html of the app.
  win.loadURL(url.format({
    pathname: path.join(__dirname, "index.html"),
    protocol: "file:",
    slashes: true
  }));

  // function KeyDownFn(evt){
  //   if(evt.keyCode == 73 && evt.ctrlKey && evt.shiftKey)
  //     evt.preventDefault();
  // }

  // Emitted when the window is closed.
  win.on("closed", () => {
    // Dereference the window object, usually you would store windows
    // in an array if your app supports multi windows, this is the time
    // when you should delete the corresponding element.
    win = null
  });
}

// This method will be called when Electron has finished
// initialization and is ready to create browser windows.
// Some APIs can only be used after this event occurs.
app.on("ready", createWindow);

  app.$ = $;

  // Open the DevTools.
  // win.webContents.openDevTools();
  // alert("sdlkfsdlkfjklsdf");

  // $("*").keydown(function(event){
  //   if( event.which == 73){
  //     console.log("IIIIIIIIIIIIIIIII");
  //     // alert("IIIIIIIIIIIIIIIII");
  //     // event.preventDefault();
  //   }
  // });

// Quit when all windows are closed.
app.on("window-all-closed", () => {
  // On macOS it is common for applications and their menu bar
  // to stay active until the user quits explicitly with Cmd + Q
  if (process.platform !== "darwin") {
    app.quit()
  }
});

app.on("activate", () => {
  // On macOS it's common to re-create a window in the app when the
  // dock icon is clicked and there are no other windows open.
  if (win === null) {
    createWindow()
  }
});

// Listen for async message from renderer process
ipcMain.on("async", (event, arg) => {
    // Print 1
    console.log(arg);
    // Reply on async message from renderer process
    event.sender.send("async-reply", 2);
});

// Listen for sync message from renderer process
ipcMain.on("sync", (event, arg) => {
    // Print 3
    console.log(arg);
    // Send value synchronously back to renderer process
    event.returnValue = 4;
    // Send async message to renderer process
    mainWindow.webContents.send("ping", 5);
});

////////////////////////////////////////////////////////////////////////////////

var m = require("./");

function TestAsync(){return new Promise((resolve) => {
  m.doAsyncStuff(123, 5, true, function(error, result){
    console.log("We are now resolving");
    return resolve(result);
  });
})}

app.on("test", (arg) => {
  TestAsync().then(() => {
    console.log("Finished the C++ function!");
  });
});
