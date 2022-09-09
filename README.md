# simple-tcp-chat
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

To build server, run:
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
To include runtime STDOUT logging, uncomment following lines in [server/CMakeLists.txt](./server/CMakeLists.txt) and/or [client/CMakeLists.txt](./client/CMakeLists.txt) files (or define _LOG_ON):
```sh
# Enable logging
#add_definitions(-D_LOG_ON)
#add_definitions(-D_SCOPE_LOG_ON)
```
_SCOPE_LOG_ON macro enables output from the scope logs, _SCOPE_LOG macro enable all kind of logs.

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

## Usage

1. Client at startup should set up its name and server password. When the client joins the server, a message with client name is broadcasted:
```sh 
Please enter your nickname: taras
Please enter the room password: 1234
[ROOM] New participant joined the room. Welcome @taras
```

2. Client is able to send public messages - just type and press enter button, also a client can send a private message to another user - the template is: `@user_name private_message`:
```sh
[ROOM] New participant joined the room. Welcome @vlad
[vlad] heey
[vlad:private] Hi taras, here is some private info
```

3. Client with name `Admin` can send a message in format: `@kick user_name` which disconnects client `user_name` if it exists:
```
Please enter your nickname: Admin
Please enter the room password: 1234
[ROOM] New participant joined the room. Welcome @Admin
I'm gonna kick vlad
@kick vlad
[ROOM] Participant @vlad has been kicked from the room. Goodbye.
```
