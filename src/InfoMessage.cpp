#include "InfoMessage.h"

InfoMessage::InfoMessage(unsigned opcode) {
	this->opcode = opcode;
	firstField = 0;
	secondField = 0;
	thirdField = 0;
}

