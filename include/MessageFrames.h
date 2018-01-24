#ifndef SRC_INFOMESSAGE_H_
#define SRC_INFOMESSAGE_H_
#include <cstdlib>

class InfoMessage {
public:
	InfoMessage(size_t opcode = 0);
	size_t opcode;
	size_t firstField;
	size_t secondField;
	size_t thirdField;
};


class DataMessage
{
public:
	DataMessage(size_t _opcode = 0);
	size_t opcode;
	size_t fileCountOrSize;
};

#endif /* SRC_INFOMESSAGE_H_ */
