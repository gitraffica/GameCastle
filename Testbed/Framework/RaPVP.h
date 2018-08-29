#pragma once


/*
* Author:Raffica
*/


/*
* RaPVP
* Author:Raffica
*
* PVP世界
*
*/

void Restart(int);
void EditAttack(int);
void EditLevel(int);

extern string announcement;
extern string chattingText;
extern RaClient chatClient;
extern RaClient PVPClient;
extern bool leftPressed;
extern bool rightPressed;
extern bool upPressed;
extern bool downPressed;


class RaPVP : public Test {

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

	//test
	RaButton* testBody;
	RaButton* testBody2;
	RaButton* testBody3;
	RaButton* testBody4;
	//test

	string mode;
	string material;

	static string clientUser;
	string serverUser;
	static int isPVPServer;									//0 未決定 1 是 2 是client
	static int ServerPort;
	static string ServerIP;
	static int serverAccepted;
	static bool clientConnected;

	RaServerThread* PVPServer;

	b2Vec2 mouse1, mouse2;
	int FrameCD = 10;
	////////////////////////////////////////////////////////////

	RaPVP() {

		string __debugTMP = mainUseAttack;

		setModeNormal(&mode);

		StaticUserData* sud = new StaticUserData();
		sud->Init(1300, 300, 0, 100, 0);
		sud->setTexture(LoadTexture("pic//background.bmp"));

		Raffica::makeWall(980, 80);

		createButtons();

		type = "PVP";

		leftBorder = -530;
		rightBorder = 490;

		Raffica::cameraTo = 100;

		//box 0 0 40 2 0 3 0.3 1 65535 0
		//	breakable 100000 1e+06 2 3 pic / ground.bmp end
		b2Body* tmp = Raffica::createBox(0, -60, 1500, 22, 0, 3, .3f, NULL, 1, 65535);
		BreakableUserData* gud = new BreakableUserData();
		vector<int>* gtex = new vector<int>();
		gtex->push_back(LoadTexture("pic/nightground.png"));
		gud->Init(100000000, 10000000, gtex);
		Raffica::setUserDataByTexture(tmp, gud, 1, 3000, 44);					//地板

		isPVPServer = 0;
		serverAccepted = 0;
		clientConnected = 0;


		mainUseAttack = __debugTMP;
	}

	~RaPVP() {
		ofstream ofs("PVP.txt");
		ofs << 0;
		ofs.close();
		client.sendMsgByHello(("send " + username + "\\PVP.txt").c_str());
		client.sendFile("PVP.txt");

		for (b2Body* i = m_world->GetBodyList(), *j; i; i = j) {
			j = i->GetNext();
			if (i) if (i->GetUserData()) {
				BreakableUserData* ud = (BreakableUserData*)i->GetUserData();
				delete ud;
			}
		}
	}
	//TODO:傳入一個參數使得能夠在這裏判斷是不是客戶端
	////////////////////////////////////////////////////////////////////////////////////////

	void init() {

	}

