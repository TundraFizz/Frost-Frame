You need to rebuild your native Node addon for Electron

npm i -g node-gyp
npm i --save-dev electron-rebuild

# Every time you run "npm install", run this
./node_modules/.bin/electron-rebuild

# On Windows if you have trouble, try:
.\node_modules\.bin\electron-rebuild.cmd



git init
git add -A
git commit -m "Initial files"
git remote add origin https://github.com/TundraFizz/Frost-Frame.git
git tag 1.0.0
git push -u origin --tags master

Frost Frame is a Node.JS application that easily captures screenshots and uploads them to a webserver

Make sure you install the Windows build tools first, or else `npm run compile` will not work. Installing the build tools must be done in a command prompt with administrative privileges

npm i -g --production windows-build-tools



export npm_config_disturl=https://atom.io/download/atom-shell
export npm_config_target=0.33.1
export npm_config_arch=x64
export npm_config_runtime=electron
HOME=~/.electron-gyp npm install module-name
