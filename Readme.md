This project contains code for a WS client and a WS server.

# TODO

- consider implementing authentication

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
