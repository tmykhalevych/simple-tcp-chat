# tcp-chat
### Client/server solution for chatting


## General pre-build steps

Set path to Boost folder, which should include `/lib` and `/include` subfolders, as BOOST_ROOT environment variable
```sh
export BOOST_ROOT="path_to_built_boost"
```

Set path to C/C++ compilers in [build.sh](./build.sh) file, if they are custom.
Here are values by default:
```sh
C_COMPILER_PATH=/usr/bin/gcc
CXX_COMPILER_PATH=/usr/bin/g++
```

Install Cppcheck
```sh
sudo apt-get install cppcheck
```

## Build client/server

To bild server, run:
```sh
./build.sh server
```
for client, run:
```sh
./build.sh client
```

You can see the build logs in server/client `/build/log` subfolder.
All binaries should be in `/build/bin` server/client subfolders.

To clean all build artifacts run (you can use `--clean` argument instead of `-c`, as you wish):

```sh
./build.sh server -c
./build.sh client -c
```
