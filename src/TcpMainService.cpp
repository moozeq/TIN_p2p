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
	case 304:
		command = new ReceiveFileTcp(opcode, socketFd);
		break;
	case 302:
		command = new FilesTableReceive(opcode, socketFd);
		break;
	default:
		break;
	}
	return command;
}

void TcpMainService::tcpServiceLoop(void)
{
	int sock, readBytes;
	struct sockaddr_in server, client;
	size_t opcode;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		perror("opening stream socket");
		exit(1);
	}

	// Bind address to the socket
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(NetMainThread::port);
	if (bind(sock, (struct sockaddr *) &server, sizeof server) < 0){
		perror("binding stream socket");
		exit(1);
	}
	// Start listen on tcp socket
	listen(sock, 5);

	Command * command;
	while(true) {
		int msgsock;
		unsigned addrlen = sizeof(client);
		if((msgsock = accept(sock,(struct sockaddr *)&client,&addrlen)) < 0) {
			perror("accept");
			break;
		}
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
	}
	close(sock);
}

void TcpMainService::execute(void)
{
	tcpServiceLoop();
}
