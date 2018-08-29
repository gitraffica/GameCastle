#pragma once

/*
* Author:Raffica
*/


/*
* RaWorld
* Author:Raffica
*
* 生成世界，并处理碰撞
*
*/

void Restart(int);
void EditAttack(int);
void EditLevel(int);

extern string announcement;
extern string chattingText;
extern RaClient chatClient;


class RaWorld : public Test {

public:


	//////////////////////////////////////////////////////////
	RaButton* attackButton;
	RaButton* restartButton;
	RaButton* editAttackButton;
	RaButton* editDefendButton;
	RaButton* cameraFollowButton;
	RaButton* loginButton;
	RaButton* panelButton;									//面板 但是可以被归为一个按钮
	RaButton* headButton;									//头像 同理可以被归为按钮
	RaButton* PVPButton;

	string mode;
	string material;

	b2Vec2 mouse1, mouse2;
	////////////////////////////////////////////////////////////

	RaWorld() {

		string __debugTMP = mainUseAttack;

		setModeNormal(&mode);

		StaticUserData* sud = new StaticUserData();
		sud->Init(1300, 300, 0, 100, 0);
		sud->setTexture(LoadTexture("pic//background.bmp"));

		Raffica::matchWorld();
		
		Raffica::loadWorld(this, "info.txt", false, 320);

		Raffica::makeWall(980, 80);

		createButtons();

		type = "mainworld";
		

		leftBorder = -530;
		rightBorder = 490;

		Raffica::cameraTo = 100;
		

		curTest->m_particleSystem->SetRadius(Raffica::LIQUID_RADIUS);
		curTest->m_particleSystem->SetDamping(Raffica::LIQUID_DAMPING);
		curTest->m_particleSystem->SetDensity(Raffica::LIQUID_DENSITY);

		mainUseAttack = __debugTMP;
	}

