# TcpClientServerApp

[![ci](https://github.com/iamazeem/TcpClientServerApp/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/iamazeem/TcpClientServerApp/actions/workflows/ci.yml)

This repo contains the asynchronous protocol-based
[TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol) client and
server applications using
[Boost::Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html).

## Build

Prerequisites:

- [Boost::Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html)
- [CMake](https://cmake.org/)

Clone and build:

```shell
git clone git@github.com:iamazeem/TcpClientServerApp.git
cd TcpClientServerApp
mkdir build
cd build
cmake ..
make
```

The client and server binaries will be generated:

- `./server/server`
- `./client/client`

## Run

Run `server` on one terminal:

```shell
./server/server
```

Run `client` on another terminal:

```shell
./client/client
```

## License

[MIT](./LICENSE)
