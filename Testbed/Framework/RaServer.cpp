/*
* Author:Raffica
* ���FRaServer
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning (disable:4267)
#pragma warning (disable:4244)

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <map>
#include <vector>
#include "RaServer.h"
#include <thread>
#include <io.h>

using namespace std;

int RaServer::PORT;
SOCKET RaServer::slisten;
map<string, string> RaServer::user;
map<string, bool> RaServerThread::usernameMap;
vector<thread*> RaServerThread::threads;
vector<string> RaServerThread::threadUser;

/*
* coutError(void)
* ������һ��socket����
*/

inline void RaServer::coutError() {
	cout << "socket����" << WSAGetLastError() << endl;
	GG = true;
}

/*
* isHello(char* msg)
*
* �ж�msg�ǲ���Hello
*/

inline bool RaServer::isHello(char* msg) {
	string hello = msg;
	if (hello.substr(0, 5) == "Hello") return true;
	return false;
}

/*
* initUserData()
*
* ��ʼ���û���Ϣ
*/

void RaServer::initUserData() {

	ifstream ins(userAddr);
	string name, password;

	while (!ins.eof()) {
		ins >> name >> password;
		user[name] = password;
	}
	ins.close();
}


/*
* bool login(string username, string password)
*
* username �û��� password ����
*
* ����ֵ bool �Ƿ��¼�ɹ�
*/

bool RaServer::login(string username, string password) {
	if (user[username] == password) return true;
	return false;
}

/*
* int initSocket(int port = 1234);
*
* ��socket�ĳ�ʼ�� ���ʧ�ܷ���-1
* PORTӦ����Ȼ�Ƕ˿ڵ���˼��
*/

int RaServer::initSocket(int _PORT) {
	RaServer::PORT = _PORT;
	//��ʼ��WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) {
		cout << "WSA��ʼ��ʧ��" << WSAGetLastError() << "\n";
		return -1;
	}

	//�����׽���    
	RaServer::slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (RaServer::slisten == INVALID_SOCKET) {
		cout << "�׽��ִ���ʧ��" << WSAGetLastError() << "\n";
		return -1;
	}

	//��IP�Ͷ˿�    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(RaServer::PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//bind(RaServer::slisten, (SOCKADDR*)&sin, sizeof(sin));
	if (::bind(RaServer::slisten, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR) {
		printf("bind ʧ��%d", WSAGetLastError());
		return -1;
	}

	//��ʼ����    
	if (listen(RaServer::slisten, 100) == SOCKET_ERROR) {
		cout << "����ʧ��" << WSAGetLastError() << endl;
		return -1;
	}

	return 1;
}

/*
* SOCKET acceptClient()
*
* acceptһ���ͻ��˵����Ӳ���SOCKET����
*/

inline SOCKET RaServer::acceptClient() {
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);

	SOCKET ret = ::accept(RaServer::slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
	if (ret == INVALID_SOCKET) {
		printf("accept error !");
		return -1;
	}
	return ret;
}

/*
* void _send(char* msg, int bufsize = -1)
*
* sClient ����socket����
* msg������Ҫ������Ϣ
* bufsize������Ҫ������Ϣ���ֽ���
* bufsizeĬ��Ϊ-1����ʱbufsizeΪmsg��strlen
*
*/

void RaServer::_send(const char* msg = 0, int bufsize = -1) {
	if (bufsize == -1) bufsize = strlen(msg);
	int re2 = (int)send(clientSocket, msg, bufsize, 0);
	if (re2 == SOCKET_ERROR) coutError();
}

/*
* char* _recv()
*
* ������Ϣ
* ֻ����bufsize == 256�Ķ���
*
*/

inline char* RaServer::_recv() {
	char *revData = new char[256];
	memset(revData, '\0', sizeof(revData));
	int re = recv(clientSocket, revData, 256, 0);
	if (re == SOCKET_ERROR) coutError();
	if (re > 0) {
		revData[re] = '\0';
	}
	bits = re;
	return revData;
}

void RaServer::closeSocket() {
	closesocket(clientSocket);
}

/*
* getFile(string fileName)
*
* �ӿͻ��˻���ļ�
*/

void RaServer::getFile(string fileName) {

	ofstream ofs((fileName).c_str(), ios::binary);
	bool flag = false;
	string _file;
	char *file;
	while (1) {
		if (!isHello(_recv())) halt();
		_send("���Ϥ褦");

		file = _recv();
		_file = file;
		if (_file.substr(0, 5) == "RaEnd") {
			_send("getting file");
			if (!flag) {
				ofs.close();
				system(("del " + fileName).c_str());
			}
			ofs.close();
			break;
		}
		else {
			if (!flag) flag = true;
			ofs.write(file, sizeof(char)*bits);
		}
		_send("getting file");

		free(file);
	}
}

/*
* sendFile(string fileName)
*
* fileName �ļ���
* ��client����һ���ļ�
*/

void RaServer::sendFile(string fileName) {
	ifstream ist(fileName.c_str(), ios::binary);
	string file;
	char _file[256];

	while (!ist.eof()) {
		ZeroMemory(_file, 256);
		ist.read(_file, 256 * sizeof(char));
		int getbits = ist.gcount();
		string hello;

		hello = _recv();
		if (hello.substr(0, 5) != "Hello") {
			halt();
			//�����쳣
		}
		_send("���Ϥ褦");

		hello = _recv();
		if (hello.substr(0, 4) != "gett") {
			//�����쳣
			if (hello.substr(0, 4) == "rese") {
				while (1) {
					_send(_file, getbits);
					hello = _recv();
					if (hello.substr(0, 4) == "gett") break;
					if (hello.substr(0, 4) != "rese") halt();
				}
			}
			else halt();
		}
		else {
			_send(_file, getbits);
		}
	}

	string hello;
	hello = _recv();
	if (hello.substr(0, 4) != "Hell") halt();
	_send("���Ϥ褦");
	hello = _recv();
	if (hello.substr(0, 4) != "gett") halt();
	_send("RaEnd");
}

