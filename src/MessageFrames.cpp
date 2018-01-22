#include "MessageFrames.h"

InfoMessage::InfoMessage(size_t _opcode) {
	opcode = _opcode;
	firstField = 0;
	secondField = 0;
	thirdField = 0;
}

DataMessage::DataMessage(size_t _opcode){
	opcode = _opcode;
	fileCountOrSize = 0;
}
