#pragma once

#include "RaEasy.h"
#include "Box2D\Box2D.h"

#define SETNORMAL ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, mode)

/*
* Author: 惠一锋
*
* HYFEditor
* 用来编辑自己的城堡的头文件
* 
*/

//请参照RaEditor中的函数说明，必要时可以复制代码，但是注意不要引入bug

class HYFEditor {

public:

	static void setModeNormal(string* md);

	static void createButtons();

	static void MouseUp(const b2Vec2& p);

	static void MouseDown(const b2Vec2& p);
		

	static void Step2(Settings* settings);												//单帧执行，图层比正常图层前面

};