#pragma once
#include "hyfEditor.h"
#include "HYFAttackEditor.h"
#include "RaEditor.h"

/*
* RaAttackEditor.h
* Author:Raffica
*
*
* 将mode设为normal请务必使用((RaEditor*)test)->setModeNormal((RaEditor*)test, &mode);
*							或者用宏定义SETNORMAL;
*
* 要新建一个防御系统种类，首先创建一个static RaButton*然后再在setModeNormal里将其删除
* 在denfenderUp里写创造RaButton*的代码
* 按照普通的按钮那样写mouseUp和step2，然后再在RaEasy.cpp里面写对应的防御函数
* 在savenowworld里写save，在loadworld里写load
* 
*
*
*
* step2是逐帧调用的
*
* mode:
* normal 如其名
* copy 此时按一下刚体 如果有的话那么将mode设置为copy2 如果是copy2的话在step2()中 用来移动tmp刚体
* box 创建刚体 此时按一下屏幕 并将mode设置为box2 如果是box2的话在step2中 用来加特技，在mouseup中用来建立一个box（以mouse1和mouse2为aabb）
* circle 同理 只是mouseup中创建一个球，以mouse1为圆心以|mouse1-mouse2|为半径
* copyaabb 准备按第一下 copyaabb2 按第一下中 copyaabb3 按完了第一下你已经选完了你想要copy的地方 copyaabb4 正在拖你已经选好的东西
* deleteaabb 准备按第一下 deleteaabb2 正在按第一下中 在deleteaabb2状态下mouseup可以删除一个aabb里的东西
* joint 准备按第一下 创建一个钢筋
* defend 调出defender的菜单:
* archer saber rocketguard
* 
*
*/

extern Test* curTest;
class RaEditor;

b2Body* getBodyAtMouse(const b2Vec2& p, Test* test);			//Test.h

class RaAttackEditor : public RaEditor {

public:

	static void engineUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "engine") {
			SETNORMAL;
			*mode = "engine";
			cout << *mode << endl;
			
			mouse2 = mouse1 = -p;

		}
		else {
			SETNORMAL;
		}
	}

	static void rocketattackUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "rocketattack") {
			SETNORMAL;
			*mode = "rocketattack";

			mouse2 = mouse1 = -p;

		}
		else {
			SETNORMAL;
		}
	}

	static void railgunUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "railgun") {
			SETNORMAL;
			*mode = "railgun";

			mouse2 = mouse1 = -p;

		}
		else {
			SETNORMAL;
		}
	}

	static void balancerUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "balancer") {
			SETNORMAL;
			*mode = "balancer";

			mouse2 = mouse1 = -p;

		}
		else {
			SETNORMAL;
		}
	}

	static void attackerUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "attacker") {
			SETNORMAL;
			HYFAttackEditor::attackerUp(p, mode, button);
			*mode = "attacker";
			mouse2 = mouse1 = p;

			vector<string> b1, b2, b3, b4;
			b1.push_back("pic/rjengine.bmp");
			b1.push_back("pic/rjengine2.bmp");
			b2.push_back("pic/rocketattack.bmp");
			b2.push_back("pic/rocketattack2.bmp");
			b3.push_back("pic/rjbalancer.bmp");
			b3.push_back("pic/rjbalancer2.bmp");
			b4.push_back("pic/misaka.png");
			b4.push_back("pic/misaka2.png");
			
			engineBody =				Raffica::createButton(-10, 20, 2, 1, b1, "engine");
			rocketattackBody =			Raffica::createButton(-10, 17, 2, 1, b2, "rocketattack");
			balancerBody =				Raffica::createButton(-10, 11, 2, 1, b3, "balancer");
			railgunBody =				Raffica::createButton(-10, 8,  2, 1, b4, "railgun");

			engineBody->setPressUp(engineUp);
			rocketattackBody->setPressUp(rocketattackUp);
			balancerBody->setPressUp(balancerUp);
			railgunBody->setPressUp(railgunUp);
		}
		else {
			SETNORMAL;
		}
	}

	void createButtons2() {										//不继承的添加按钮
		vector<string> button1;
		
		button1.push_back("pic/rjattack.bmp");
		button1.push_back("pic/rjattack2.bmp");
		
		attackerBody = Raffica::createButton(-4, -19.5f, 2.5f, 1.4f, button1, "attacker");

		attackerBody->setPressUp(attackerUp);

		((BreakableUserData*)attackerBody->body->GetUserData())->tip = "创建攻击性Object";

		HYFAttackEditor::createButtons2();
	}

	RaAttackEditor() {
		type = "attack";
	}

	void init() {
		string __debugTMP = mainUseAttack;

		this->worldFileName = "attackinfo.txt";
		((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);

		client.sendMsgByHello(("get " + attackerName + "\\attackinfo.txt").c_str());
		client.getFile("attackinfo.txt", "");

		Raffica::loadWorld(this, worldFileName);
		Raffica::makeWall(390, 80, 0);
		leftBorder = -110;
		rightBorder = 190;
		createButtons();
		createButtons2();

		Raffica::cameraTo = 50;

		m_world->SetGravity(b2Vec2(0.0f, Raffica::normalG));

		curTest->m_particleSystem->SetRadius(Raffica::LIQUID_RADIUS);
		curTest->m_particleSystem->SetDamping(Raffica::LIQUID_DAMPING);
		curTest->m_particleSystem->SetDensity(Raffica::LIQUID_DENSITY);

		mainUseAttack = __debugTMP;
	}

	static Test* Create() {
		return new RaAttackEditor;
	}
	
};
