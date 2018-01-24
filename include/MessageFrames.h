#ifndef SRC_INFOMESSAGE_H_
#define SRC_INFOMESSAGE_H_
#include <cstdlib>
#include <string>

class InfoMessage {
public:
	InfoMessage(size_t opcode = 0);
	InfoMessage(size_t _opcode, size_t myId, std::string _hash);
	size_t opcode;
	size_t firstField;
	size_t secondField;
	size_t thirdField;
	char hash[33];
};


class DataMessage
{
public:
	DataMessage(size_t _opcode = 0);
	size_t opcode;
	size_t fileCountOrSize;
};

#endif /* SRC_INFOMESSAGE_H_ */
