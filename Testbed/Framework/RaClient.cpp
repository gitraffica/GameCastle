/*
* RaClient.cpp
*
* Author: Raffica
*
* �͑���
*
* �÷���
* ݔ���Ñ������ܴa
* send *** ��������l��*** emmmmmm����·���ͽ^��·�������� ���M���ļ���ݔ����
* get *** �ķ���������*** ����������]�@�ļ���Ԓ�������a���κ��ļ�
* ��������@�ɂ������Ԓ������Á����� hhh
* ݔ��RaEnd���ԽY��
* ֱ���P�]�͑��ˌ����·��������Fһ���e�`��Ȼ����Ӱ�����ʹ��
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