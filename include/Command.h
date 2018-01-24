#ifndef COMMAND_H_
#define COMMAND_H_

#include <string>

class Command
{
public:
	Command() {}
	virtual ~Command()	{}

	virtual void execute(void) = 0;
	virtual bool reqSeparateThread(void) = 0;

	/**
	 * 	@brief	This is wrapper static method which is function
	 * 	to be executed by new thread together with command to execute
	 *
	 * 	@param	command is pointer to Command object to execute
	 */
	static void * commandExeWrapper(void * command)
	{
		Command * cmd = static_cast<Command *>(command);
		cmd->execute();
		delete cmd;
		return 0;
	}

	/**
	 * 	@brief 	Method which prints fatal error resulting in thread
	 * 	termination. It prints message deletes resources properly and exits thread.
	 *
	 * 	@param 	cmd is command which failed during execution
	 * 	@param	errorMsg is error message to be printed on stderr
	 */
	static void printErrAndDie(Command * cmd, std::string errorMsg)
	{
	    perror(errorMsg.c_str());
	    delete cmd;
	    pthread_exit((void*)nullptr);
	}
};

#endif /* COMMAND_H_ */
