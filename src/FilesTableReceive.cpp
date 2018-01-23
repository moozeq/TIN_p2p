#include "FilesTableReceive.h"

#include <iostream>

void FilesTableReceive::execute(void)
{
	char buf[1024];
	size_t filesCount;
	int readBytes;
	int fileNumber = 0;
	if ((readBytes = read(socketFd, &filesCount, sizeof(filesCount))) == -1)
		perror("reading stream message");

	do {
		memset(buf, 0, sizeof(buf));
		if ((readBytes = read(socketFd,buf, 1024)) == -1)
			perror("reading stream message");
		if (readBytes != 0)
		{
			if(opcode == 302)
			{
				for(unsigned i = 0; i < readBytes/sizeof(size_t); ++i)
				{
					std::cout<<"File " << fileNumber++ <<" id: "
							<< static_cast<size_t>(buf[i * sizeof(size_t)]) << std::endl;
				}
			}
			else if(opcode == 303)
			{
				for(unsigned i = 0; i < readBytes/(2 * sizeof(size_t)); ++i)
				{
					std::cout<<"File " << fileNumber++ <<" id: "
							<< static_cast<size_t>(buf[2 * i * sizeof(size_t)])
							<< ", owner id: " << static_cast<size_t>(buf[4 + 2 * i * sizeof(size_t)])
							<< std::endl;
				}
			}
		}
	} while (readBytes != 0);
	close(socketFd);
}
