.PHONY: all clean build-server build-shared

all: build-server build-client

build-shared:
	$(MAKE) -C shared

build-server: build-shared
	$(MAKE) -C server

build-client: build-shared
	$(MAKE) -C client

clean:
	$(MAKE) -C shared clean
	$(MAKE) -C server clean
	$(MAKE) -C client clean
