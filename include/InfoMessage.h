#ifndef SRC_INFOMESSAGE_H_
#define SRC_INFOMESSAGE_H_

class InfoMessage {
public:
	InfoMessage(unsigned opcode = 0);
	unsigned opcode;
	unsigned firstField;
	unsigned secondField;
	unsigned thirdField;
};

#endif /* SRC_INFOMESSAGE_H_ */
