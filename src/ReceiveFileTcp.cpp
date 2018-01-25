#include "ReceiveFileTcp.h"

void ReceiveFileTcp::execute(void)
{
	size_t ownerId;
	char buf[1024];
	char hash[33];

	int readBytes;
	if ((readBytes = read(socketFd, hash, 33)) < 0) //read hash
		perror("reading stream message");
	if ((readBytes = read(socketFd, &ownerId, sizeof(size_t))) < 0) //read ownerId
		perror("reading stream message");

	std::ofstream newFile(hash, std::ios::out);
	do {
		memset(buf, 0, sizeof(buf));
		if ((readBytes = read(socketFd, buf, 1024)) < 0)
			perror("reading stream message");
		if (readBytes != 0)
			newFile.write(buf, readBytes);
	} while (readBytes != 0);

	newFile.close();
	std::string fileHash(hash);
	if (opcode == 304)
		NetMainThread::getNodeInfo()->addNewFileEntry(fileHash, ownerId);
	std::cout << "Added new file:\n\tFile's hash: " << fileHash
			<< "\n\tFile's owner ID: "<< ownerId << std::endl;
	close(socketFd);
}