	~RaWorld() {
		for(b2Body* i = m_world->GetBodyList(), *j;i;i = j) {
			j = i->GetNext();
			if (i) if (i->GetUserData()) {
				BreakableUserData* ud = (BreakableUserData*)i->GetUserData();
				delete ud;
			}
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////
	static void attackUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		curTest->inputing = false;
		string __debugTMP = mainUseAttack;
		client.sendMsgByHello(("get " + mainUseAttack + "\\attackinfo.txt").c_str());
		client.getFile("attackinfo.txt");
		Raffica::loadWorld(curTest, "attackinfo.txt", true, 150);

		mainUseAttack = __debugTMP;
		delete ((RaWorld*)curTest)->attackButton;
		((RaWorld*)curTest)->attackButton = 0;
		delete ((RaWorld*)curTest)->restartButton;
		((RaWorld*)curTest)->restartButton = 0;
		delete ((RaWorld*)curTest)->editAttackButton;
		((RaWorld*)curTest)->editAttackButton = 0;
		delete ((RaWorld*)curTest)->editDefendButton;
		((RaWorld*)curTest)->editDefendButton = 0;
		delete ((RaWorld*)curTest)->cameraFollowButton;
		((RaWorld*)curTest)->cameraFollowButton = 0;
		delete ((RaWorld*)curTest)->panelButton;
		((RaWorld*)curTest)->panelButton = 0;
		((RaWorld*)curTest)->createButtons();						//防止新绘制出的攻城车把按钮覆盖了
		delete ((RaWorld*)curTest)->attackButton;
		((RaWorld*)curTest)->attackButton = 0;
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

	//点了这个 摄像机可以在跟随和不跟随战车的状态间切换

	static void cameraFollowUp(const b2Vec2& p, string* mode, const RaButton* button) {
		Raffica::followCamera = !Raffica::followCamera;
	}

	//更换头像

	static void headUp(const b2Vec2& p, string* mode, const RaButton* button) {
		static bool fileOK = false;
		if (fileOK == true) return;

		fileOK = true;
		OPENFILENAME ofn;      // 公共对话框结构。     
		TCHAR szFile[MAX_PATH]; // 保存获取文件名称的缓冲区。               
								// 初始化选择文件对话框。     
		ZeroMemory(&ofn, sizeof(OPENFILENAME));
		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = NULL;
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "png图片(*.png)\0*.png\0\0";
		ofn.nFilterIndex = 1;
		ofn.lpstrFileTitle = NULL;
		ofn.nMaxFileTitle = 0;
		ofn.lpstrInitialDir = NULL;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		//ofn.lpTemplateName =  MAKEINTRESOURCE(ID_TEMP_DIALOG);    
		// 显示打开选择文件对话框

		if (GetOpenFileName(&ofn)) {
			fileOK = false;
			//显示选择的文件
			const_cast<string&>(button->urls[0]) = szFile;
			const_cast<string&>(button->urls[1]) = szFile;
			LoadTexture(szFile);								//防止用户传些奇怪的东西
			client.sendMsgByHello(("send " + username + ".png").c_str());
			client.sendFile(szFile);
		}
	}

	//点了这个你就能聊天了

	static void loginUp(const b2Vec2& p, string* mode, const RaButton* button) {
		if (*mode != "login") *mode = "login";
		else setModeNormal(mode);
	}

	static void PVPUp(const b2Vec2& p, string* mode, const RaButton* button) {
		startPVP(1);
	}

	static void setModeNormal(string* md) {
		*md = "normal";
		if(curTest) curTest->inputing = false;
	}

	void Keyboard(unsigned char key) override {
		if (mode == "login") {
			Raffica::handleChat(key);
		}
		else {
			//TODO: emmmmmmmmmmmI don't know, 233333
		}
	}

	void MouseDown(const b2Vec2& p) override {
		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if(body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if(ud && ud->isButton) {											//如果点到了按钮
			ud->button->pressDown(p, &mode, ud->button);
		} else if(!ud) {
			if (mode == "normal") {
				Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y);
			}
		}
		else {
			//Test::MouseDown(p);
			Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y);
		}
	}

	void MouseUp(const b2Vec2& p) override {
		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if(body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if(ud && ud->isButton) {											//如果点到了按钮
			cout << ud->isButton << endl;
			cout << ud->button->name << endl;					//////////////////////
			ud->button->pressUp(p, &mode, ud->button);
		} else Test::MouseUp(p);
	}

	void Step2(Settings* settings) override {													//单帧执行，图层比正常图层前面
		float x1, x2, y1, y2;
		x1 = min(mouse1.x, mouse2.x);
		x2 = max(mouse1.x, mouse2.x);
		y1 = min(mouse1.y, mouse2.y);
		y2 = max(mouse1.y, mouse2.y);
		if(mode == "attack2") {
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

		Raffica::engineWork(0, 0);					//更新camera位置

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

	void createButtons() {
		vector<string> button1, button2, button3, button4, button5, button6, button7, button8, button9;
		button1.push_back("pic/rjattack.bmp");
		button1.push_back("pic/rjattack2.bmp");
		button2.push_back("pic/rjrestart.bmp");
		button2.push_back("pic/rjrestart2.bmp");
		button3.push_back("pic/rjeditattack.bmp");
		button3.push_back("pic/rjeditattack2.bmp");
		button4.push_back("pic/rjeditdefend.bmp");
		button4.push_back("pic/rjeditdefend2.bmp");
		button5.push_back("pic/rjcamerafollow.bmp");
		button5.push_back("pic/rjcamerafollow2.bmp");
		button6.push_back("pic/rjlogin.bmp");
		button6.push_back("pic/rjlogin2.bmp");
		button7.push_back("pic/panel.png");
		button7.push_back("pic/panel.png");
		button8.push_back("pic/head.png");
		button8.push_back("pic/head.png");
		button9.push_back("pic/PVP.png");
		button9.push_back("pic/PVP2.png");

		panelButton =			Raffica::createButton(48, 0,  25,25,button7, "panel");
		//headButton =			Raffica::createButton(40, 6,  2, 2, button8, "head");
		attackButton =			Raffica::createButton(38, 24, 2, 1, button1, "attack");
		restartButton =			Raffica::createButton(38, 0,  2, 1, button2, "restart");
		editAttackButton =		Raffica::createButton(38, -2, 2, 1, button3, "editattack");
		editDefendButton =		Raffica::createButton(38, -4, 2, 1, button4, "editdefend");
		cameraFollowButton =	Raffica::createButton(38, 16, 2, 1, button5, "camerafollow");
		loginButton =			Raffica::createButton(38, -12,2, 1, button6, "login");
		PVPButton =				Raffica::createButton(38, -6, 2, 1, button9, "PVP");

		attackButton->setPressUp(attackUp);
		restartButton->setPressUp(restartUp);
		editAttackButton->setPressUp(editAttackUp);
		editDefendButton->setPressUp(editDefendUp);
		cameraFollowButton->setPressUp(cameraFollowUp);
		loginButton->setPressUp(loginUp);
		//headButton->setPressUp(headUp);				//废除
		PVPButton->setPressUp(PVPUp);
	}
	///////////////////////////////////////////////////////
	static Test* Create() {
		return new RaWorld;
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
		b2Manifold* manifold = contact->GetManifold();
		int cnt = manifold->pointCount;
		b2WorldManifold* wmanifold = new b2WorldManifold();
		contact->GetWorldManifold(wmanifold);
		UserData *ud1 = (UserData*)contact->GetFixtureA()->GetBody()->GetUserData();
		UserData *ud2 = (UserData*)contact->GetFixtureB()->GetBody()->GetUserData();
		float sharp1, sharp2;

		if(contact == NULL) return;
		if(ud1 == NULL) sharp1 = 1.0f; else sharp1 = ud1->sharp;
		if(ud2 == NULL) sharp2 = 1.0f; else sharp2 = ud2->sharp;

		if(ud1 != NULL) {
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
		
		if(ud2 != NULL) {
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

	//m_world->SetDestructionListener(&raDestructionListener);
		/*
		RaHuman* _human;
		
		Raffica::createBox(0, -2, 20, 2, 0);
		//Raffica::createBall(0, 0, 3);
		Raffica::createCar(0, 13);
		_human = Raffica::createHuman(-2, 23);
				 Raffica::createHuman(8, 23);
		int a  = LoadTexture("rj.bmp");
		int c  = LoadTexture("rj2.bmp");
		int d  = LoadTexture("rj3.bmp");
		b.push_back(d);b.push_back(c);b.push_back(a);
		BreakableUserData* userdata = new BreakableUserData();
		userdata->Init(10000, 200, &b);

		Raffica::setUserData(_human->head, userdata);
		Raffica::setUserDataByTexture(_human->head, userdata, a, _human->headR*2, _human->headR*2);
		Raffica::setUserDataByTexture(_human->body, new UserData(), a, _human->bodyW, _human->bodyH);*/