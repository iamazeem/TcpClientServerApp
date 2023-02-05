# TcpClientServerApp

[![ci](https://github.com/iamazeem/TcpClientServerApp/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/iamazeem/TcpClientServerApp/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-darkgreen.svg?style=flat-square)](./LICENSE)
[![Buy Me a Coffee](https://img.shields.io/badge/Support-Buy%20Me%20A%20Coffee-orange.svg?style=flat-square)](https://www.buymeacoffee.com/iamazeem)

A sample asynchronous protocol-based
[TCP](https://en.wikipedia.org/wiki/Transmission_Control_Protocol) client and
server applications using C++ and
[Boost::Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html)

```mermaid
sequenceDiagram
    participant client
    participant server
    client-->>+server: connect
    server->>client: welcome
    client->>+server: command
    server->>-client: output
    client->>server: exit
    server-->-client: disconnect
```

## Dependencies

- C++11 or above
- [Boost::Asio](https://www.boost.org/doc/libs/1_76_0/doc/html/boost_asio.html)
- [CMake](https://cmake.org/)

## Build

```shell
git clone git@github.com:iamazeem/TcpClientServerApp.git
cd TcpClientServerApp
cmake -S . -B build
cmake --build build
```

The client and server binaries will be generated under `build` directory:

- `./build/server/server`
- `./build/client/client`

## Run

Run `server` on one terminal:

```shell
./build/server/server
```

Run `client` on another terminal:

```shell
./build/client/client
```

## License

[MIT](./LICENSE)
