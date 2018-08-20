# File Transfer Application
Simple file transfer application using Client Server Architecture.

The server process and the client process runs on two different machines and the communication between the two processes is achieved using Sockets.
The server task can be summarized as follows :
 The server starts running before any client, and wait for connections.
 When the server gets a client, forks and, let the child process take care of the client in a separate function, called serviceClient, while the parent process goes back to wait for the next client.
 Then, the server’s child process
1. Gets in an infinite loop then :
– Reads a command from the client’s socket, the command can be one of get fileName, put fileName or quit.
– If the client sends quit, then the server’s child, closes socket and quits.
– If the client sends get fileName, then (if file exists) the server’s child open the file and writes all its contents to the client’s socket. The client saves it locally.  ASCII code 4 (CTR-D) can be sent by the server to signal the end of file to the client.
– If the client sends put fileName, then the server’s child reads all teh file’s contents from socket and saves it locally. ASCII code 4 (CTR-D) can be sent by client to signal the end of file to the client.
