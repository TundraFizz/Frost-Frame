{
  "targets": [{
    "target_name": "pow",
    "sources": [ "pow.cpp" ],
    "cflags": ["-Wall", "-std=c++11"],
    "cflags!": [ '-fno-exceptions' ],
    "cflags_cc!": [ '-fno-exceptions' ],
    "include_dirs": [
      "<!(node -e \"require('nan')\")",
      "<!(node -e \"require('streaming-worker-sdk')\")"
    ]
  }]
}
