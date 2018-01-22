#include "TcpMainService.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>

TcpMainService::TcpMainService()
{
}

TcpMainService::~TcpMainService()
{

}

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
	default:
		break;
	}
	return command;
}

void TcpMainService::tcpServiceLoop(void)
{
	int sock;
	struct sockaddr_in server;
	int msgsock;
	size_t opcode;
	size_t rval;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		perror("opening stream socket");
		exit(1);
	}

	/* dowiaz adres do gniazda */
	/* “prawdziwy” serwer pobrał by port poprzez getservbyname() */
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 38888;
	if (bind(sock, (struct sockaddr *) &server, sizeof server) == -1){
		perror("binding stream socket");
		exit(1);
	}
	/* zacznij przyjmować polaczenia... */
	listen(sock, 5);

	Command * command;
	while(true) {
		msgsock = accept(sock,(struct sockaddr *) 0,(unsigned *) 0);
		if (msgsock == -1 )
			perror("accept");
		else
		{
			memset(&opcode, 0, sizeof(opcode));
			if ((rval = read(msgsock, &opcode, sizeof(opcode))) == -1)
				perror("reading stream message");
			if (rval == 0)
				printf("Ending connection\n");
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
//			do {
//				memset(buf, 0, sizeof buf);
//				if ((rval = read(msgsock,buf, 1024)) == -1)
//					perror("reading stream message");
//				if (rval == 0)
//					printf("Ending connection\n");
//				else
//					printf("-->%s\n", buf);
//			} while (rval != 0);
		close(msgsock);
	}

	pthread_exit(NULL);
}
