#pragma once

#include "RaEasy.h"
#include "Box2D\Box2D.h"

#define SETNORMAL ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, mode)

/*
* Author: ��һ��
*
* HYFEditor
* �����༭�Լ��ĳǱ���ͷ�ļ�
* 
*/

//�����RaEditor�еĺ���˵������Ҫʱ���Ը��ƴ��룬����ע�ⲻҪ����bug

class HYFEditor {

public:

	static void setModeNormal(string* md);

	static void createButtons();

	static void MouseUp(const b2Vec2& p);

	static void MouseDown(const b2Vec2& p);
		

	static void Step2(Settings* settings);												//��ִ֡�У�ͼ�������ͼ��ǰ��

};