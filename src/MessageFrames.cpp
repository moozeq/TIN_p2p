#include "MessageFrames.h"
#include <string.h>
#include <string>

InfoMessage::InfoMessage(size_t _opcode) {
	opcode = _opcode;
	firstField = 0;
	secondField = 0;
	thirdField = 0;
}

InfoMessage::InfoMessage(size_t _opcode, size_t nodeId, std::string _hash) {
	opcode = _opcode;
	firstField = nodeId;
	secondField = 0;
	thirdField = 0;
	strncpy(hash, _hash.c_str(), 33);
}

DataMessage::DataMessage(size_t _opcode){
	opcode = _opcode;
	fileCountOrSize = 0;
}
