#include <AddFile.h>
#include <stdio.h>
#include <stdlib.h>
#include "NodeInfo.h"
#include "NetMainThread.h"
#include "ListFilesRequest.h"
#include "RemoveFileRequest.h"
#include "GetFile.h"
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
	pthread_t thread;

	// Get user commands from terminal
	while(1)
	{
		std::string userCommand;
		std::cout << "Enter command (join, add, get, list, remove, stat, exit):\n> ";
		getline(std::cin, userCommand);
		Command * command = newTerminalCommand(userCommand);
		if(command != nullptr)
		{
			if(command->reqSeparateThread())
			{
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

	return 0;
}
