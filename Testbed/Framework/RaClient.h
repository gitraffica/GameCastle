/*
* Author: Raffica
*
* RaClient.h
* RafficaStudio
*
* 对c艹的socket的封装...真鸡儿累
* 进行了基本的握手，在发送信息前询问是否可以使用，在发送信息后询问是否接收到
*
*
*/


#pragma once
#pragma warning (disable:4267)
#pragma warning (disable:4302)
#pragma warning (disable:4311)
#pragma warning (disable:4244)
#pragma warning (disable:4996)

#include "RaImport.h"

using namespace std;

class RaClient {

public:

	int bits;
	string IP;
	SOCKET clientSocket;
	string username;

	int PORT = 1234;


	inline void coutError() {
		cout << "socket错误" << WSAGetLastError() << endl;
	}

	inline void sendLogin(string name, string pass) {
		_send(name.c_str());
		_recv();
		_send(pass.c_str());
		_recv();
		assert(name != "register");
		string res = _recv();
		if (res == "Identify failed!") {
			cout << "Wrong Password or UserName!\n";
			sendLogin(name, pass);
			return;
		} else if (res == "You are already in server!") {
			cout << "You are already in server!\n";
			sendLogin(name, pass);
		} else if (res == "The user has been registered!") {
			cout << "The user has been registered!\n";
			sendLogin(name, pass);
		}
		else {
			username = name;
		}
	}

	inline void sendRegister(string name, string pass) {
		_send("register");
		_recv();
		_send(name.c_str());
		_recv();
		_send(pass.c_str());
		string r = _recv();
		cout << "bbb";
		string res;
		if (r.length() != 4) res = r.substr(4, r.length() - 4);
		else res = _recv();
		cout << res << endl;
		if (res == "Identify failed!") {	
			//cout << "Wrong Password or UserName!\n";
			sendLogin(name, pass);
			return;
		}
		else if (res == "You are already in server!") {
			cout << "You are already in server!\n";
			sendLogin(name, pass);
		}
		else if (res == "The user has been registered!") {
			cout << "The user has been registered!\n";
			sendLogin(name, pass);
		}
		else {
			username = name;
		}
	}