	////////////////////////////////////////////////////////////////////////////////////////
	static void attackUp(const b2Vec2& p, string* mode, const RaButton* button) {
		SETNORMAL;
		curTest->inputing = false;
		client.sendMsgByHello(("get " + username + "\\attackinfo.txt").c_str());
		client.getFile("attackinfo.txt");
		Raffica::loadWorld(curTest, "attackinfo.txt", true, 150);

		client.sendMsgByHello(("get " + clientUser + "\\attackinfo.txt").c_str());
		client.getFile("attackinfo.txt");
		Raffica::loadWorld(curTest, "attackinfo.txt", false);

		testUp(b2Vec2(), &string(), ((RaPVP*)curTest)->testBody);
		delete ((RaPVP*)curTest)->attackButton;
		((RaPVP*)curTest)->attackButton = 0;
		delete ((RaPVP*)curTest)->restartButton;
		((RaPVP*)curTest)->restartButton = 0;
		delete ((RaPVP*)curTest)->editAttackButton;
		((RaPVP*)curTest)->editAttackButton = 0;
		delete ((RaPVP*)curTest)->editDefendButton;
		((RaPVP*)curTest)->editDefendButton = 0;
		delete ((RaPVP*)curTest)->cameraFollowButton;
		((RaPVP*)curTest)->cameraFollowButton = 0;
		delete ((RaPVP*)curTest)->panelButton;
		((RaPVP*)curTest)->panelButton = 0;
		((RaPVP*)curTest)->createButtons();						//防止新绘制出的攻城车把按钮覆盖了
		delete ((RaPVP*)curTest)->attackButton;
		((RaPVP*)curTest)->attackButton = 0;
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

	//test

	static void sendNowPVP() {
		Raffica::savenowWorld("_test.txt", (username + "\\test").c_str(), true);
	}

	static void loadNowPVP() {
		client.sendMsgByHello(("get " + ((RaPVP*)curTest)->serverUser + "\\test\\_test.txt").c_str());
		//cout << ("get " + ((RaPVP*)curTest)->serverUser + "\\test\\_test.txt").c_str() << endl;
		client.getFile("_test.txt", "");
		Raffica::loadWorld(curTest, "_test.txt", false, 0, true);
		Raffica::PVPdeltaTime = 0;
	}

	static void testUp(const b2Vec2& p, string* mode, const RaButton* button) {
		sendNowPVP();
	}

	static void testUp2(const b2Vec2& p, string* mode, const RaButton* button) {
		loadNowPVP();
		if (isPVPServer == 2) cout << "判定沒錯\n";
	}

	static void testUp3(const b2Vec2& p, string* mode, const RaButton* button) {
		static int serverStarted = 0;

		((RaPVP*)curTest)->PVPServer = new RaServerThread();
		auto& serverThread = ((RaPVP*)curTest)->PVPServer;
		RaServer& server = serverThread->servers[0];

		while (RaServer::initSocket(rand() * 10000 / 32767 + 1) == -1);
		//while (1) {
			//serverThread->runThread();
			/*
			server.clientSocket = RaServer::acceptClient();			//從listenSocket裏面accept到的clientSocket
			cout << "友利奈緒\n";
			settings.pause = true;
			if (server.clientSocket == -1) return;*/
		//}


		ofstream ofs("PVP.txt");
		ofs << "1 1";							//正在接受PVP；沒有需要接走的world
		ofs.close();
		client.sendMsgByHello(("send " + username + "\\PVP.txt").c_str());
		//cout << ("send " + username + "\\PVP.txt").c_str() << endl;
		client.sendFile("PVP.txt");

		ofs.open("_test.txt");
		ofs << " ";							//正在接受PVP；沒有需要接走的world
		ofs.close();
		client.sendMsgByHello(("send " + username + "\\test\\_test.txt").c_str());
		//cout << ("send " + username + "\\_test.txt").c_str() << endl;
		client.sendFile("_test.txt");
		isPVPServer = 1;
	}

	static void handleAsServer() {
		int ablePVP, worldGet;

		client.sendMsgByHello(("get " + username + "\\PVP.txt").c_str());
		client.getFile("PVP.txt");

		ifstream ifs("PVP.txt");
		ifs >> ablePVP >> worldGet;

		if (worldGet == 0) {

			clientConnected = 1;						//说明有人连进来了

			string op, op2;				//单击 键盘
			float x, y;
			ifs >> op >> x >> y >> op2 >> clientUser;
			if (op == "1") {
				Raffica::aimWork(x, y, true);
			}
			if (op2[0] == '0') {
				Raffica::engineWork(-engineStrength, 0, true);
			}
			if (op2[1] == '0') {
				Raffica::engineWork(engineStrength, 0, true);
			}
			if (op2[2] == '0') {
				Raffica::engineWork(0, engineStrength, true);
			}
			if (op2[3] == '0') {
				Raffica::engineWork(0, -engineStrength, true);
			}

			sendNowPVP();
			ofstream ofs("PVP.txt");
			ofs << "1 1 " << Raffica::PVPisMouseDown << ' ' << Raffica::PVPMouseX << ' ' << Raffica::PVPMouseY;
			//正在接受PVP；沒有需要接走的world
			ofs.close();
			client.sendMsgByHello(("send " + username + "\\PVP.txt").c_str());
			client.sendFile("PVP.txt");
			//cout << "SendWorld\n";
		}
	}

	static void handleAsClient() {
		int ablePVP, worldGet;

		client.sendMsgByHello(("get " + ((RaPVP*)curTest)->serverUser + "\\PVP.txt").c_str());
		client.getFile("PVP.txt");
		
		ifstream ifs("PVP.txt");
		ifs >> ablePVP >> worldGet;
		if (worldGet == 1) {

			string op;				//单击 键盘
			float x, y;

			ifs >> op >> x >> y;
			if (op == "1") Raffica::aimWork(x, y, false);

			loadNowPVP();
			ofstream ofs("PVP.txt");
			ofs << "1 0\n";							//正在接受PVP；沒有需要接走的world

			Raffica::PVPKeyboard.resize(4);
			if (leftPressed)  Raffica::PVPKeyboard[0] = '0'; else Raffica::PVPKeyboard[0] = '1';
			if (rightPressed) Raffica::PVPKeyboard[1] = '0'; else Raffica::PVPKeyboard[1] = '1';
			if (upPressed)    Raffica::PVPKeyboard[2] = '0'; else Raffica::PVPKeyboard[2] = '1';
			if (downPressed)  Raffica::PVPKeyboard[3] = '0'; else Raffica::PVPKeyboard[3] = '1';

			ofs << Raffica::PVPisMouseDown << ' ' << Raffica::PVPMouseX << ' ' << \
				Raffica::PVPMouseY << endl << Raffica::PVPKeyboard << ' ' << username << endl;

			if (Raffica::PVPisMouseDown == true) Raffica::PVPisMouseDown = false;

			ofs.close();
			client.sendMsgByHello(("send " + ((RaPVP*)curTest)->serverUser + "\\PVP.txt").c_str());
			client.sendFile("PVP.txt");
			//cout << "GetWorld\n";
		}

		((RaPVP*)curTest)->m_world->SetGravity(b2Vec2(0, 0));
	}

	static void testUp4(const b2Vec2& p, string* mode, const RaButton* button) {
		Raffica::analyseUser();
		Raffica::createPVPUserButton();
	}

	static void setModeNormal(string* md) {
		*md = "normal";
		if (curTest) curTest->inputing = false;
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
		if (body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if (ud && ud->isButton) {											//如果点到了按钮
			ud->button->pressDown(p, &mode, ud->button);
		}
		else if (!ud) {
			if (mode == "normal") {
				if (this->isPVPServer == 2) {		//client
					//Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y, true);
				} else Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y, false);
				
				Raffica::PVPisMouseDown = true;
				Raffica::PVPMouseX = mouse.x;
				Raffica::PVPMouseY = mouse.y;
			}
		}
		else {
			//Test::MouseDown(p);
			if (this->isPVPServer == 2) {			//client
				//Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y, true);
			}
			else Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y, false);

			Raffica::PVPisMouseDown = true;
			Raffica::PVPMouseX = mouse.x;
			Raffica::PVPMouseY = mouse.y;
		}
	}

