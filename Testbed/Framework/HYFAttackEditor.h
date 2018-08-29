#pragma once

#define SETNORMAL ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, mode)

#include "hyfEditor.h"
//#include "RaEditor.h"
//#include "RaAttackEditor.h"

/*
* Author: 惠一锋
*
* HYFAttackEditor
* 用来编辑自己的城堡的头文件
*
*/

class HYFAttackEditor : public HYFEditor {

public:

	static void rengineUp(const b2Vec2& p, string* mode, const RaButton* button);

	static void createButtons2();

	static void attackerUp(const b2Vec2& p, string* mode, const RaButton* button);

};