#include "ReceiveFileTcp.h"

void ReceiveFileTcp::execute(void)
{
	char buf[1024];
	size_t fileSize, ownerId;
	std::ofstream newFile;
	int readBytes;
	char header[2 * sizeof(size_t) + 16];	// fileIf, file size, hash (16 bytes)
	if ((readBytes = read(socketFd, header, sizeof(header))) == -1)
		perror("reading stream message");

	fileSize = (size_t)(header);
	std::string fileHash(&header[3], &header[3] + 16);
	ownerId = (size_t)(&header[19]);
	newFile.open(fileHash.c_str(), std::ios::out);

	do {
		memset(buf, 0, sizeof(buf));
		if ((readBytes = read(socketFd, buf, 1024)) == -1)
			perror("reading stream message");
		if (readBytes != 0)
			newFile.write(buf, readBytes);
		fileSize -= readBytes;
	} while (readBytes != 0);
	if(fileSize != 0)
		perror("Socket read error! Not all bytes received");

	newFile.close();
	NetMainThread::getNodeInfo()->addNewFileEntry(fileHash, ownerId);
	close(socketFd);
	pthread_exit(NULL);
}
