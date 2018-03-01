Make sure you install the Windows build tools first, or else `npm run compile` will not work
Installing the build tools must be done in a command prompt with administrative privileges

npm i -g --production windows-build-tools


You need to rebuild your native Node addon for Electron

npm i -g node-gyp
npm i --save-dev electron-rebuild
npm i

# Every time you run "npm install", run this
./node_modules/.bin/electron-rebuild
.\node_modules\.bin\electron-rebuild.cmd

git init
git add -A
git commit -m "Initial files"
git remote add origin https://github.com/TundraFizz/Frost-Frame.git
git tag 1.0.0
git push -u origin --tags master

Frost Frame is a Node.JS application that easily captures screenshots and uploads them to a webserver



export npm_config_disturl=https://atom.io/download/atom-shell
export npm_config_target=0.33.1
export npm_config_arch=x64
export npm_config_runtime=electron
HOME=~/.electron-gyp npm install module-name



Manually building for Electron
If you are a developer developing a native module and want to test it against Electron, you might want to rebuild the module for Electron manually. You can use node-gyp directly to build for Electron:

cd /path-to-module/
HOME=~/.electron-gyp node-gyp rebuild --target=1.7.5 --arch=x64 --dist-url=https://atom.io/download/electron
The HOME=~/.electron-gyp changes where to find development headers. The --target=1.2.3 is version of Electron. The --dist-url=... specifies where to download the headers. The --arch=x64 says the module is built for 64bit system.

node-gyp rebuild --target=1.7.5 --arch=x64 --dist-url=https://atom.io/download/atom-shell
node-gyp build
node-gyp rebuild







npm install --save nan
node-gyp configure
node-gyp build

npm install --save-dev electron-rebuild
./node_modules/.bin/electron-rebuild





npm i
./node_modules/.bin/electron-rebuild
npm start


npm i
./node_modules/.bin/electron-rebuild; npm start

Be sure to delete the `bin` and `build` directories every time you change C++ code
