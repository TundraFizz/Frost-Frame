"use strict"
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

///////////////////////////////////////
app.on("test", (arg) => {
  console.log(arg);

  var worker = require("streaming-worker");
  var through = require("through");
  var addon_path = path.join(__dirname, "build/Release/pow");
  const eo = worker(addon_path);
  eo.to.emit("go", 10);

  setTimeout(function(){
    eo.to.emit("start", 20);
    eo.to.emit("start", -1);
  }, 5000);

  eo.from.on('even_event', function(value){
    console.log("EVEN:  " + value);
  });

  eo.from.on('odd_event', function(value){
    console.log("ODD:   " + value);
  });
});
///////////////////////////////////////

// In this file you can include the rest of your app's specific main process
// code. You can also put them in separate files and require them here.

if(0){

const yolo = require("./build/Release/pow");
console.log(yolo.Reeeeeee());

}else{

// const worker = require("streaming-worker");
// const addon_path = path.join(__dirname, "./build/Release/pow");
// const simple_stream = worker(addon_path);

// console.log(simple_stream.pow(4, 2));

// simple_stream.from.on("integer", function(value){
//   console.log(value);
// });

}
