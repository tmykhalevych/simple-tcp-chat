# tcp-chat
### Client/server solution for chatting


## General pre-build steps

1. Set path to Boost folder, which should include `/lib` and `/include` subfolders, as BOOST_ROOT environment variable
```sh
export BOOST_ROOT="path_to_built_boost"
```

2. Set path to C/C++ compilers in [build.sh](./build.sh) file, if they are custom.
Here are values by default:
```sh
C_COMPILER_PATH=/usr/bin/gcc
CXX_COMPILER_PATH=/usr/bin/g++
```

3. Install Cppcheck
```sh
sudo apt-get install cppcheck
```

4. Install Google Protocol Buffers library

You can find the instruction [here](https://github.com/protocolbuffers/protobuf/blob/master/src/README.md).
It could take a while)

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
## Configuration

Default solution configurations are defined in [server/include/config.h](./server/include/config.h).

But, you can specify basic client/server configuration as params:
1. Run server on a custon port:
```sh
./server/buid/bin/tcp-chat-server <port>
```
2. Specify server host and port when running client:
```sh
./client/buid/bin/tcp-chat-client <server_host> <server_port>
```
