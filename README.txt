Name: Elmer Baca Holguin
Class: CSCI 4273 Network Systems
Project: Lab 1 UDP Clients/Server Communiction
---------------------------------------------------------
uftp_client.c

functions:
Main() - takes in two command line arguments for the server ip address and port. A dashboard prints with the commands that the user might want to enter. User input is parsed accordingly and has data sent to the server when needed. All strings buffers are cleared with each iteration of the dashboard printing. The user can exit with the command or using ctrl+c

clearBuffs() - fills any all string buffers with null terminators for usage again

----------------------------------------------------------
uftp_server

functions:
Main() - takes in one command line argument which should be the port number. It then binds this port and waits for some input. This is done in an infinite loop and the information received is parsed. The functions accordiningly respondes depending on what the argument was.

sendFile() - meant to fill the buffer we'll send with either the content of the file requested or with an error message

clearBuffs() - fulls any used character buffers with null terminators for usage again
