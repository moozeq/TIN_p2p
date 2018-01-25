#include "FilesTableReceive.h"

#include <iostream>

void FilesTableReceive::execute(void)
{
	const unsigned bufSize = 25*32; // 32 is size of hash (string)
	char buf[bufSize];
	int readBytes;
	std::string tmpHash;

	do {
		memset(buf, 0, sizeof(buf));
		if ((readBytes = read(socketFd,buf, bufSize)) == -1)
			perror("reading stream message");
		if (readBytes != 0)
		{
			if(opcode == 302)
			{
				for(unsigned i = 0; i < (unsigned)readBytes/32; ++i)
				{
					tmpHash.assign(&buf[i*32], 32);
					std::cout<<"File with id: "
							<< tmpHash << std::endl;
				}
			}
		}
	} while (readBytes != 0);
	close(socketFd);
}
