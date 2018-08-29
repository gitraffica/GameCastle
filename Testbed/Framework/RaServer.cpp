/*
* Author:Raffica
* 實現RaServer
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
* 输出最后一个socket错误
*/

inline void RaServer::coutError() {
	cout << "socket错误" << WSAGetLastError() << endl;
	GG = true;
}

/*
* isHello(char* msg)
*
* 判断msg是不是Hello
*/

inline bool RaServer::isHello(char* msg) {
	string hello = msg;
	if (hello.substr(0, 5) == "Hello") return true;
	return false;
}

/*
* initUserData()
*
* 初始化用户信息
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
* username 用户名 password 密码
*
* 返回值 bool 是否登录成功
*/

bool RaServer::login(string username, string password) {
	if (user[username] == password) return true;
	return false;
}

/*
* int initSocket(int port = 1234);
*
* 对socket的初始化 如果失败返回-1
* PORT应该显然是端口的意思吧
*/

int RaServer::initSocket(int _PORT) {
	RaServer::PORT = _PORT;
	//初始化WSA
	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0) {
		cout << "WSA初始化失败" << WSAGetLastError() << "\n";
		return -1;
	}

	//创建套接字    
	RaServer::slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (RaServer::slisten == INVALID_SOCKET) {
		cout << "套接字创建失败" << WSAGetLastError() << "\n";
		return -1;
	}

	//绑定IP和端口    
	sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(RaServer::PORT);
	sin.sin_addr.S_un.S_addr = INADDR_ANY;
	//bind(RaServer::slisten, (SOCKADDR*)&sin, sizeof(sin));
	if (::bind(RaServer::slisten, (SOCKADDR*)&sin, sizeof(sin)) == SOCKET_ERROR) {
		printf("bind 失败%d", WSAGetLastError());
		return -1;
	}

	//开始监听    
	if (listen(RaServer::slisten, 100) == SOCKET_ERROR) {
		cout << "监听失败" << WSAGetLastError() << endl;
		return -1;
	}

	return 1;
}

/*
* SOCKET acceptClient()
*
* accept一个客户端的连接并将SOCKET返回
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
* sClient 就是socket名字
* msg就是你要发的消息
* bufsize就是你要发的消息的字节数
* bufsize默认为-1，此时bufsize为msg的strlen
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
* 接收消息
* 只接受bufsize == 256的东西
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
* 从客户端获得文件
*/

void RaServer::getFile(string fileName) {

	ofstream ofs((fileName).c_str(), ios::binary);
	bool flag = false;
	string _file;
	char *file;
	while (1) {
		if (!isHello(_recv())) halt();
		_send("おはよう");

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
* fileName 文件名
* 向client发送一个文件
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
			//处理异常
		}
		_send("おはよう");

		hello = _recv();
		if (hello.substr(0, 4) != "gett") {
			//处理异常
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
	_send("おはよう");
	hello = _recv();
	if (hello.substr(0, 4) != "gett") halt();
	_send("RaEnd");
}

/*
* halt(void)
*
* 我怀疑收到的是假的client
*/

void RaServer::halt() {
	cout << "something got wrong...\n";
	closeSocket();
	GG = true;
}

/*
* 構造函數
*/

RaServer::RaServer() {
	GG = false;
	clientSocket = slisten = 0;
}


void RaServerThread::restartThread(int ID) {					//顧名思義 重啓線程
	string user = threadUser[ID];
	usernameMap[user] = false;
	threadUser[ID] = "";
	threads[ID - 1] = new thread(threadFunc, ID);
}

int RaServerThread::threadFunc(int ID) {						//線程
	string param, src;
	string op;
	string fileName;
	RaServer server;

	server.clientSocket = RaServer::acceptClient();			//從listenSocket裏面accept到的clientSocket
	servers.push_back(server);

	string mode = "normal";									//相當於一個臨時變量....代碼好醜啊

															/****登錄*****/

	while (1) {
		if (server.GG) {									//代碼好雞兒醜orz
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
		}													//獲取用戶名和密碼
		if (name == "register") {							//如果收到的是register
			name = pass;
			pass = server._recv();
			if (RaServer::user.find(name) == RaServer::user.end()) {			//這用戶名被註冊過了嗎？
				ofstream ost(userAddr, ios::app);
				ost << endl << name << ' ' << pass;
				ost.close();
				usernameMap[name] = 1;
				threadUser[ID] = name;
				RaServer::user[name] = pass;
				server._send(("Welcome " + name).c_str());
				break;
			}
			else {										//的確被註冊過了
				server._send("The user has been registered!");
				continue;
			}
		}
		if (server.login(name, pass)) {
			if (usernameMap.find(name) == usernameMap.end()) {		//如果用戶沒在服務器請求隊列裏的話
				usernameMap[name] = 1;
				threadUser[ID] = name;
				server._send(("Welcome " + name).c_str());
				break;
			}
			else {													//guna你還想登2次
				if (usernameMap[name] == true) server._send("You are already in server!");
				else {
					usernameMap[name] = 1;
					threadUser[ID] = name;
					server._send(("Welcome " + name).c_str());
					break;
				}
			}
		}
		else {														//密碼錯誤
			server._send("Identify failed!");
		}
	}

	//循环接收数据    
	while (1) {
		/********************判定username***************************/
		string username = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}

		cout << username << endl;
		if (username.substr(0, 11) == "Hello I am ") {				//手動模擬第一次握手
			server._send("おはよう");								//哦哈喲~
			if (server.GG) {
				restartThread(ID);
				return -1;
			}
		}
		else {
			//处理异常
			server.halt();
			return -1;
		}

		//准备接收文件

		string msg = server._recv();
		if (server.GG) {
			restartThread(ID);
			return -1;
		}

		int ___k = 0;							//参数的序数
		{					//接下來一長段代碼都在字符串處理...
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

				else if (___k == 1) {						//不要漏了else
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
			if (op == "get") {								//好的！現在命令是get你要發送文件了！
				if (___k == 2) {
					fileName = src;
					cout << "传输文件" << src << endl;
					server._send("Ready to send it");		//準備發送！
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					string Hello = server._recv();
					if (Hello.substr(0, 5) != "Hello") {
						server.halt();
						return -1;
					}
					server._send("おはよう");				//哦哈喲~
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					Hello = server._recv();
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					if (_access(src.c_str(), 0) == -1) {		//根本就沒這文件嘛
						if (Hello.substr(0, 5) == "getti") {	//手動模擬握手
							server._send("RaEnd");				//結束
						}
						else {
							server.halt();
							return -1;
						}
						continue;
					}
					else {
						if (Hello.substr(0, 5) != "getti") {	//握手
							server.halt();
							return -1;
						}
						server._send("Ready to send file!");	//我要射了！
						server.sendFile(fileName);
					}
				}
				else {
					cout << "传输文件参数错误\n";
					server.halt();
					return -1;
				}
			}
			else if (op == "send") {							//聽說客戶端要給我發送文件
				if (___k == 2) {
					fileName = src;
					cout << "接收文件" << src << endl;
					server._send("Ready to get it");
					if (server.GG) {
						restartThread(ID);
						return -1;
					}
					server.getFile(fileName);
				}
				else {
					cout << "接收文件参数错误\n";
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