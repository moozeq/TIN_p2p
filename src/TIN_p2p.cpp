#include <AddFile.h>
#include <stdio.h>
#include <stdlib.h>
#include "NodeInfo.h"
#include "NetMainThread.h"
#include <iostream>
#include <string>
#include <sstream>
#include <pthread.h>

int joined = 0;
pthread_t netMainThread;

Command * newTerminalCommand(std::string textCommand)
{
	Command * outCommand;
	std::string comm, param;
	std::stringstream ss(textCommand);
	ss >> comm >> param;
	if(comm == "add") {
		outCommand = new AddFile(param);
	}
	else if(textCommand == "join")
		outCommand = new NetMainThread();
	else if (textCommand == "exit")
		exit(0);
	else
		outCommand = nullptr;
	return outCommand;
}


int main(void)
{
	// Get user commands from terminal
	while(1)
	{
		std::string userCommand;
		std::cout << "Enter command (join, add, exit):\n> ";
		getline(std::cin, userCommand);
		Command * command = newTerminalCommand(userCommand);
		if(command != nullptr)
		{
			if(command->reqSeparateThread())
			{
				if (userCommand == "join" && !joined) {
					pthread_create(&netMainThread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
					++joined;
					continue;
				} else if (userCommand == "join" && joined) {
					std::cout << "You've already joined in P2P network" << std::endl;
					continue;
				} else {
					pthread_t thread;
					pthread_create(&thread, NULL, Command::commandExeWrapper, static_cast<void *>(command));
				}
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
