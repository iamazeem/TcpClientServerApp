# TcpClientServerApp

This repo contains the asynchronous protocol-based
[TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol) client and
server applications using
[Boost::Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html).

The `message.hpp/cpp` and `packet.hpp/cpp` header and source files contain the
message types and format that both the client and server can understand.

## Build

Prerequisites: [CMake](https://cmake.org/)

Clone the repo via SSH:

```shell
git clone git@github.com:iamazeem/TcpClientServerApp.git
```

or HTTPS:

```shell
git clone https://github.com/iamazeem/TcpClientServerApp.git
```

and follow the instructions below.

### Server

On a terminal, run:

```shell
cd Server
mkdir build
cd build
cmake ..
make
```

### Client

On another terminal, run:

```shell
cd Client
mkdir build
cd build
cmake ..
make
```

## License

[MIT](./LICENSE)
