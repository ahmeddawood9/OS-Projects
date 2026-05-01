# Concurrent Web Server

A multi-threaded HTTP/1.0 web server implemented in C, featuring thread pooling, custom scheduling policies, and security enhancements.

## Features

- **Thread Pool**: Uses a fixed-size pool of worker threads to handle concurrent HTTP requests.
- **Producer-Consumer Buffer**: A synchronized shared buffer for connection descriptors using mutexes and condition variables.
- **Scheduling Policies**:
  - **FIFO (First-In-First-Out)**: Requests are handled in the order they arrive.
  - **SFF (Smallest File First)**: Prioritizes requests for smaller files to reduce average response time.
- **Security**: Prevents directory traversal attacks by rejecting any path containing `..`.
- **Content Support**: Handles both static files (HTML, images, text) and dynamic content via CGI.

## Usage

Compile the project:
```bash
cd src
make
```

Run the server:
```bash
./wserver [-d basedir] [-p port] [-t threads] [-b buffers] [-s schedalg]
```

### Parameters
- `-d`: Root directory for the server (default: `.`).
- `-p`: Port to listen on (default: `10000`).
- `-t`: Number of worker threads (default: `1`).
- `-b`: Number of request buffers (default: `1`).
- `-s`: Scheduling algorithm, either `FIFO` or `SFF` (default: `FIFO`).

## Testing
A simple client is included for testing:
```bash
./wclient localhost 10000 /index.html
```
