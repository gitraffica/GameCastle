/*
* Author: Raffica
* 
* RaServer.h
* RafficaStudio
* 
* ��cܳ��socket�ķ�װ...�漦����
* �����˻��������֣��ڷ�����Ϣǰѯ���Ƿ����ʹ�ã��ڷ�����Ϣ��ѯ���Ƿ���յ�
* 
* 
*/

#pragma once

#include <iostream>
#include <cstdio>
#include "RaImport.h"
#include <thread>

using namespace std;

#define userAddr "user.txt"				//����Ñ�����Ϣ�����ģ�

#define maxThread 2							//��󾀳̔�

#define __PORT 1234						//�˿�

class RaServer {

public:

	int bits;

	static int PORT;
	string IP;
	SOCKET clientSocket;
	static SOCKET slisten;
	string username;
	static map<string, string> user;

	bool GG;													//�@��socket��������

	void coutError();

	bool isHello(char* msg);

	static void initUserData();
	
	bool login(string username, string password);

	static int initSocket(int _PORT = __PORT);

	void _send(const char* msg, int bufsize);

	char* _recv();

	void closeSocket();

	void getFile(string fileName);

	void sendFile(string fileName);

	void halt();

	static SOCKET acceptClient();


	RaServer();

};

class RaServerThread {

public:

	static map<string, bool> usernameMap;					//�@���Ñ����������Ñ��Ƿ��ھ���
	static vector<thread*> threads;						//�M�̳أ��Ҵ�������˂�݆��
	static vector<string> threadUser;						//ÿ���M�̌������Ñ���

	static vector<RaServer> servers;						//SOCKET��


	static void restartThread(int ID);

	static int threadFunc(int ID);

	static void runThread();

};



/*
                      :;J7, :,                        ::;7:
                      ,ivYi, ,                       ;LLLFS:
                      :iv7Yi                       :7ri;j5PL
                     ,:ivYLvr                    ,ivrrirrY2X,
                     :;r@Wwz.7r:                :ivu@kexianli.
                    :iL7::,:::iiirii:ii;::::,,irvF7rvvLujL7ur
                   ri::,:,::i:iiiiiii:i:irrv177JX7rYXqZEkvv17
                ;i:, , ::::iirrririi:i:::iiir2XXvii;L8OGJr71i
              :,, ,,:   ,::ir@mingyi.irii:i:::j1jri7ZBOS7ivv,
             ,::,    ::rv77iiiriii:iii:i::,rvLq@huhao.Li
             ,,      ,, ,:ir7ir::,:::i;ir:::i:i::rSGGYri712:
           :::  ,v7r:: ::rrv77:, ,, ,:i7rrii:::::, ir7ri7Lri
          ,     2OBBOi,iiir;r::        ,irriiii::,, ,iv7Luur:
        ,,     i78MBBi,:,:::,:,  :7FSL: ,iriii:::i::,,:rLqXv::
        :      iuMMP: :,:::,:ii;2GY7OBB0viiii:i:iii:i:::iJqL;::
       ,     ::::i   ,,,,, ::LuBBu BBBBBErii:i:i:i:i:i:i:r77ii
      ,       :       , ,,:::rruBZ1MBBqi, :,,,:::,::::::iiriri:
     ,               ,,,,::::i:  @arqiao.       ,:,, ,:::ii;i7:
    :,       rjujLYLi   ,,:::::,:::::::::,,   ,:i,:,,,,,::i:iii
    ::      BBBBBBBBB0,    ,,::: , ,:::::: ,      ,,,, ,,:::::::
    i,  ,  ,8BMMBBBBBBi     ,,:,,     ,,, , ,   , , , :,::ii::i::
    :      iZMOMOMBBM2::::::::::,,,,     ,,,,,,:,,,::::i:irr:i:::,
    i   ,,:;u0MBMOG1L:::i::::::  ,,,::,   ,,, ::::::i:i:iirii:i:i:
    :    ,iuUuuXUkFu7i:iii:i:::, :,:,: ::::::::i:i:::::iirr7iiri::
    :     :rk@Yizero.i:::::, ,:ii:::::::i:::::i::,::::iirrriiiri::,
     :      5BMBBBBBBSr:,::rv2kuii:::iii::,:i:,, , ,,:,:i@petermu.,
          , :r50EZ8MBBBBGOBBBZP7::::i::,:::::,: :,:,::i;rrririiii::
              :jujYY7LS0ujJL7r::,::i::,::::::::::::::iirirrrrrrr:ii:
           ,:  :@kevensun.:,:,,,::::i:i:::::,,::::::iir;ii;7v77;ii;i,
           ,,,     ,,:,::::::i:iiiii:i::::,, ::::iiiir@xingjief.r;7:i,
        , , ,,,:,,::::::::iiiiiiiiii:,:,:::::::::iiir;ri7vL77rrirri::
         :,, , ::::::::i:::i:::i:i::,,,,,:,::i:i:::iir;@Secbone.ii:::
*/