	void MouseUp(const b2Vec2& p) override {
		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if (body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if (ud && ud->isButton) {											//如果点到了按钮
			cout << ud->isButton << endl;
			cout << ud->button->name << endl;					//////////////////////
			ud->button->pressUp(p, &mode, ud->button);
		}
		else Test::MouseUp(p);
	}

	void Step2(Settings* settings) override {													//单帧执行，图层比正常图层前面
		float x1, x2, y1, y2;
		x1 = min(mouse1.x, mouse2.x);
		x2 = max(mouse1.x, mouse2.x);
		y1 = min(mouse1.y, mouse2.y);
		y2 = max(mouse1.y, mouse2.y);
		if (mode == "attack2") {
			glLoadIdentity();
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
		for (unsigned int i = 0; i < Raffica::ButtonList.size(); i++) {
			((BasicUserData*)(Raffica::ButtonList[i]->body->GetUserData()))->textureIndex = LoadTexture(Raffica::ButtonList[i]->urls[0].c_str());
		}									//清除所有的按钮已经按下的标记 以防按下时按钮移开后还显示已经按下

		b2Body* body = getBodyAtMouse(p, this);
		BasicUserData* ud;
		if (body) ud = (BasicUserData*)body->GetUserData(); else ud = 0;
		if (ud && ud->isButton) {
			if (!ud->button->isMaterialButton) ud->button->pressMove(p, &mode, ud->button);
			else ud->button->pressMove(p, &material, ud->button);
		}
		else Test::MouseMove(p);

		if (mode == "normal") mouse1 = p, mouse2 = p;
		else {
			mouse2 = p;
		}
	}

	void createButtons() {
		vector<string> button1, button2, button3, button4, button5, button6, button7, button8, button9;
		vector<string> buttona;
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
		button9.push_back("pic/PVPServer.png");
		button9.push_back("pic/PVPServer2.png");
		buttona.push_back("pic/PVPClient.png");
		buttona.push_back("pic/PVPClient2.png");

		panelButton =			Raffica::createButton(48, 0, 25, 25, button7, "panel");
		//headButton =			Raffica::createButton(40, 6, 2, 2, button8, "head");
		attackButton =			Raffica::createButton(38, 24, 2, 1, button1, "attack");
		restartButton =			Raffica::createButton(38, 0, 2, 1, button2, "restart");
		editAttackButton =		Raffica::createButton(38, -2, 2, 1, button3, "editattack");
		editDefendButton =		Raffica::createButton(38, -4, 2, 1, button4, "editdefend");
		cameraFollowButton =	Raffica::createButton(38, 16, 2, 1, button5, "camerafollow");
		loginButton =			Raffica::createButton(38, -6, 2, 1, button6, "login");
		testBody =				Raffica::createButton(-24, 22, 2, 1, button9, "test");
		testBody2 =				Raffica::createButton(-24, 20, 2, 1, buttona, "test2");
		testBody3 =				Raffica::createButton(-24, 16, 2, 1, button9, "test3");
		testBody4 =				Raffica::createButton(-24, 14, 2, 1, buttona, "test4");

		attackButton->setPressUp(attackUp);
		restartButton->setPressUp(restartUp);
		editAttackButton->setPressUp(editAttackUp);
		editDefendButton->setPressUp(editDefendUp);
		cameraFollowButton->setPressUp(cameraFollowUp);
		loginButton->setPressUp(loginUp);
		//headButton->setPressUp(headUp);			//头像按钮被我废止了
		testBody->setPressUp(testUp);
		testBody2->setPressUp(testUp2);
		testBody3->setPressUp(testUp3);
		testBody4->setPressUp(testUp4);
	}
	///////////////////////////////////////////////////////
	static Test* Create() {
		return new RaPVP;
	}

	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {

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

	void EnterFrame() override {

		FrameCD--;
		if (FrameCD < 0) {
			FrameCD = 1;

			Raffica::userdataList.push_back(0);		//虽然不知道为什么，但是它修好了一个疑似内存越界访问导致的bug

			if (isPVPServer == 1) {
				//cout << "服務器です\n";
				handleAsServer();
				//sendNowPVP();

				/* 如果是第一次clientConnected == 1说明有人连进来了 */
				static bool check = false;							//此类static变量有可能导致不能重复开pvp的问题
				if (clientConnected == 1 && check == false) {
					check = true;
					attackUp(b2Vec2(0, 0), &mode, attackButton);
				}
			}

			if (isPVPServer == 2) {
				//cout << "客戶端です\n";
				handleAsClient();
				//loadNowPVP();
			}
		}
		if(isPVPServer != 2) Raffica::PVPisMouseDown = 0;
		Raffica::PVPKeyboard = "";
	}


};