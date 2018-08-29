#pragma once

#define SETNORMAL ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, mode)
#define SETNORMAL2 ((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode)

/*
* RaEditor.h
* Author:Raffica
*
*
* 将mode设为normal请务必使用((RaEditor*)test)->setModeNormal((RaEditor*)test, mode);
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

/*
* QueryCallBack类
* 功能 提供callback函数来查询aabb
*/

#include <iostream>
#include "RaRules.h"
#include "hyfEditor.h"
#include "Test.h"
#include "glui\glui.h"
#include "RaCPUEditor.h"
using namespace std;


class QueryCallback : public b2QueryCallback {

public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		bool inside = fixture->TestPoint(m_point);
		if (inside && body->GetUserData() != NULL) {
			BasicUserData* bud = (BasicUserData*)body->GetUserData();
			if (bud->type == "explosiveeffect") return true;

			if (bud->isButton == true || m_fixture == NULL) {
				if (bud->isButton == true && bud->name != "panel" && (m_fixture == NULL || ((BasicUserData*)m_fixture->GetBody()->GetUserData())->isButton == false)) m_fixture = fixture;
				if (bud->isButton == false) m_fixture = fixture;
			}			//如果点到了按钮 判定点了最前面那个按钮 如果有按钮和正常物品那么判定点了按钮
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

extern Test* curTest;

void Restart(int);
void EditAttack(int);
void EditLevel(int);
void startPVP(int);

b2Body* getBodyAtMouse(const b2Vec2& p, Test* test);			//Test.h

class RaEditor : public Test {

public:

	static b2Vec2 mouse1, mouse2;						//这决定了拖出来的线段/矩形的两端点
	b2Vec2 border1, border2;							//边界
	string mode;										//normal,box,box2,delete,ball,ball2
	string material;
	char* worldFileName;								//file name
	bool continuous;


	//为了添加按钮，你需要定义一个RaButton*变量，并用Raffica::createButton()创建
	//并在下面的RaEditor()里添加相应的事件和材质
	//按钮的功能应该在MouseUp()里定义
	//有时候按钮的功能需要配合MouseDown() MouseMove() Step2()

	//按钮们
	RaButton* createBoxBody;
	RaButton* createLiquidBody;
	RaButton* createBallBody;

	RaButton* copyBody;
	static b2Body* copyTarget;
	static BreakableUserData* copyUserData;
	static b2Body* copiedBody;

	RaButton* copyAABBBody;
	static vector<b2Body*> copyTargets;
	static vector<BreakableUserData*> copyUserDatas;
	static vector<b2Body*> copiedBodys;

	RaButton* panelButton;
	RaButton* panelButton2;
	RaButton* jointBody;
	RaButton* rjointBody;
	RaButton* saveBody;
	RaButton* deleteBody;
	RaButton* deleteAABBBody;
	RaButton* savenowBody;
	RaButton* gravityBody;
	RaButton* continuousBody;
	RaButton* loginButton;
	vector<RaButton*> materialBody;


	RaButton* defenderBody;

	static RaButton* archerBody;
	static RaButton* saberBody;
	static RaButton* rocketguardBody;
	static RaButton* monitorBody;							//当检测到敌方时发出R信号
	static RaButton* launcherBody;							//当接收到R信号时发射东西，力度根据R信号大小。
	static RaButton* lampBody;								//当接收到R信号时亮
	static RaButton* capBody;								//R信号的电容器
	static RaButton* mainBody;

	RaButton* attackerBody;
	static RaButton* engineBody;
	static RaButton* rocketattackBody;
	static RaButton* rengineBody;
	static RaButton* balancerBody;
	static RaButton* railgunBody;							//你股间跳动的香蕉，是我不变的信仰
	static RaButton* repeaterBody;

	RaButton* restartButton;
	RaButton* editAttackButton;
	RaButton* editDefendButton;
	RaButton* PVPButton;

	/*TEST*/
	RaButton* testBody;
	/*/TEST*/


	void setModeNormal(RaEditor* test, string* md) {
		string mo = *md;
		*md = "normal";
		for (unsigned int i = 0; i < test->copyUserDatas.size(); i++) delete test->copyUserDatas[i];
		test->copyTargets.clear(); test->copyTargets.shrink_to_fit();
		test->copiedBodys.clear(); test->copiedBodys.shrink_to_fit();
		test->copyUserDatas.clear(); test->copyUserDatas.shrink_to_fit();

		if (test->archerBody) {
			delete test->archerBody;
			test->archerBody = 0;
		}
		if (test->saberBody) {
			delete test->saberBody;
			test->saberBody = 0;
		}
		if (test->rocketguardBody) {
			delete test->rocketguardBody;
			test->rocketguardBody = 0;
		}
		if (test->monitorBody) {
			delete test->monitorBody;
			test->monitorBody = 0;
		}
		if(test->launcherBody) {
			delete test->launcherBody;
			test->launcherBody = 0;
		}
		if (test->lampBody) {
			delete test->lampBody;
			test->lampBody = 0;
		}
		if (test->capBody) {
			delete test->capBody;
			test->capBody = 0;
		}
		if (test->repeaterBody) {
			delete test->repeaterBody;
			test->repeaterBody = 0;
		}

		if (test->type == "attack") {
			if (test->engineBody) {
				delete test->engineBody;
				test->engineBody = 0;
			}
			if (test->rocketattackBody) {
				delete test->rocketattackBody;
				test->rocketattackBody = 0;
			}
			if (test->balancerBody) {
				delete test->balancerBody;
				test->balancerBody = 0;
			}
			if (test->railgunBody) {
				delete test->railgunBody;
				test->railgunBody = 0;
			}
		}

		HYFEditor::setModeNormal(md);

		if (continuous) {
			*md = mo;
			if (mo[mo.size() - 1] == '2') md->resize(md->size() - 1);
			mouse1 = mouse;
		}
	}

	static void restartUp(const b2Vec2& p, string* mode, const RaButton* button) {
		Restart(1);
	}

	static void editAttackUp(const b2Vec2& p, string* mode, const RaButton* button) {
		EditAttack(1);
	}

	static void editDefendUp(const b2Vec2& p, string* mode, const RaButton* button) {
		EditLevel(1);
	}

	static void PVPUp(const b2Vec2& p, string* mode, const RaButton* button) {
		startPVP(1);
	}

	/*
	* 设置材质的按钮的侦听函数
	*/

	static void materialUp(const b2Vec2& p, string* mode, const RaButton* button) {
		*mode = button->material->name;
	}

	//创建一个球

	static void createBallUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "ball") {
			SETNORMAL;
			mouse2 = mouse1 = p;
			*mode = "ball";
		} else SETNORMAL;
	}