	inline int initSocket(string _IP = "127.0.0.1", int _PORT = 1234) {
		WORD sockVersion = MAKEWORD(2, 2);
		WSADATA data;
		if (WSAStartup(sockVersion, &data) != 0) {
			return -1;
		}
		IP = _IP;
		PORT = _PORT;

		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET) {
			coutError();
			return -1;
		}
		sockaddr_in serAddr;
		serAddr.sin_family = AF_INET;
		serAddr.sin_port = htons(PORT);
		serAddr.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
		if (connect(clientSocket, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR) {
			//连接失败   
			coutError();
			closesocket(clientSocket);
			return -1;
		}

		return 1;
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

	inline void _send(const char* msg = 0, int bufsize = -1) {
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

	char revData[256];

	inline char* _recv() {
		memset(revData, '\0', sizeof(revData));
		int re = recv(clientSocket, revData, 256, 0);
		if (re == SOCKET_ERROR) coutError();
		if (re > 0) {
			revData[re] = '\0';
		}
		bits = re;
		return revData;
	}

	static string GetIp() {
		string Inernet_ip;
		Inernet_ip.resize(32);
		TCHAR szTempPath[_MAX_PATH] = { 0 }, szTempFile[MAX_PATH] = { 0 };
		std::string buffer;
		GetTempPath(MAX_PATH, szTempPath);
		UINT nResult = GetTempFileName(szTempPath, _T("~ex"), 0, szTempFile);

		cout << "IP是:" << Inernet_ip << endl;
		int ret = URLDownloadToFile(NULL, _T("http://automation.whatismyip.com/n09230945.asp"), szTempFile, BINDF_GETNEWESTVERSION, NULL);

		cout << "IP是:" << Inernet_ip << endl;
		if (ret == S_FALSE) {

			cout << "IP是:" << Inernet_ip << endl;
			return 0;
		}
		FILE *fp;
		if (_wfopen_s(&fp, (wchar_t*)(szTempFile), (wchar_t*)_T("rb")) != 0) {

			cout << "IP是:" << Inernet_ip << endl;
			return 0;
		}
		fseek(fp, 0, SEEK_END);//得到文件大小  
		int ilength = ftell(fp);
		fseek(fp, 0, SEEK_SET);
		if (ilength>0)
		{
			buffer.resize(ilength);
			fread(&buffer[0], sizeof(TCHAR), ilength, fp);
			fclose(fp);
			DeleteFile(_T("ip.ini"));

			char* str_ip = strstr(&buffer[0], "[");
			if (str_ip != NULL)
			{
				sscanf_s(str_ip + 1, "%[^]]", &Inernet_ip[0], 32);
			}
			cout << "IP是:" << Inernet_ip << endl;
			return Inernet_ip;
		}
		else
		{
			fclose(fp);

			cout << "IP是:" << Inernet_ip << endl;
			return 0;
		}
	}

	inline void closeSocket() {
		closesocket(clientSocket);
	}

	inline char* sendMsgByHello(const char* msg = 0, int size = -1) {
		string tmp;
		tmp = "Hello I am " + username;
		_send(tmp.c_str());
		char* r = _recv();
		_send(msg, size);
		return _recv();
	}

	/*
	* void getFile(string fileName)
	*
	* 从服务器获得文件
	*/

	inline void getFile(string fileName, string destination = "") {

		//system(("IF NOT EXIST " + destination + " MD " + destination).c_str());
		cout << "一方通行" << fileName << endl;

		ofstream ofs((destination + fileName).c_str(), ios::binary);
		bool flag = false;
		string _file;
		char file[1500];
		while (1) {
			strcpy(file, sendMsgByHello("getting file"));

			if (file[0] == 'R' && file[1] == 'a' && file[2] == 'E' && file[3] == 'n' && file[4] == 'd') {
				cout << "getFile OJBK\n";
				if (!flag) {
					cout << "nothing\n";
					ofs.close();
					system(("del " + fileName).c_str());
				}
				break;
			}
			else {
				if (flag) {
					ofs.write(file, sizeof(char)*bits);
				}
				else flag = true;
			}
		}
		ofs.close();
	}

	/*
	* void sendFile(string fileName)
	*
	* 发送一个文件
	*/

	inline void sendFile(string fileName) {
		ifstream ist(fileName.c_str(), ios::binary);
		string file;
		char _file[256];

		while (!ist.eof()) {
			ZeroMemory(_file, 256);
			ist.read(_file, 256 * sizeof(char));
			int getbits = ist.gcount();

			if (getbits == 0) break;

			_send(("Hello I am " + username).c_str());
			char* r = _recv();
			string hello = r;
			if (hello != "おはよう") { halt(); }
			_send(_file, getbits);
			r = _recv();
			hello = r;
			if (hello.substr(0, 4) != "gett") {
				//处理异常
				if (hello.substr(0, 4) == "rese") {
					while (1) {
						_send(_file, getbits);

						r = _recv();
						hello = r;

						if (hello.substr(0, 4) == "gett") break;
						if (hello.substr(0, 4) != "rese") halt();
					}
				}
				else halt();
			}
			else {
			}
		}

		_send(("Hello I am " + username).c_str());
		string hello;
		char* r;

		r = _recv();
		hello = r;

		if (hello != "おはよう") halt();
		_send("RaEnd");

		r = _recv();
		hello = r;

		if (hello.substr(0, 4) != "gett") halt();
		//cout << "sendFile OJBK\n";
	}

	/*
	* halt(void)
	*
	* 我怀疑收到的是假的client
	*/

	inline void halt() {
		cout << "something got wrong...\n";
		assert(0);
		system("pause");
		closeSocket();
	}

};


/*
void runSocket(string _IP, int _PORT) {
		IP = _IP;
		PORT = _PORT;

		initSocket(IP);
		sendLogin();
		cout << "Login Success! UserName: " << username << endl;

		while (true) {
			string str;
			getline(cin, str);
			if (str.length() == 0) continue;
			if (str == "RaEnd") break;
			cout << sendMsgByHello(str.c_str()) << endl;

			if (str.substr(0, 4) == "get ") {
				string fileName = str.substr(4, str.length());
				getFile(fileName);
			}
			else if (str.substr(0, 5) == "send ") {
				string fileName = str.substr(5, str.length());
				if (_access(fileName.c_str(), 0) == -1) halt();
				sendFile(fileName);
			}
		}


		closeSocket();
		WSACleanup();
	}
*/