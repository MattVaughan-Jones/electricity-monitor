# Current PR TODO

- Send a recording name from client to server - done
- Refactor more things to use send_response() instead of having lots of places build_req - done

Next
- Update server to handle POST request with the recording name
- If name is an empty string, server sets a default

Next:
- change ws_fd shared memory to a struct which now also holds the filename of the current recording
    - Set the filename when receiving the start instruction from UI
    - Delete the filename when receiving the stop instruction from UI

Next:
- Create a new file when receiving a new filename
- Continuously write incoming data to that file

This project contains code for a WS client and server, and a UI to control the client.

# How I'd like it to work:
- user plugs in the device which runs the setup function
- user accesses the web page, enters the name of the appliance being measured and clicks a "record" button
- also implement a "stop recording" button
- server starts receiving data from the ESP32 and sends it to the web page to be displayed as a graph
- data is stored on the server (this laptop) so the user can go back and look at old recordings

# TODO

- consider implementing authentication. Could be interesting.
- Show a "connected" indicator in the ui
- I'm currently keeping all file descriptors in the server. Could implement connection pooling or proper connection lifecycle to close connections that aren't used. This can be ignored for now.

# How to build for deployment

## Prerequisites

- clang compiler
- make
- bear (for generating compile_commands.json) `brew install bear`

## Build Commands

from the `electricity-monitor/` root directory:

```bash
# Build everything
make all

# Build only server
make build-server

# Build only client
make build-client

# Build shared library
make build-shared
```

## Separate Machine Deployment

1. Clone the entire repository on both machines
2. Run `make build-server` on server machine
3. Run `make build-client` on client machine
4. The shared code is automatically compiled and linked into each executable

# How to run locally

## Start the server

```bash
cd server
make run
```

## Run the client

```bash
cd client
make run
```

## Development with VSCode + clangd

### Generate compile_commands.json

```bash
# After any Makefile changes or new files
bear -- make clean
bear -- make all
```

### When to regenerate

- Modified Makefile
- Added new source files
- Changed compiler flags
- Changed include paths
