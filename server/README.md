The bible: https://beej.us/guide/bgnet/html/index-wide.html
Structure of a server: https://nmon.sourceforge.io/pmwiki.php?n=Site.Nweb

how to build a WS server: https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_servers#the_websocket_handshake
how to build a WS client: https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API/Writing_WebSocket_client_applications

# TODO

Security considerations and scenario handling:

- Only allow POST requests

If I was to return any data to the client:

- Modify your program to improve the path-parsing logic to handle folders, and handle responses appropriately.
- Modify the permissions on a few dummy folders and files to make their read permissions forbidden to your server program. Implement the 403 response appropriately.
- Parse uri such that it does not allow malicious folder navigation using "..".
- Modify your path-parsing to strip out (and handle) any troublesome characters.
- Handle (ignore) query strings and fragments. (? and #).

# If making it multi-threaded later

Should add:

```
sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }
```

From: https://beej.us/guide/bgnet/html/index-wide.html#a-simple-stream-server
