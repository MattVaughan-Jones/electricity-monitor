# Current PR TODO

Next:
- Make sure I can start/stop multiple times and it creates multiple files, without restarting the server
- Refactor
    - move all data/fs-related stuff to a module
    - move set_recording_name out of device_control.c
    - other?
Next:
- Continuously write incoming data to that file
Next
If I was to return any data to the client:
- Modify your program to improve the path-parsing logic to handle folders, and handle responses appropriately.
- Modify the permissions on a few dummy folders and files to make their read permissions forbidden to your server program. Implement the 403 response appropriately.
- Parse uri such that it does not allow malicious folder navigation using "..".
- Modify your path-parsing to strip out (and handle) any troublesome characters.
- Handle (ignore) query strings and fragments. (? and #).

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