	//创建长方体

	static void createBoxUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "box") {
			SETNORMAL;
			mouse2 = mouse1 = p;
			*mode = "box";
		} else SETNORMAL;
	}

	//创建流体

	static void liquidUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "liquid") {
			SETNORMAL;
			mouse2 = mouse1 = p;
			*mode = "liquid";
		} else SETNORMAL;
	}

	//创建铁棍关节

	static void jointUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "joint" && *mode != "joint2") {
			SETNORMAL;
			mouse2 = mouse1 = p;
			*mode = "joint";
		} else SETNORMAL;
	}

	//创建R导线关节

	static void rjointUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "rjoint" && *mode != "rjoint2") {
			SETNORMAL;
			mouse2 = mouse1 = p;
			*mode = "rjoint";
		}
		else SETNORMAL;
	}

	//保存按钮

	static void saveUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		//Raffica::saveWorld();
	}

	//保存当前局势按钮

	static void savenowUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		if(curTest->type == "defend") Raffica::savenowWorld(((RaEditor*)curTest)->worldFileName, castleName);
		else if(curTest->type == "attack") Raffica::savenowWorld(((RaEditor*)curTest)->worldFileName, attackerName);
	}

	//删除一个刚体按钮

	static void deleteUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "delete") {
			SETNORMAL;
			*mode = "delete";
		} else SETNORMAL;
	}

	//设定重力有无的按钮

	static void gravityUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(fabs(curTest->m_world->GetGravity().y) < 0.0005f) curTest->m_world->SetGravity(b2Vec2(0, (float)Raffica::normalG));
		else curTest->m_world->SetGravity(b2Vec2(0.0f, 0.0f));
		b2Body* body = curTest->m_world->GetBodyList();
		for(;body;body = body->GetNext()) body->SetAwake(true);
	}

	//按下这个按钮，你能够切换是否连续加控件

	static void continuousUp(const b2Vec2& p, string* mode, const RaButton* button) {
		bool& continuous = ((RaEditor*)curTest)->continuous;
		continuous = !continuous;
	}

	//复制一个刚体的按钮

	static void copyUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "copy") {
			SETNORMAL;
			*mode = "copy";
			mouse2 = mouse1 = p;
			
		} else SETNORMAL;
	}

	//复制一堆刚体的按钮

	static void copyaabbUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "copyaabb") {
			SETNORMAL;
			*mode = "copyaabb";
			mouse2 = mouse1 = p;
		}
	}

	//删除一堆刚体的按钮

	static void deleteaabbUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "deleteaabb") {
			SETNORMAL;
			*mode = "deleteaabb";
			mouse2 = mouse1 = p;
		}
	}


	//点了这个你就能聊天了

	static void loginUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "login") *mode = "login";
		else SETNORMAL;
	}

	
	//点开defender界面的按钮

	static void defenderUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if(*mode != "defender") {
			SETNORMAL;
			*mode = "defender";
			mouse2 = mouse1 = p;
			
			vector<string> b1, b2, b3, b4, b5, b6, b7, b8;
			b1.push_back("pic/archer.png");
			b1.push_back("pic/archer2.bmp");
			b2.push_back("pic/saber.bmp");
			b2.push_back("pic/saber2.bmp");
			b3.push_back("pic/rocketguard.bmp");
			b3.push_back("pic/rocketguard2.bmp");
			b4.push_back("pic/monitor.bmp");
			b4.push_back("pic/monitor2.bmp");
			b5.push_back("pic/launcher.bmp");
			b5.push_back("pic/launcher2.bmp");
			b6.push_back("pic/lamp.png");
			b6.push_back("pic/lamp2.png");
			b7.push_back("pic/racap.bmp");
			b7.push_back("pic/racap2.bmp");
			b8.push_back("pic/repeater.bmp");
			b8.push_back("pic/repeater2.bmp");

			archerBody = Raffica::createButton			(-10, 20, 2, 1, b1, "archer");
			saberBody  = Raffica::createButton			(-10, 17, 2, 1, b2, "saber");
			rocketguardBody = Raffica::createButton		(-10, 14, 2, 1, b3, "rocketguard");
			monitorBody = Raffica::createButton			(-6,  20, 2, 1, b4, "monitor");
			launcherBody = Raffica::createButton		(-6,  17, 2, 1, b5, "launcher");
			lampBody = Raffica::createButton			(-6,  14, 2, 1, b6, "lamp");
			capBody = Raffica::createButton				(-6,  11, 2, 1, b7, "cap");
			repeaterBody = Raffica::createButton		(-6,  8 , 2, 1, b8, "repeater");

			archerBody->setPressUp(archerUp);
			saberBody->setPressUp(saberUp);
			rocketguardBody->setPressUp(rocketguardUp);
			monitorBody->setPressUp(monitorUp);
			launcherBody->setPressUp(launcherUp);
			lampBody->setPressUp(lampUp);
			capBody->setPressUp(capUp);
			repeaterBody->setPressUp(repeaterUp);
		} else {
			SETNORMAL;
		}
	}

	static void archerUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "archer";
		mouse2 = mouse1 = -p;
	}

	static void saberUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "saber";
		mouse2 = mouse1 = -p;
	}

	static void rocketguardUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "rocketguard";
		mouse2 = mouse1 = -p;
	}

	static void monitorUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "monitor";
		mouse2 = mouse1 = -p;
	}

	static void launcherUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "launcher";
		mouse2 = mouse1 = -p;
	}

	static void lampUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "lamp";
		mouse2 = mouse1 = -p;
	}

	static void capUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "cap";
		mouse2 = mouse1 = -p;
	}

	static void repeaterUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "repeater";
		mouse2 = mouse1 = -p;
	}

	static void mainUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		*mode = "main";
		mouse2 = mouse1 = -p;
	}

	//TEST
	static void testUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		RaCPUEditor::BuildCityGate(curTest);
		EditLevel(1);
	}

	//按下鼠标（包括按在按钮上）

	void MouseDown(const b2Vec2& p) {
		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if(body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if(!ud || !ud->isButton) {											//如果点到了空白或者点到了刚体
			if(mode == "copyaabb") {
				mouse2 = mouse1 = p;
				mode = "copyaabb2";
				return;
			}
			if(mode == "deleteaabb") {
				mouse2 = mouse1 = p;
				mode = "deleteaabb2";
				return;
			}
			if(mode == "copyaabb3") {
				mouse2 = mouse1 = p;
				mode = "copyaabb4";
				return;
			}
			//if(!ud) {														//如果点到了空白处
				if(mode == "box") {
					mouse2 = mouse1 = p;
					mode = "box2";
					return;
				}

				if(mode == "ball") {
					mouse2 = mouse1 = p;
					mode = "ball2";
					return;
				}

				if (mode == "liquid") {
					mouse2 = mouse1 = p;
					mode = "liquid2";
					return;
				}
				
			//}
			else if(mode == "copy") {
				mouse2 = mouse1 = p;
				copyTarget = getBodyAtMouse(p, this);
				if (copyTarget == NULL) {
					SETNORMAL2;
					return;
				}
				mode = "copy2";

				copyUserData = new BreakableUserData(*(BreakableUserData*)copyTarget->GetUserData());
				//BreakableUserData& ud = *(BreakableUserData*)copyTarget->GetUserData();
				
				if(((BasicUserData*)copyTarget->GetUserData())->type != "defend")
					Raffica::virtualCopy(copyTarget, copiedBody, copyUserData);
				else copiedBody = 0;
				

			}
			else if(mode == "joint") {
				mouse2 = mouse1 = p;
				
				copyTarget = getBodyAtMouse(p, this);

				if (!copyTarget) return;
				if (!copyTarget->GetUserData()) return;

				if(((BasicUserData*)copyTarget->GetUserData())->isButton) {
					return;
				}

				copyTarget->SetLinearVelocity(b2Vec2(0, 0));
				mode = "joint2";
			}
			else if (mode == "rjoint") {
				mouse2 = mouse1 = p;

				copyTarget = getBodyAtMouse(p, this);

				if (!copyTarget) return;
				if (!copyTarget->GetUserData()) return;

				if (((BasicUserData*)copyTarget->GetUserData())->isButton) {
					return;
				}

				copyTarget->SetLinearVelocity(b2Vec2(0, 0));
				mode = "rjoint2";
			}
			else if(mode == "copy2");
			else if(mode == "normal") {
				Test::MouseDown(p);
			}
		}
		else {																			//点到了按钮
			if(!ud->button->material) ud->button->pressDown(p, &mode, ud->button);
			else ud->button->pressDown(p, &material, ud->button);
		}
		
		HYFEditor::MouseDown(p);

	}

	void MouseUp(const b2Vec2& p) {

		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if(body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;

		float mouseLength = 50;
		if (mode == "ball2") mouseLength = 25;
		if (mode == "copyaabb2" || mode == "deleteaabb2") mouseLength = 1000;

		b2Vec2 delta(mouse2.x - mouse1.x, mouse2.y - mouse1.y);
		if (delta.Length() > mouseLength) {
			mouse2.x = mouse1.x + (mouse2.x - mouse1.x) * mouseLength / delta.Length();
			mouse2.y = mouse1.y + (mouse2.y - mouse1.y) * mouseLength / delta.Length();
		}

		if(!(ud && ud->isButton)) {											//如果没有正在按按钮

			if(mode == "box2" && mouse1.x != mouse2.x && mouse1.y != mouse2.y) {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				body = Raffica::createBoxByMaterial(mouse1, mouse2, material);

				totalCost += Raffica::calculateCost(body->GetMass(), ((BreakableUserData*)body->GetUserData())->defend, ((BreakableUserData*)body->GetUserData())->hp, material);

				return;
			}

			if(mode == "ball2" && mouse1.x != mouse2.x && mouse1.y != mouse2.y) {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				body = Raffica::createBallByMaterial(mouse1, mouse2, material);

				totalCost += Raffica::calculateCost(body->GetMass(), ((BreakableUserData*)body->GetUserData())->defend, ((BreakableUserData*)body->GetUserData())->hp, material);

				return;
			}

			if (mode == "liquid2" && mouse1.x != mouse2.x && mouse1.y != mouse2.y) {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				for (float i = min(mouse1.x, mouse2.x); i < max(mouse1.x, mouse2.x); i += .9f) {
					for (float j = min(mouse1.y, mouse2.y); j < max(mouse1.y, mouse2.y); j += .9f) {
						//body = Raffica::createBoxByMaterial(b2Vec2(i, j), b2Vec2(i+2, j+2), material);
						Raffica::createParticle(b2Vec2(i, j), b2Vec2(i+.9f, j+.9f), 0, 0, 100, 100, 100);
					}
				}
				//Raffica::createParticle(mouse1, mouse2, 50, 50, 100);
				//totalCost += Raffica::calculateCost(body->GetMass(), ((BreakableUserData*)body->GetUserData())->defend, ((BreakableUserData*)body->GetUserData())->hp, material);

				return;
			}

			if(mode == "joint2") {
				copiedBody = getBodyAtMouse(mouse2, this);

				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);

				if(!copiedBody) return;
				if(copiedBody == copyTarget) return;
				if (((BreakableUserData*)copiedBody->GetUserData())->isButton) return;

				b2Joint* j = Raffica::createDisJoint(copiedBody, copyTarget, mouse2, mouse1, true, true);

				DistanceJointUserData* dud = new DistanceJointUserData();
				dud->width = 0.5f;
				dud->textureIndex = LoadTexture("pic/material/steelstick.png");
				Raffica::setUserData(j, dud);
				
				copyTarget = 0;
				copiedBody = 0;

				return;

			}

			if (mode == "rjoint2") {
				copiedBody = getBodyAtMouse(mouse2, this);

				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);

				if (!copiedBody) return;
				if (copiedBody == copyTarget) return;
				if (((BreakableUserData*)copiedBody->GetUserData())->isButton) return;

				b2Joint* j = Raffica::createDisJoint(copiedBody, copyTarget, mouse2, mouse1, true, true);

				DistanceJointUserData* dud = new DistanceJointUserData();
				dud->width = 0.5f;
				dud->textureIndex = LoadTexture("pic/material/rline.png");
				dud->feature = "R_TRANSPORT";
				Raffica::setUserData(j, dud);

				copyTarget = 0;
				copiedBody = 0;

				return;

			}

			if(mode == "copy2") {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);

				if(!copiedBody) return;
				b2Filter f;
				f.categoryBits = 1U, f.maskBits = 65535U;
				copiedBody->GetFixtureList()->SetFilterData(f);

				BreakableUserData* bud = (BreakableUserData*)copiedBody->GetUserData();
				totalCost += Raffica::calculateCost(copiedBody->GetMass(), ((BreakableUserData*)copiedBody->GetUserData())->defend, ((BreakableUserData*)copiedBody->GetUserData())->hp, bud->feature);

				return;
			}

			if(mode == "copyaabb4") {
				b2Filter f;
				f.categoryBits = 1U, f.maskBits = 65535U;
				for(unsigned int i = 0;i < copyTargets.size();i ++) {
					copiedBody = copiedBodys[i];
					if (!copiedBody) continue;
					BreakableUserData* bud = (BreakableUserData*)copiedBody->GetUserData();
					totalCost += Raffica::calculateCost(copiedBody->GetMass(), ((BreakableUserData*)copiedBody->GetUserData())->defend, ((BreakableUserData*)copiedBody->GetUserData())->hp, bud->feature);
					copiedBodys[i]->GetFixtureList()->SetFilterData(f);
				}
				copyTargets.clear(); copyTargets.shrink_to_fit();
				copiedBodys.clear(); copiedBodys.shrink_to_fit();
				copyUserDatas.clear(); copyUserDatas.shrink_to_fit();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				return;
			}

			if(mode == "delete") {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				
				if(body) {

					BasicUserData* bud = ((BasicUserData*)body->GetUserData());
					totalCost -= Raffica::calculateCost(body->GetMass(), ((BreakableUserData*)body->GetUserData())->defend, ((BreakableUserData*)body->GetUserData())->hp, bud->feature);
					if((bud->breakable)) if(body->GetFixtureList()->GetDensity() != 0) {
						delete bud;
						bud = 0;
					}
				}
				return;
			}

			if(mode == "copyaabb2" && mouse1.x != mouse2.x && mouse1.y != mouse2.y) {
				mode = "copyaabb3";
				b2AABB aabb;
				aabb.lowerBound = b2Vec2(mouse1.x, mouse1.y);
				aabb.upperBound = b2Vec2(mouse2.x, mouse2.y);
				this->copyTargets.clear();
				this->copyTargets.shrink_to_fit();
				Raffica::queryAABB(&copyTargets, mouse1, mouse2);
				copyUserDatas.resize(copyTargets.size());
				copiedBodys.resize(copyTargets.size());
				for(unsigned int i = 0;i < copyTargets.size();i ++) {
					if(((BasicUserData*)copyTargets[i]->GetUserData())->type != "defend") {
						Raffica::virtualCopy(copyTargets[i], copiedBodys[i], copyUserDatas[i]);
					} else copiedBodys[i] = 0;
				}
				vector<b2Joint*> copyJoints;
				Raffica::queryAABBJoint(&copyJoints, mouse1, mouse2);
				for (unsigned int i = 0; i < copyTargets.size(); i++) {
					if (!copiedBodys[i]) continue;
					copiedBodys[i]->SetTransform(copiedBodys[i]->GetPosition() + b2Vec2(10000, 0), copiedBodys[i]->GetAngle());
				}
				for (unsigned int i = 0; i < copyJoints.size(); i++) {
					b2Joint*& j = copyJoints[i];
					b2Body* ba = getBodyAtMouse(j->GetAnchorA() + b2Vec2(10000, 0), curTest);
					b2Body* bb = getBodyAtMouse(j->GetAnchorB() + b2Vec2(10000, 0), curTest);
					if (!bb || !ba) continue;
					b2Joint* newjoint = Raffica::createDisJoint(ba, bb,
						j->GetAnchorA() + b2Vec2(10000, 0), j->GetAnchorB() + b2Vec2(10000, 0), true, true);
					DistanceJointUserData* ud = new DistanceJointUserData();
					ud->textureIndex = ((DistanceJointUserData*)copyJoints[i]->GetUserData())->textureIndex;
					ud->width = ((DistanceJointUserData*)copyJoints[i]->GetUserData())->width;
					Raffica::setUserData(newjoint, ud);
				}
				for (unsigned int i = 0; i < copyTargets.size(); i++) {
					if (!copiedBodys[i]) continue;
					copiedBodys[i]->SetTransform(copiedBodys[i]->GetPosition() + b2Vec2(-10000, 0), copiedBodys[i]->GetAngle());
				}
				return;
			}

			if(mode == "deleteaabb2" && mouse1.x != mouse2.x && mouse1.y != mouse2.y) {
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				this->copyTargets.clear();
				this->copyTargets.shrink_to_fit();
				Raffica::queryAABB(&copyTargets, mouse1, mouse2);
				for(unsigned int i = 0;i < copyTargets.size();i ++) {
					BasicUserData* bud = ((BasicUserData*)copyTargets[i]->GetUserData());
					copiedBody = copyTargets[i];
					cout << ((BreakableUserData*)copiedBody->GetUserData())->defend << endl;
					totalCost -= Raffica::calculateCost(copiedBody->GetMass(), ((BreakableUserData*)copiedBody->GetUserData())->defend, ((BreakableUserData*)copiedBody->GetUserData())->hp, bud->feature);
					delete bud;
				}
				return;
			}

			HYFEditor::MouseUp(p);

			if(mode == "archer") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::archerDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/archer.png"));

				ud->Init(4300, 3001, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "archer";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_ARCHER;
				return;
			}

			if (mode == "saber") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::saberDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/saber.bmp"));

				ud->Init(4300, 3002, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "saber";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_SABER;
				return;
			}
			
			if (mode == "rocketguard") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::rocketguardDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/rocketguard.bmp"));

				ud->Init(4300, 3003, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "rocketguard";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_ROCKETDEFENDER;
				return;
			}

			if (mode == "lamp") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::lampDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/lamp.png"));

				ud->Init(4300, 3012, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "lamp";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_LAMP;
				return;
			}

			if (mode == "cap") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::capDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/racap.bmp"));

				ud->Init(4300, 3013, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "cap";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_CAP;
				return;
			}

			if (mode == "repeater") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::repeaterDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/repeater.bmp"));

				ud->Init(4300, 3014, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "repeater";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_REPEATER;
				return;
			}

			if (mode == "launcher") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 6, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::launcherDefender;			///
				ud->width = 12, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/launcher.bmp"));

				ud->Init(4300, 3011, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "launcher";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_LAUNCHER;
				return;
			}

			if (mode == "monitor") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::monitorDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/monitor.bmp"));

				ud->Init(400300, 30010, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "monitor";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_MONITOR;
				return;
			}

			if (mode == "engine") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::engineDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/engine.bmp"));

				ud->Init(4300, 3004, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "engine";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_ENGINE;
				return;
			}

			if (mode == "rocketattack") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::rocketattackDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/rocketattack.bmp"));

				ud->Init(4300, 3006, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "rocketattack";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_ROCKETATTACKER;
				return;
			}
			if (mode == "balancer") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::balancerDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/balancer.bmp"));

				ud->Init(4300, 3007, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "balancer";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_BALANCER;
				return;
			}

			if (mode == "main") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 2, 2);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::mainDefender;
				ud->width = 4, ud->height = 4;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/main.bmp"));

				ud->Init(4300, 3008, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "main";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_MAIN;
				return;
			}

			if (mode == "railgun") {
				mouse2 = mouse1 = p;
				Building* build = new Building();
				((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
				b2Body* body = Raffica::createBox(mouse2.x, mouse2.y, 3, 3);
				DefendUserData* ud = new DefendUserData();
				ud->defenderFunc = Raffica::railgunDefender;
				ud->width = 6, ud->height = 6;

				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/misaka.png"));

				ud->Init(4300, 3009, textures);
				Raffica::setUserData(body, ud);

				build->body = body;
				build->name = "railgun";
				build->userdata = ud;

				Raffica::buildingList.push_back(build);

				totalCost += Raffica::COST_RAILGUN;
				return;
			}

			if (mode == "normal") Test::MouseUp(p);

		}
		if(ud) if(ud->isButton) if(ud->button->pressed) {									//对按钮分类的原因是一个要改mode，一个要改material
			if(!ud->button->isMaterialButton) ud->button->pressUp(p, &mode, ud->button);
			else ud->button->pressUp(p, &material, ud->button);
		}

	}

	void Step2(Settings* settings) {													//单帧执行，图层比正常图层前面
		float x1, x2, y1, y2;

		float mouseLength = 50;
		if (mode == "ball2") mouseLength = 25;
		if (mode == "copyaabb2" || mode == "deleteaabb2") mouseLength = 1000;

		b2Vec2 delta(mouse2.x - mouse1.x, mouse2.y - mouse1.y);
		if (delta.Length() > mouseLength) {
			mouse2.x = mouse1.x + (mouse2.x - mouse1.x) * mouseLength / delta.Length();
			mouse2.y = mouse1.y + (mouse2.y - mouse1.y) * mouseLength / delta.Length();
		}

		x1 = min(mouse1.x, mouse2.x);
		x2 = max(mouse1.x, mouse2.x);
		y1 = min(mouse1.y, mouse2.y);
		y2 = max(mouse1.y, mouse2.y);
		
		if(mode == "box2") {
			glLoadIdentity();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (*Raffica::materialList[material]->textures)[(Raffica::materialList[material]->textures)->size()-1]);
			glBegin(GL_POLYGON);
			glTexCoord2f(x1, y1); glVertex2f(x1, y1);
			glTexCoord2f(x1, y2); glVertex2f(x1, y2);
			glTexCoord2f(x2, y2); glVertex2f(x2, y2);
			glTexCoord2f(x2, y1); glVertex2f(x2, y1);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		if(mode == "ball2") {
			float R = sqrt((mouse1.x-mouse2.x)*(mouse1.x-mouse2.x)+(mouse1.y-mouse2.y)*(mouse1.y-mouse2.y));
			glLoadIdentity();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (*Raffica::materialList[material]->textures)[(Raffica::materialList[material]->textures)->size() - 1]);
			glBegin(GL_POLYGON);
			for(int i = 0;i < 50;i ++) {
				glVertex2f(R*cos(2*3.14f/50*i) + mouse1.x, R*sin(2*3.14f/50*i) + mouse1.y);
			}
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		if (mode == "liquid2") {
			glLoadIdentity();
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (*Raffica::materialList[material]->textures)[(Raffica::materialList[material]->textures)->size() - 1]);
			glBegin(GL_POLYGON);
			glTexCoord2f(x1, y1); glVertex2f(x1, y1);
			glTexCoord2f(x1, y2); glVertex2f(x1, y2);
			glTexCoord2f(x2, y2); glVertex2f(x2, y2);
			glTexCoord2f(x2, y1); glVertex2f(x2, y1);
			glEnd();
			glDisable(GL_TEXTURE_2D);
		}
		if(mode == "joint2") {
			copyTarget->ApplyForce(b2Vec2(-m_world->GetGravity().x*copyTarget->GetMass(), -m_world->GetGravity().y*copyTarget->GetMass()), copyTarget->GetPosition(), true);
			copyTarget->SetLinearVelocity(b2Vec2(0, 0));

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glLoadIdentity(); 
			float len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))/2;
			float angle = atan2((mouse1.y-mouse2.y), (mouse1.x-mouse2.x));
			
			glTranslatef((x1+x2) / 2, (y1+y2) / 2, 0);
			glRotatef(angle*180.0f/3.1416f+90.0f, 0, 0, 1.0f);
			glColor4f(0.5f, 0.5f, 0.5f, 0.8f);
			glBegin(GL_POLYGON);
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-.5f, -len);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-.5f, len);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(.5f, len);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(.5f, -len);
			glEnd();
			glDisable(GL_BLEND);
		}
		if (mode == "rjoint2") {
			copyTarget->ApplyForce(b2Vec2(-m_world->GetGravity().x*copyTarget->GetMass(), -m_world->GetGravity().y*copyTarget->GetMass()), copyTarget->GetPosition(), true);
			copyTarget->SetLinearVelocity(b2Vec2(0, 0));

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glLoadIdentity();
			float len = sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2)) / 2;
			float angle = atan2((mouse1.y - mouse2.y), (mouse1.x - mouse2.x));

			glTranslatef((x1 + x2) / 2, (y1 + y2) / 2, 0);
			glRotatef(angle*180.0f / 3.1416f + 90.0f, 0, 0, 1.0f);
			glColor4f(0.8f, 0.4f, 0.4f, 0.8f);
			glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-.5f, -len);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-.5f, len);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(.5f, len);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(.5f, -len);
			glEnd();
			glDisable(GL_BLEND);
		}
		if(mode == "copy2") {
			if(!copiedBody) return;
			copiedBody->SetTransform(mouse2, copiedBody->GetAngle());
			copiedBody->ApplyForce(b2Vec2(-m_world->GetGravity().x*copiedBody->GetMass(), -m_world->GetGravity().y*copiedBody->GetMass()), copiedBody->GetPosition(), true);

			//Raffica::setUserData(copiedBody, new UserData(*((UserData*)(copyTarget->GetUserData()))));		//这什么**
		}
		if(mode == "copyaabb2") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glBegin(GL_POLYGON);
			glTexCoord2f(x1, y1); glVertex2f(x1, y1);
			glTexCoord2f(x1, y2); glVertex2f(x1, y2);
			glTexCoord2f(x2, y2); glVertex2f(x2, y2);
			glTexCoord2f(x2, y1); glVertex2f(x2, y1);
			glEnd();
			glDisable(GL_BLEND);
		}
		if(mode == "deleteaabb2") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glBegin(GL_POLYGON);
			glTexCoord2f(x1, y1); glVertex2f(x1, y1);
			glTexCoord2f(x1, y2); glVertex2f(x1, y2);
			glTexCoord2f(x2, y2); glVertex2f(x2, y2);
			glTexCoord2f(x2, y1); glVertex2f(x2, y1);
			glEnd();
			glDisable(GL_BLEND);
		}
		if(mode == "copyaabb4") {
			for(unsigned int i = 0;i < copiedBodys.size();i ++) {
				if(!copiedBodys[i]) continue;
				copiedBodys[i]->SetTransform(mouse2-mouse1+copiedBodys[i]->GetPosition(), copiedBodys[i]->GetAngle());
				copiedBodys[i]->ApplyForce(b2Vec2(-m_world->GetGravity().x*copiedBodys[i]->GetMass(), -m_world->GetGravity().y*copiedBodys[i]->GetMass()), copiedBodys[i]->GetPosition(), true);
				copiedBodys[i]->SetLinearVelocity(b2Vec2(0, 0));
			}
			mouse1 = mouse2;
		}
		if(mode == "copyaabb3") {
			for(unsigned int i = 0;i < copiedBodys.size();i ++) {
				if(!copiedBodys[i]) continue;
				copiedBodys[i]->ApplyForce(b2Vec2(-m_world->GetGravity().x*copiedBodys[i]->GetMass(), -m_world->GetGravity().y*copiedBodys[i]->GetMass()), copiedBodys[i]->GetPosition(), true);
				copiedBodys[i]->SetLinearVelocity(b2Vec2(0, 0));
			}
		}
		if (mode == "archer" || mode == "saber" || mode == "rocketguard" || mode == "engine" || mode == "rocketattack" ||
			mode == "balancer" || mode == "main" || mode == "monitor" || mode == "lamp" || mode == "cap"
			|| mode == "repeater") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glBegin(GL_POLYGON);
			glTexCoord2f(mouse2.x-2, mouse2.y-2); glVertex2f(mouse2.x-2, mouse2.y-2);
			glTexCoord2f(mouse2.x-2, mouse2.y+2); glVertex2f(mouse2.x-2, mouse2.y+2);
			glTexCoord2f(mouse2.x+2, mouse2.y+2); glVertex2f(mouse2.x+2, mouse2.y+2);
			glTexCoord2f(mouse2.x+2, mouse2.y-2); glVertex2f(mouse2.x+2, mouse2.y-2);
			glEnd();
			glDisable(GL_BLEND);
		}
		if (mode == "railgun") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glBegin(GL_POLYGON);
			glTexCoord2f(mouse2.x - 3, mouse2.y - 3); glVertex2f(mouse2.x - 3, mouse2.y - 3);
			glTexCoord2f(mouse2.x - 3, mouse2.y + 3); glVertex2f(mouse2.x - 3, mouse2.y + 3);
			glTexCoord2f(mouse2.x + 3, mouse2.y + 3); glVertex2f(mouse2.x + 3, mouse2.y + 3);
			glTexCoord2f(mouse2.x + 3, mouse2.y - 3); glVertex2f(mouse2.x + 3, mouse2.y - 3);
			glEnd();
			glDisable(GL_BLEND);
		}
		if (mode == "launcher") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
			glBegin(GL_POLYGON);
			glTexCoord2f(mouse2.x - 6, mouse2.y - 2); glVertex2f(mouse2.x - 6, mouse2.y - 2);
			glTexCoord2f(mouse2.x - 6, mouse2.y + 2); glVertex2f(mouse2.x - 6, mouse2.y + 2);
			glTexCoord2f(mouse2.x + 6, mouse2.y + 2); glVertex2f(mouse2.x + 6, mouse2.y + 2);
			glTexCoord2f(mouse2.x + 6, mouse2.y - 2); glVertex2f(mouse2.x + 6, mouse2.y - 2);
			glEnd();
			glDisable(GL_BLEND);
		}
		HYFEditor::Step2(settings);

		char buf[50];
		sprintf(buf, "price:%f", this->totalCost);
		//strcat("price:", itoa(this->totolCost, buf, 10));
		if(strcmp(entry->name, "主界面")) curTest->m_debugDraw.DrawString(20, 40, buf);


	}

	void MouseMove(const b2Vec2& p) {

		for(unsigned int i = 0;i < Raffica::ButtonList.size();i ++) {
			((BasicUserData*)(Raffica::ButtonList[i]->body->GetUserData()))->textureIndex = LoadTexture(Raffica::ButtonList[i]->urls[0].c_str());
		}									//清除所有的按钮已经按下的标记 以防按下时按钮移开后还显示已经按下

		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if(body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if(ud && ud->isButton) {
			if(!ud->button->isMaterialButton) ud->button->pressMove(p, &mode, ud->button);
			else ud->button->pressMove(p, &material, ud->button);
		} else Test::MouseMove(p);

		if(mode == "normal") mouse1 = p, mouse2 = p;
		else {
			mouse2 = p;
		}
	}

	void Keyboard(unsigned char key) override {
		if (mode == "login") {
			Raffica::handleChat(key);
		}
		else {
			//TODO: G is gravity while T stands stop
			if (key == 'G' || key == 'g') {
				gravityUp(b2Vec2(0, 0), &mode, gravityBody);
			}
			else if (key == 'T' || key == 't') {
				for (b2Body* i = m_world->GetBodyList(); i; i = i->GetNext()) {
					i->SetLinearVelocity(b2Vec2(0, 0));
					i->SetAngularVelocity(0);
				}
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////
	/******************************************分界线************************************************/
	//////////////////////////////////////////////////////////////////////////////////////////////////


	RaEditor() {
		this->type = "normal";
		this->continuous = false;
		material = "iron";
	}

	~RaEditor() {
		((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);
		for(b2Body* i = m_world->GetBodyList();i;i = i->GetNext()) {
			delete (i->GetUserData());
		}
	}

	static Test* Create() {
		return new RaEditor;
	}

	void createButtons() {
		vector<string> button1, button2, button3, button4, button5, button6, button7, button8, button9, buttona, buttonb, buttonc;
		vector<string> buttond, buttone, buttonf, buttong, buttonh, buttoni, buttonj, buttonk, buttonl, buttonm, buttonn;
		button1.push_back("pic/rjbox.bmp");
		button1.push_back("pic/rjbox2.bmp");
		button2.push_back("pic/rjsave.bmp");
		button2.push_back("pic/rjsave2.bmp");
		button3.push_back("pic/rjdelete.bmp");
		button3.push_back("pic/rjdelete2.bmp");
		button4.push_back("pic/rjball.bmp");
		button4.push_back("pic/rjball2.bmp");
		button5.push_back("pic/rjsavenow.bmp");
		button5.push_back("pic/rjsavenow2.bmp");
		button6.push_back("pic/rjgravity.bmp");
		button6.push_back("pic/rjgravity2.bmp");
		button7.push_back("pic/rjcopy.bmp");
		button7.push_back("pic/rjcopy2.bmp");
		button8.push_back("pic/rjcopyaabb.bmp");
		button8.push_back("pic/rjcopyaabb2.bmp");
		button9.push_back("pic/rjdeleteaabb.bmp");
		button9.push_back("pic/rjdeleteaabb2.bmp");
		buttona.push_back("pic/rjjoint.bmp");
		buttona.push_back("pic/rjjoint2.bmp");
		buttonb.push_back("pic/rjdefend.bmp");
		buttonb.push_back("pic/rjdefend2.bmp");
		buttonc.push_back("pic/rjrestart.bmp");
		buttonc.push_back("pic/rjrestart2.bmp");
		buttond.push_back("pic/rjeditattack.bmp");
		buttond.push_back("pic/rjeditattack2.bmp");
		buttone.push_back("pic/rjeditdefend.bmp");
		buttone.push_back("pic/rjeditdefend2.bmp");
		buttonf.push_back("pic/rjcontinuous.bmp");
		buttonf.push_back("pic/rjcontinuous2.bmp");
		buttong.push_back("pic/rjtest.bmp");
		buttong.push_back("pic/rjtest2.bmp");
		buttonh.push_back("pic/main.bmp");
		buttonh.push_back("pic/main2.bmp");
		buttoni.push_back("pic/rjlogin.bmp");
		buttoni.push_back("pic/rjlogin2.bmp");
		buttonj.push_back("pic/panel.png");
		buttonj.push_back("pic/panel.png");
		buttonk.push_back("pic/rjrjoint.bmp");
		buttonk.push_back("pic/rjrjoint2.bmp");
		buttonl.push_back("pic/PVP.png");
		buttonl.push_back("pic/PVP2.png");
		buttonm.push_back("pic/panel2.png");
		buttonm.push_back("pic/panel2.png");
		buttonn.push_back("pic/rjliquid.bmp");
		buttonn.push_back("pic/rjliquid2.bmp");

		const int unused = 1000;
		
		panelButton = Raffica::createButton		(38, 0, 15, 25,buttonj, "panel");
		panelButton2 = Raffica::createButton	(-25, -25, 48,10, buttonm, "panel");

		createBoxBody = Raffica::createButton	(-34, -19.5f, 2.5f, 1.4f, button1, "box");
		createBallBody = Raffica::createButton	(-34, -22.5f, 2.5f, 1.4f, button4, "ball");
		gravityBody = Raffica::createButton		(-28, -22.5f, 2.5f, 1.4f, button6, "gravity");
		continuousBody = Raffica::createButton	(-28, -19.5f, 2.5f, 1.4f, buttonf, "continuous");
		deleteBody = Raffica::createButton		(-22, -22.5f, 2.5f, 1.4f, button3, "delete");
		deleteAABBBody = Raffica::createButton	(-22, -19.5f, 2.5f, 1.4f, button9, "deleteaabb");
		copyBody = Raffica::createButton		(-16, -22.5f, 2.5f, 1.4f, button7, "copy");
		copyAABBBody = Raffica::createButton	(-16, -19.5f, 2.5f, 1.4f, button8, "copyaabb");
		jointBody = Raffica::createButton		(-10, -22.5f, 2.5f, 1.4f, buttona, "joint");
		rjointBody = Raffica::createButton		(-10, -19.5f, 2.5f, 1.4f, buttonk, "rjoint");
		defenderBody = Raffica::createButton	(-4, -22.5f, 2.5f, 1.4f, buttonb, "defender");
		mainBody = Raffica::createButton		(20, -21, 2.5f, 1.4f, buttonh, "main");
		createLiquidBody = Raffica::createButton(2, -21 + unused, 2.5f, 1.4f, buttonn, "liquid");

		restartButton = Raffica::createButton	(36, 14 + unused,  2.5f, 1.4f, buttonc, "restart");
		editAttackButton = Raffica::createButton(36, 8,  2.5f, 1.4f, buttond, "editattack");
		editDefendButton = Raffica::createButton(36, 2 + unused, 2.5f, 1.4f, buttone, "editdefend");
		loginButton = Raffica::createButton		(36, 20 + unused, 2.5f, 1.4f, buttoni, "login");
		PVPButton = Raffica::createButton		(36, -4 + unused, 2.5f, 1.4f, buttonl, "PVP");
		if (strcmp(entry->name, "主界面"))
			savenowBody = Raffica::createButton(30, 10, 2.5f, 1.4f, button5, "save2");
		//testBody = Raffica::createButton		(-24,22, 2, 1, buttong, "test");

		//createBoxBody->_pressDown = createBoxDown;
		createBoxBody->setPressUp(createBoxUp);
		createBallBody->setPressUp(createBallUp);
		if (strcmp(entry->name, "主界面")) savenowBody->setPressUp(savenowUp);
		deleteBody->setPressUp(deleteUp);
		gravityBody->setPressUp(gravityUp);
		copyBody->setPressUp(copyUp);
		copyAABBBody->setPressUp(copyaabbUp);
		deleteAABBBody->setPressUp(deleteaabbUp);
		jointBody->setPressUp(jointUp);
		defenderBody->setPressUp(defenderUp);
		restartButton->setPressUp(restartUp);
		editAttackButton->setPressUp(editAttackUp);
		editDefendButton->setPressUp(editDefendUp);
		PVPButton->setPressUp(PVPUp);
		continuousBody->setPressUp(continuousUp);
		//testBody->setPressUp(testUp);
		mainBody->setPressUp(mainUp);
		loginButton->setPressUp(loginUp);
		rjointBody->setPressUp(rjointUp);
		createLiquidBody->setPressUp(liquidUp);

		((BreakableUserData*)createBoxBody->body->GetUserData())->tip = "创建一个方形物体";
		((BreakableUserData*)createBallBody->body->GetUserData())->tip = "创建一个球形物体";
		if(strcmp(entry->name, "主界面")) ((BreakableUserData*)savenowBody->body->GetUserData())->tip = "保存";
		((BreakableUserData*)deleteBody->body->GetUserData())->tip = "删除一个物体";
		((BreakableUserData*)gravityBody->body->GetUserData())->tip = "开/关重力";
		((BreakableUserData*)copyBody->body->GetUserData())->tip = "复制一个物体";
		((BreakableUserData*)copyAABBBody->body->GetUserData())->tip = "复制指定范围内的物体";
		((BreakableUserData*)deleteAABBBody->body->GetUserData())->tip = "删除指定范围内的物体";
		((BreakableUserData*)jointBody->body->GetUserData())->tip = "在两个物体间创建钢筋";
		((BreakableUserData*)defenderBody->body->GetUserData())->tip = "创建防御性Object";
		((BreakableUserData*)restartButton->body->GetUserData())->tip = "随机指定服务器内的一个人发起进攻";
		((BreakableUserData*)editAttackButton->body->GetUserData())->tip = "编辑攻城车";
		((BreakableUserData*)editDefendButton->body->GetUserData())->tip = "编辑城堡";
		((BreakableUserData*)PVPButton->body->GetUserData())->tip = "PVP";
		((BreakableUserData*)continuousBody->body->GetUserData())->tip = "连续操作";
		((BreakableUserData*)mainBody->body->GetUserData())->tip = "创建基地";
		((BreakableUserData*)rjointBody->body->GetUserData())->tip = "在两个物体间创建R导线";
		((BreakableUserData*)panelButton->body->GetUserData())->tip = "面板";
		((BreakableUserData*)panelButton2->body->GetUserData())->tip = "面板2";

		int j = 0;

		for(map<string, Material*>::iterator i = Raffica::materialList.begin();i != Raffica::materialList.end();++ i, ++ j) {		//创建材质按钮
			vector<string> buturl;
			buturl.push_back(i->second->buttonUpURL); buturl.push_back(i->second->buttonDownURL);
			RaButton* bu;
			bu = Raffica::createButton(-23 + 4 * (float)j, -17, 2, 1, buturl, i->second->name);
			bu->isMaterialButton = true;
			bu->material = i->second;
			bu->setPressUp(materialUp);
		}

		HYFEditor::createButtons();
	}


	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		if (Raffica::drawTitleName != "主界面") return;
		b2Manifold* manifold = contact->GetManifold();
		int cnt = manifold->pointCount;
		b2WorldManifold* wmanifold = new b2WorldManifold();
		contact->GetWorldManifold(wmanifold);
		UserData *ud1 = (UserData*)contact->GetFixtureA()->GetBody()->GetUserData();
		UserData *ud2 = (UserData*)contact->GetFixtureB()->GetBody()->GetUserData();
		float sharp1, sharp2;

		if (contact == NULL) return;
		if (ud1 == NULL) sharp1 = 1.0f; else sharp1 = ud1->sharp;
		if (ud2 == NULL) sharp2 = 1.0f; else sharp2 = ud2->sharp;

		if (ud1 != NULL) {
			if (ud1->breakable) {
				BreakableUserData* bud1 = (BreakableUserData*)ud1;
				for (int i = 0; i < cnt; i++) {
					if (sharp2 * impulse->normalImpulses[i] / 10 > bud1->defend) {
						float dmg = sharp2 * impulse->normalImpulses[i] / 10 - bud1->defend;
						bud1->hp -= dmg / 10;
					}
				}
			}
		}

		if (ud2 != NULL) {
			if (ud2->breakable) {
				BreakableUserData* bud2 = (BreakableUserData*)ud2;
				for (int i = 0; i < cnt; i++) {
					if (sharp1 * impulse->normalImpulses[i] / 10 > bud2->defend) {
						float dmg = sharp1 * impulse->normalImpulses[i] - bud2->defend;
						bud2->hp -= dmg / 10;
					}
				}
			}
		}
	}
};


class RaDefendEditor : public RaEditor {

public:

	static Test* Create() {
		return new RaDefendEditor;
	}

	RaDefendEditor() {
		this->type = "defend";
	}

	void init() {

		string __debugTMP = mainUseAttack;

		curTest->totalCost = 0;

		((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &mode);

		string _castlename;
		for (int i = 0; i < castleName.size() && castleName[i] != ' ' && castleName[i] != '\0'; i++) {
			_castlename.push_back(castleName[i]);
		}

		client.sendMsgByHello(("get " + _castlename + "\\info.txt").c_str());
		client.getFile("info.txt", "");

		Raffica::loadWorld(curTest, "info.txt");

		this->worldFileName = "info.txt";
		Raffica::makeWall(370, 80, 70);
		leftBorder = -100;
		rightBorder = 190;
		createButtons();
		m_world->SetGravity(b2Vec2(0.0f, 0.0f));


		curTest->m_particleSystem->SetRadius(Raffica::LIQUID_RADIUS);
		curTest->m_particleSystem->SetDamping(Raffica::LIQUID_DAMPING);
		curTest->m_particleSystem->SetDensity(Raffica::LIQUID_DENSITY);


		mainUseAttack = __debugTMP;
	}

};

