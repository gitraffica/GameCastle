/*
* RaClient.cpp
*
* Author: Raffica
*
* 客戶端
*
* 用法：
* 輸入用戶名和密碼
* send *** 向服務器發送*** emmmmmm相對路徑和絕對路徑都可以 二進制文件傳輸可能
* get *** 從服務器接收*** 如果服務器沒這文件的話將不會產生任何文件
* 如果不是這兩個命令的話你可以用來聊天 hhh
* 輸入RaEnd可以結束
* 直接關閉客戶端將導致服務器出現一個錯誤，然而不影響正常使用
* 
*/

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#pragma comment(lib, "ws2_32.lib")  
#pragma warning (disable:4244)
#pragma warning (disable:4267)

#include <WINSOCK2.H>
#include <STDIO.H>  
#include <iostream>  
#include <cstring>
#include <string>
#include <ctime>
#include <fstream>
#include <cstddef>
#include <cassert>
#include <io.h>
#include <thread>
#include "RaClient.h"

#define TEST

RaClient client;
RaClient chatClient;
RaClient PVPClient;

int isRegister;
int SERVER_PORT, SERVER_PORT2, SERVER_PORT3;
string SERVER_IP;
string username;
string password;
string mainUseAttack = "";
thread* chatThread;

using namespace std;