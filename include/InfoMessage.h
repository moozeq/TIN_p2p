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

#endif /* SRC_INFOMESSAGE_H_ */