/*
* halt(void)
*
* �һ����յ����Ǽٵ�client
*/

void RaServer::halt() {
	cout << "something got wrong...\n";
	closeSocket();
	GG = true;
}

/*
* ���캯��
*/

RaServer::RaServer() {
	GG = false;
	clientSocket = slisten = 0;
}


void RaServerThread::restartThread(int ID) {					//���˼�x �؆�����
	string user = threadUser[ID];
	usernameMap[user] = false;
	threadUser[ID] = "";
	threads[ID - 1] = new thread(threadFunc, ID);
}

int RaServerThread::threadFunc(int ID) {						//����
	string param, src;
	string op;
	string fileName;
	RaServer server;

	server.clientSocket = RaServer::acceptClient();			//��listenSocket�Y��accept����clientSocket
	servers.push_back(server);

	string mode = "normal";									//�ஔ�һ���R�r׃��....���a���h��

															/****���*****/

	while (1) {
		if (server.GG) {									//���a���u���horz
			restartThread(ID);
			return -1;
		}
		string name = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}
		string pass = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}													//�@ȡ�Ñ������ܴa
		if (name == "register") {							//����յ�����register
			name = pass;
			pass = server._recv();
			if (RaServer::user.find(name) == RaServer::user.end()) {			//�@�Ñ������]���^�ˆ᣿
				ofstream ost(userAddr, ios::app);
				ost << endl << name << ' ' << pass;
				ost.close();
				usernameMap[name] = 1;
				threadUser[ID] = name;
				RaServer::user[name] = pass;
				server._send(("Welcome " + name).c_str());
				break;
			}
			else {										//�Ĵ_���]���^��
				server._send("The user has been registered!");
				continue;
			}
		}
		if (server.login(name, pass)) {
			if (usernameMap.find(name) == usernameMap.end()) {		//����Ñ��]�ڷ�����Ո������Y��Ԓ
				usernameMap[name] = 1;
				threadUser[ID] = name;
				server._send(("Welcome " + name).c_str());
				break;
			}
			else {													//guna��߀���2��
				if (usernameMap[name] == true) server._send("You are already in server!");
				else {
					usernameMap[name] = 1;
					threadUser[ID] = name;
					server._send(("Welcome " + name).c_str());
					break;
				}
			}
		}
		else {														//�ܴa�e�`
			server._send("Identify failed!");
		}
	}

	//ѭ����������    
	while (1) {
		/********************�ж�username***************************/
		string username = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}

		cout << username << endl;
		if (username.substr(0, 11) == "Hello I am ") {				//�ք�ģ�M��һ������
			server._send("���Ϥ褦");								//Ŷ����~
			if (server.GG) {
				restartThread(ID);
				return -1;
			}
		}
		else {
			//�����쳣
			server.halt();
			return -1;
		}

		//׼�������ļ�

		string msg = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}

		int ___k = 0;							//����������
		{					//����һ�L�δ��a�����ַ���̎��...
			src.clear();
			param.clear();
			op.clear();
			for (int i = 0; i < msg.length(); i++) {
				if (msg[i] != ' ') {
					param.push_back(msg[i]);
				}

				if (___k == 0) {
					if (msg[i] == ' ') {
						___k++;
						op = param;
						param.clear();
					}
				}

				else if (___k == 1) {						//��Ҫ©��else
					if (msg[i] == ' ') {
						___k++;
						src = param;
						break;
						param.clear();
					}
				}

			}
			if (param.length() != 0) {
				___k++;
				src = param;
			}
			cout << op;
			if (op == "get") {								//�õģ��F��������get��Ҫ�l���ļ��ˣ�
				if (___k == 2) {
					fileName = src;
					cout << "�����ļ�" << src << endl;
					server._send("Ready to send it");		//�ʂ�l�ͣ�
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					string Hello = server._recv();
					if (Hello.substr(0, 5) != "Hello") {
						server.halt();
						return -1;
					}
					server._send("���Ϥ褦");				//Ŷ����~
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					Hello = server._recv();
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					if (_access(src.c_str(), 0) == -1) {		//�����͛]�@�ļ���
						if (Hello.substr(0, 5) == "getti") {	//�ք�ģ�M����
							server._send("RaEnd");				//�Y��
						}
						else {
							server.halt();
							return -1;
						}
						continue;
					}
					else {
						if (Hello.substr(0, 5) != "getti") {	//����
							server.halt();
							return -1;
						}
						server._send("Ready to send file!");	//��Ҫ���ˣ�
						server.sendFile(fileName);
					}
				}
				else {
					cout << "�����ļ���������\n";
					server.halt();
					return -1;
				}
			}
			else if (op == "send") {							// �f�͑���Ҫ�o�Ұl���ļ�
				if (___k == 2) {
					fileName = src;
					cout << "�����ļ�" << src << endl;
					server._send("Ready to get it");
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					server.getFile(fileName);
				}
				else {
					cout << "�����ļ���������\n";
					server.halt();
					return -1;
				}
			}
			else {
				server._send("received");
			}
		}

	}
	return 0;
}

void RaServerThread::runThread() {
	for (int i = 0; i < maxThread; i++) {
		thread* t = new thread(threadFunc, i + 1);
		threads.push_back(t);
		threadUser.push_back((string)"");
	}
	threadUser.push_back((string)"");
}