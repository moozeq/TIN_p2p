#ifndef PRINTP2PSTATS_H_
#define PRINTP2PSTATS_H_

#include "Command.h"
#include <arpa/inet.h>
#include <functional>

class PrintP2PStats: public Command
{
public:
	PrintP2PStats() = default;
	virtual ~PrintP2PStats() = default;
	void execute(void);
	bool reqSeparateThread(void) { return false; }

private:
	void printNodeInfo(size_t nodeId, struct in_addr *);
};

#endif /* PRINTP2PSTATS_H_ */
