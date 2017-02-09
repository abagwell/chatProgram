
This is a simple chat systems that works for a pair of users (chat server and chat client). The workflow is as follows:

1. chatserve starts on host A.
2. chatserve on host A waits on a port (specified by command-line) for a client request. 3. chatclient starts on host B, specifying host A’s hostname and port number on the
command line.
4. chatclient on host B gets the user’s “handle” by initial query (a one-word name, up to
  10 characters). chatclient will display this handle as a prompt on host B, and will
prepend it to all messages sent to host A. e.g., “ SteveO> Hi!!”
5. chatclient on host B sends an initial message to chatserve on host A : PORTNUM.
This causes a connection to be established between Host A and Host B. Host A and host B are now peers, and may alternate sending and receiving messages. Responses from host A should have host A’s “handle” prepended.
6. Host A responds to Host B, or closes the connection with the command “\quit”
7. Host B responds to Host A, or closes the connection with the command “\quit”
8. If the connection is not closed, repeat from 6.
9. If the connection is closed, chatserve repeats from 2 (until a SIGINT is received).
