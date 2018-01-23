#include "ReceiveFileTcp.h"

void ReceiveFileTcp::execute(void)
{
	size_t ownerId;
	char buf[1024];
	char hash[33];

	int readBytes;
	if ((readBytes = read(socketFd, hash, 33)) == -1) //read hash
		perror("reading stream message");
	if ((readBytes = read(socketFd, &ownerId, sizeof(size_t))) == -1) //read ownerId
		perror("reading stream message");

	std::ofstream newFile(hash, std::ios::out | std::ios::binary);
	do {
		memset(buf, 0, sizeof(buf));
		if ((readBytes = read(socketFd, buf, 1024)) == -1)
			perror("reading stream message");
		if (readBytes != 0)
			newFile.write(buf, readBytes);
	} while (readBytes != 0);

	newFile.close();
	std::string fileHash(hash);
	NetMainThread::getNodeInfo()->addNewFileEntry(fileHash, ownerId);
	close(socketFd);
}
