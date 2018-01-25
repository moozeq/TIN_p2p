#include <AddFile.h>
#include <stdio.h>
#include <stdlib.h>
#include "NodeInfo.h"
#include "NetMainThread.h"
#include "ListFilesRequest.h"
#include "RemoveFileRequest.h"
#include "GetFile.h"
#include "Leave.h"
#include "PrintP2PStats.h"
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>

Command * newTerminalCommand(std::string textCommand)
{
	Command * outCommand;
	std::string comm, param;
	std::stringstream ss(textCommand);
	ss >> comm >> param;
	if(comm == "add")
		outCommand = new AddFile(param);
	else if(comm == "get")
		outCommand = new GetFile(param);
	else if(comm == "remove")
		outCommand = new RemoveFileRequest(param);
	else if(textCommand == "join")
		outCommand = new NetMainThread();
	else if(textCommand == "list")
		outCommand = new ListFilesRequest();
	else if (textCommand == "leave")
		outCommand = new Leave();
	else if(textCommand == "stat")
		outCommand = new PrintP2PStats();
	else if (textCommand == "exit")
		exit(0);
	else
		outCommand = nullptr;
	return outCommand;
}


int main(void)
{
	std::string userCommand;
	pthread_t thread, netThread;

	// Get user commands from terminal
	while(1)
	{
		std::string userCommand;
		std::cout << "Enter command (join, add, get, list, leave, remove, stat, exit):\n> ";
		getline(std::cin, userCommand);
		Command * command = newTerminalCommand(userCommand);
		if(command != nullptr)
		{
			if(command->reqSeparateThread())
			{
				if (userCommand == "join")
					pthread_create(&netThread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
				else{
					pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
					pthread_detach(thread);
				}
			}
			else
			{
				command->execute();
				delete command;
				if (userCommand == "leave") {
					pthread_join(netThread, NULL);
					return 0;
				}
			}
		}
	}

	return 0;
}
