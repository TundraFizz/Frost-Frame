var m = require("./");

function TestAsync(){return new Promise((resolve) => {
  m.doAsyncStuff(123, 5, true, function(error, result){
    console.log("We are now resolving");
    return resolve(result);
  });
})}

TestAsync().then(() => {
  console.log("Finished the C++ function!");
});
