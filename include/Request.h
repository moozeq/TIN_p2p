#ifndef SRC_REQUEST_H_
#define SRC_REQUEST_H_

class Request {
public:
	Request(unsigned opcode = 0);
	unsigned opcode;
};

#endif /* SRC_REQUEST_H_ */
