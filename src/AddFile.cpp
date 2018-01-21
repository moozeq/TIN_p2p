#include "AddFile.h"
#include "NetMainThread.h"
#include "NodeInfo.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <openssl/md5.h>

using namespace std;

void AddFile::execute(void)
{
	stringstream ss(command), ssMD5;
	string comm, filename, fileStr;
	ss >> comm >> filename;
	ifstream file(filename, ios::in | ios::binary);
	if (!file) {
		cout << "Couldn't open file" << endl;
		return;
	}
	file.seekg(0, ios::end);
	fileStr.reserve(file.tellg());
	file.seekg(0, ios::beg);

	fileStr.assign((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());

	unsigned char digest[16];
	MD5((unsigned char*)fileStr.c_str(), fileStr.size(), (unsigned char*)&digest);
	string mdString(32, 'x');
	for(int i = 0; i < 16; i++)
		ssMD5 << hex << (int)digest[i];

	mdString = ssMD5.str();
	cout << mdString << endl << endl;
	NetMainThread::getNodeInfo()->addNewFile(mdString, fileStr, 0);
	cout << "Successfully added file" << endl;
}
