#include <stdio.h>
#include <stdlib.h>
#include "NodeInfo.h"
#include "NetMainThread.h"
#include "AddNode.h"
#include <iostream>
#include <pthread.h>

Command * newTerminalCommand(std::string textCommand)
{
	Command * outCommand;
	if(textCommand == "add") {
		outCommand = new AddNode();
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
	std::string userCommand;

	// Get user commands from terminal
	while(1)
	{
		std::cout << "Enter command (join, exit):\n> ";
		std::cin >> userCommand;
		Command * command = newTerminalCommand(userCommand);
		if(command != nullptr)
		{
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

	return 0;
}
