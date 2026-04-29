# Initial Memcached

This project involves working with and extending [memcached](https://memcached.org/), a high-performance, distributed memory object caching system. The goal is to understand the codebase and add new commands for arithmetic operations.

## 🚀 Getting Started

### Prerequisites

- `gcc`
- `make`
- `libevent-dev` (required by memcached)

### Build Instructions

The `memcached` source is included as a tarball. To build the project, follow these steps:

```bash
cd initial-memcached
tar xvzf memcached-1.6.17.tar.gz
cd memcached-1.6.17
./configure
make
```

### Running Memcached

Once built, you can run the server:

```bash
./memcached
```

By default, it listens on port `11211`. You can connect to it using `telnet`:

```bash
telnet 127.0.0.1 11211
```

## 🛠 Features to Implement

- **`mult` command:** Multiply a stored integer value.
- **`div` command:** Perform integer division on a stored value.

Both commands follow the syntax of the existing `incr` and `decr` commands.
