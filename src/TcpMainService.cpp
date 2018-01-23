#include "TcpMainService.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

Command * TcpMainService::getCommand(size_t opcode, int socketFd)
{
	Command * command = nullptr;
	switch(opcode){
	case 103:
		break;
	case 301:
	case 302:
		command = new FilesTableReceive(opcode, socketFd);
		break;
	case 304:
		command = new ReceiveFileTcp(opcode, socketFd);
		break;
	default:
		break;
	}
	return command;
}

void TcpMainService::tcpServiceLoop(void)
{
	int sock, msgsock, readBytes;
	struct sockaddr_in server;
	size_t opcode;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("opening stream socket");
		exit(1);
	}

	// Bind address to the socket
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *) &server, sizeof server) == -1){
		perror("binding stream socket");
		exit(1);
	}
	// Start listen on tcp socket
	listen(sock, 5);

	Command * command;
	while(true) {
		msgsock = accept(sock,(struct sockaddr *) 0,(unsigned *) 0);
		if (msgsock == -1 )
			perror("accept");
		else
		{
			memset(&opcode, 0, sizeof(opcode));
			if ((readBytes = read(msgsock, &opcode, sizeof(size_t))) == -1)
				perror("reading stream message");
			if (readBytes == 0)
				std::cout << "Ending connection" << std::endl;
			else
			{
				command = getCommand(opcode, msgsock);
				if(command->reqSeparateThread())
				{
					pthread_t thread;
					pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
					pthread_detach(thread);
				}
				else
				{
					command->execute();
					delete command;
				}
			}
		}
		close(msgsock);	// It is not needed in parent thread anymore
	}
	close(sock);
}

void TcpMainService::execute(void)
{
	tcpServiceLoop();
}
