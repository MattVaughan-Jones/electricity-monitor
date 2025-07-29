The bible: https://beej.us/guide/bgnet/html/index-wide.html
Pretty much what I want in the end: https://www.geeksforgeeks.org/c/socket-programming-cc/

# Project guidance

In terms of HTTP, the only resource you need is the HTTP standard itself. https://tools.ietf.org/html/rfc1945

For now, you're only going to concern yourself with:

What constitutes a properly-formatted request.

The conditions that would result in following properly-formatted responses: 200 OK, 400 Bad Request, 403 Forbidden, 404 Not Found, 500 Internal Service Error, and 501 Not Implemented (These are the codes you're going to implement)

For now, ignore everything related to headers other than noting that headers exist.

In terms of the sockets stuff, use Beej's guide. https://beej.us/guide/bgnet/. At its simplest, your program needs to:

Create a socket

Bind the socket to an address

Listen on the address

Block on Accept until a connection is made

Read on the connected socket

Figure out how to respond

Write back on the connected socket

Close the connection

Go back to blocking on Accept

That out of the way, here's a quick checklist to give your project a little structure:

Write a program that accepts a connection on a port (specify the port number as a command line argument), and immediately sends back a dummy HTTP 1.0 "200 OK" response, along with a dummy minimal HTML-encoded message before closing the connection. For the entire project, you're going to respond with HTTP 1.0 responses regardless of what version of the request you receive. Test this using netcat, then try it using a web browser.

Modify your program to parse the request. You can ignore all of the headers for now. For now, you're only responding to validly formatted GET requests. Send the dummy message back for any validly formatted GET requests. If the request is improperly formatted, respond 400. For any other valid requests apart from GET requests, respond with 501.

Modify your program to take another command line argument for the "root" directory. Make a directory somewhere, and put a dummy HTML file called index.html and another dummy HTML file called whatever you want. Add a dummy image file as well. When your server starts up, verify that the folder exists and that your program has permissions to view the contents. Modify your program to parse the path from valid GET requests. Upon parsing the path, check the root folder to see if a file matches that filename. If so, respond 200, read the file and write the file to the client socket. If the path is "/" (ie. the root directory) serve index.html. If the requested file does not exist, respond 404 not found. Make sure your solution works for text files as well as binaries (ie. images).

Add a couple of folders to your root folder, and add dummy html files (and dummy index.html files) to them. Add a few levels of nested folders. Modify your program to improve the path-parsing logic to handle folders, and handle responses appropriately.

Modify the permissions on a few dummy folders and files to make their read permissions forbidden to your server program. Implement the 403 response appropriately. Scrutinize the URI standard, and modify your path-parsing to strip out (and handle) any troublesome characters. Modify your path-parsing to handle (ignore) query strings and fragments. (? and #). Modify your path-parsing to ensure it does not allow malicious folder navigation using "..".

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
