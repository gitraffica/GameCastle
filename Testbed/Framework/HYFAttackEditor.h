#pragma once

#define SETNORMAL ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, mode)

#include "hyfEditor.h"
//#include "RaEditor.h"
//#include "RaAttackEditor.h"

/*
* Author: ��һ��
*
* HYFAttackEditor
* �����༭�Լ��ĳǱ���ͷ�ļ�
*
*/

class HYFAttackEditor : public HYFEditor {

public:

	static void rengineUp(const b2Vec2& p, string* mode, const RaButton* button);

	static void createButtons2();

	static void attackerUp(const b2Vec2& p, string* mode, const RaButton* button);

};