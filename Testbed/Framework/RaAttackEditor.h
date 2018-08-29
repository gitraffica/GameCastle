#pragma once
#include "hyfEditor.h"
#include "HYFAttackEditor.h"
#include "RaEditor.h"

/*
* RaAttackEditor.h
* Author:Raffica
*
*
* ��mode��Ϊnormal�����ʹ��((RaEditor*)test)->setModeNormal((RaEditor*)test, &mode);
*							�����ú궨��SETNORMAL;
*
* Ҫ�½�һ������ϵͳ���࣬���ȴ���һ��static RaButton*Ȼ������setModeNormal�ｫ��ɾ��
* ��denfenderUp��д����RaButton*�Ĵ���
* ������ͨ�İ�ť����дmouseUp��step2��Ȼ������RaEasy.cpp����д��Ӧ�ķ�������
* ��savenowworld��дsave����loadworld��дload
* 
*
*
*
* step2����֡���õ�
*
* mode:
* normal ������
* copy ��ʱ��һ�¸��� ����еĻ���ô��mode����Ϊcopy2 �����copy2�Ļ���step2()�� �����ƶ�tmp����
* box �������� ��ʱ��һ����Ļ ����mode����Ϊbox2 �����box2�Ļ���step2�� �������ؼ�����mouseup����������һ��box����mouse1��mouse2Ϊaabb��
* circle ͬ�� ֻ��mouseup�д���һ������mouse1ΪԲ����|mouse1-mouse2|Ϊ�뾶
* copyaabb ׼������һ�� copyaabb2 ����һ���� copyaabb3 �����˵�һ�����Ѿ�ѡ��������Ҫcopy�ĵط� copyaabb4 ���������Ѿ�ѡ�õĶ���
* deleteaabb ׼������һ�� deleteaabb2 ���ڰ���һ���� ��deleteaabb2״̬��mouseup����ɾ��һ��aabb��Ķ���
* joint ׼������һ�� ����һ���ֽ�
* defend ����defender�Ĳ˵�:
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

	void createButtons2() {										//���̳е���Ӱ�ť
		vector<string> button1;
		
		button1.push_back("pic/rjattack.bmp");
		button1.push_back("pic/rjattack2.bmp");
		
		attackerBody = Raffica::createButton(-4, -19.5f, 2.5f, 1.4f, button1, "attacker");

		attackerBody->setPressUp(attackerUp);

		((BreakableUserData*)attackerBody->body->GetUserData())->tip = "����������Object";

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
