Make sure you install the Windows build tools first, or else `npm run compile` will not work
Installing the build tools must be done in a command prompt with administrative privileges

npm i -g --production windows-build-tools
npm i -g node-gyp

npm install --save nan

npm i
./node_modules/.bin/electron-rebuild; npm start

Run this every time you change the C++ code
node-gyp clean

==================================================

git init
git add -A
git commit -m "Initial files"
git remote add origin https://github.com/TundraFizz/Frost-Frame.git
git tag 1.0.0
git push -u origin --tags master

==================================================

Frost Frame is a Node.JS application that easily captures screenshots and uploads them to a webserver
