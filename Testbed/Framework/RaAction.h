#pragma once

int32 testIndex = 0;
int32 testSelection = 0;
int32 testCount = 0;
TestEntry* entry;
Test* curTest;
Settings settings;
int32 width = 1200;
int32 height = 700;
int32 framePeriod = 1000/120;
int32 mainWindow;
float settingsHz = 60.0f;
GLUI *glui;
float32 viewZoom = 4.1772f;
int tx, ty, tw, th;
bool rMouseDown;
b2Vec2 lastp;
string announcement;
string chattingText;
string castleName;
vector<string> castleNames;
string attackerName;
vector<string> attackerNames;
bool mainWinDetermined;						//胜负是否已经决定了？

bool upPressed;
bool downPressed;
bool leftPressed;
bool rightPressed;



/*
* GLUI operator(src from testbed)
*/

void Restart(int);

static void Resize(int32 w, int32 h) {
	glutReshapeWindow(1200, 700);
	width = w;
	height = h;

	float32 ratio = 12.0f / 7.0f;

	GLUI_Master.get_viewport_area(&tx, &ty, &tw, &th);
	glViewport(tx, ty, tw, th);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	// L/R/B/T
	gluOrtho2D(lower.x, upper.x, lower.y, upper.y);
}

static b2Vec2 ConvertScreenToWorld(int32 x, int32 y) {
	float32 u = x / float32(tw);
	float32 v = (th - y) / float32(th);

	float32 ratio = float32(tw) / float32(th);
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= viewZoom;

	b2Vec2 lower = settings.viewCenter - extents;
	b2Vec2 upper = settings.viewCenter + extents;

	b2Vec2 p;
	p.x = (1.0f - u) * lower.x + u * upper.x;
	p.y = 1;
	p.y = (1.0f - v) * lower.y + v * upper.y;
	return p;
}

static void Keyboard(unsigned char key, int x, int y) {
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	if (curTest) curTest->Keyboard(key);
	if (curTest->inputing) return;

	switch (key)
	{
	case 27:
#ifndef __APPLE__
		// freeglut specific function
		glutLeaveMainLoop();
#endif
		exit(0);
		break;

		// Press 'z' to zoom out.
	
		// Press 'r' to reset.
	//case 'r':
		//break;

		// Press space to launch a bomb.
	case ' ':
		if (curTest)
		{
			Raffica::aimWork(((RaEditor*)curTest)->mouse.x, ((RaEditor*)curTest)->mouse.y);
			//test->LaunchBomb();
		}
		break;
 
	case 'p':
		settings.pause = !settings.pause;
		break;

	case 'a':
		leftPressed = true;
		break;

	case 's':
		downPressed = true;
		break;

	case 'd':
		rightPressed = true;
		break;

	case 'w':
		upPressed = true;
		break;
	}

}

static void KeyboardSpecialUp(int key, int x, int y) {
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
	case GLUT_ACTIVE_SHIFT:
		// Press left to pan left.
	case GLUT_KEY_LEFT:
		//settings.viewCenter.x -= 0.5f;
		//Resize(width, height);
		leftPressed = false;
		Raffica::engineWork(-engineStrength, 0);
		break;

		// Press right to pan right.
	case GLUT_KEY_RIGHT:
		//settings.viewCenter.x += 0.5f;
		//Resize(width, height);
		rightPressed = false;
		Raffica::engineWork(engineStrength, 0);
		break;

		// Press down to pan down. 
	case GLUT_KEY_DOWN:
		//settings.viewCenter.y -= 0.5f;
		//Resize(width, height);
		downPressed = false;
		Raffica::engineWork(0, -engineStrength);
		break;

		// Press up to pan up.
	case GLUT_KEY_UP:
		//settings.viewCenter.y += 0.5f;
		//Resize(width, height);
		upPressed = false;
		Raffica::engineWork(0, engineStrength);
		break;

		// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 4.1772f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

static void KeyboardSpecial(int key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	switch (key)
	{
	case GLUT_ACTIVE_SHIFT:
		// Press left to pan left.
	case GLUT_KEY_LEFT:
		//settings.viewCenter.x -= 0.5f;
		//Resize(width, height);
		leftPressed = true;
		break;

		// Press right to pan right.
	case GLUT_KEY_RIGHT:
		//settings.viewCenter.x += 0.5f;
		//Resize(width, height);
		rightPressed = true;
		break;

		// Press down to pan down. 
	case GLUT_KEY_DOWN:
		//settings.viewCenter.y -= 0.5f;
		//Resize(width, height);
		downPressed = true;
		break;

		// Press up to pan up.
	case GLUT_KEY_UP:
		//settings.viewCenter.y += 0.5f;
		//Resize(width, height);
		upPressed = true;
		break;

		// Press home to reset the view.
	case GLUT_KEY_HOME:
		viewZoom = 4.1772f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
		break;
	}
}

static void KeyboardUp(unsigned char key, int x, int y)
{
	B2_NOT_USED(x);
	B2_NOT_USED(y);

	if (curTest->inputing) return;

	switch (key) {

	case 'a':
		leftPressed = false;
		break;

	case 's':
		downPressed = false;
		break;

	case 'd':
		rightPressed = false;
		break;

	case 'w':
		upPressed = false;
		break;

	}

	if (curTest)
	{
		curTest->KeyboardUp(key);
	}
	if(key == 'r') Restart(0);
}

static void Mouse(int32 button, int32 state, int32 x, int32 y)
{
	// Use the mouse to move things around.
	if (button == GLUT_LEFT_BUTTON)
	{
		int mod = glutGetModifiers();
		b2Vec2 p = ConvertScreenToWorld(x, y);
		if (state == GLUT_DOWN)
		{
			b2Vec2 p = ConvertScreenToWorld(x, y);
			if (mod == GLUT_ACTIVE_SHIFT)
			{
				curTest->ShiftMouseDown(p);
			}
			else
			{
				curTest->MouseDown(p);
			}
		}
		
		if (state == GLUT_UP)
		{
			curTest->MouseUp(p);
		}
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (state == GLUT_DOWN)
		{	
			lastp = ConvertScreenToWorld(x, y);
			rMouseDown = true;
		}

		if (state == GLUT_UP)
		{
			rMouseDown = false;
		}
	}
}

static void MousePassiveMotion(int32 x, int32 y) {
	b2Vec2 p = ConvertScreenToWorld(x, y);
	curTest->MouseMaintain(p);
}

static void MouseMotion(int32 x, int32 y)
{
	b2Vec2 p = ConvertScreenToWorld(x, y);
	curTest->MouseMove(p);
	
	if (rMouseDown)
	{
		b2Vec2 diff = p - lastp;
		Raffica::cameraTo -= diff.x;
		//cout << "diff " << diff.x << endl;
		Raffica::checkScreen();
		lastp = ConvertScreenToWorld(x, y);
	}
}

static void MouseWheel(int wheel, int direction, int x, int y)
{
	B2_NOT_USED(wheel);
	B2_NOT_USED(x);
	B2_NOT_USED(y);
	if (direction > 0)
	{
		viewZoom /= 1.1f;
	}
	else
	{
		viewZoom *= 1.1f;
	}
	if (viewZoom > 4.1772f) viewZoom = 4.1772f;
	if (viewZoom < 0.6830f) viewZoom = 0.6830f;
	Raffica::checkScreen();
}

void DisplayFunc();

// This is used to control the frame rate (60Hz).
static void Timer(int) {
	glutSetWindow(mainWindow);
	glutPostRedisplay();
	DisplayFunc();
	glutTimerFunc(framePeriod, Timer, 0);
}

static void DisplayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	curTest->SetTextLine(30);
	b2Vec2 oldCenter = settings.viewCenter;
	settings.hz = settingsHz;

	curTest->mainAlive = 0;
	curTest->attackMainAlive = 0;
	curTest->Step(&settings);						//测试用贴图在此
	curTest->Keyboarding();

	if (curTest->type == "PVP") Raffica::PVPdeltaTime++; else Raffica::PVPdeltaTime = 0;

	Display();									//贴图的函数
	curTest->Step2(&settings);

	glLoadIdentity();
	curTest->m_world->DrawDebugData();							//测试用
	postDisplay();

	Raffica::balancerWork();

	static int winCNT = 0, loseCNT = 0;

	if (curTest->type == "mainworld") if (((RaWorld*)curTest)->attackButton == 0 && !mainWinDetermined) {
		if (curTest->mainAlive == 0) {
			winCNT++;
			if (winCNT > 3) {
				mainWinDetermined = true;
				winCNT = 0;
				announcement.append("win!\r");
				DefendUserData* d = new DefendUserData();
				d->defenderFunc = Raffica::mainDefender;
				d->width = 3, d->height = 3;
				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/main.bmp"));
				d->Init(4000 + width*height * 300, 3008, textures);
				Raffica::createBall(10000, 3, 3, 0, 0, 0, d);

				client.sendMsgByHello(("get " + Raffica::drawTitleName + "\\win.txt").c_str());
				client.getFile("win.txt");
				int win, lose, trial;
				ifstream ifs("win.txt");
				ifs >> win >> lose >> trial;
				ifs.close();
				ofstream ofs("win.txt");
				ofs << win + 1 << ' ' << lose << ' ' << trial;
				ofs.close();
				client.sendMsgByHello(("send " + Raffica::drawTitleName + "\\win.txt").c_str());
				client.sendFile("win.txt");
			}
		}
		else winCNT = 0;
		if (curTest->attackMainAlive == 0) {
			loseCNT++;
			if (loseCNT > 3) {
				mainWinDetermined = true;
				loseCNT = 0;
				announcement.append("defeated!\r");
				DefendUserData* d = new DefendUserData();
				d->defenderFunc = Raffica::mainDefender;
				d->width = 3, d->height = 3;
				vector<int>* textures = new vector<int>();
				textures->push_back(LoadTexture("pic/main.bmp"));
				d->Init(4000 + width*height * 300, 3008, textures);
				d->attack = true;
				Raffica::createBall(10000, 3, 3, 0, 0, 0, d);

				client.sendMsgByHello(("get " + Raffica::drawTitleName + "\\win.txt").c_str());
				client.getFile("win.txt");
				int win, lose, trial;
				ifstream ifs("win.txt");
				ifs >> win >> lose >> trial;
				ifs.close();
				ofstream ofs("win.txt");
				ofs << win << ' ' << lose + 1 << ' ' << trial;
				ofs.close();
				client.sendMsgByHello(("send " + Raffica::drawTitleName + "\\win.txt").c_str());
				client.sendFile("win.txt");
			}
		}
		else loseCNT = 0;
	}
	Raffica::FrameInit();
	curTest->EnterFrame();

	settings.viewCenter.x -= (settings.viewCenter.x - Raffica::cameraTo) * 0.3f;
	Raffica::checkScreen();

	if (oldCenter.x != settings.viewCenter.x || oldCenter.y != settings.viewCenter.y) {
		Resize(width, height);
	}

	entry->name = const_cast<char*>(Raffica::drawTitleName.c_str());

	DebugDraw::selectFont(20, ANSI_CHARSET, "微软雅黑");
	curTest->DrawTitle(20, 20, entry->name);
	curTest->DrawTitle(200, 20, const_cast<char*>(Raffica::drawTitleName2.c_str()));
	//curTest->DrawTitle(5, 35, const_cast<char*>(Raffica::belong.c_str()));
	DebugDraw::selectFont(17, ANSI_CHARSET, "微软雅黑");
	curTest->DrawTitle(930, 500, const_cast<char*>(announcement.c_str()));
	curTest->DrawTitle(930, 650, const_cast<char*>(chattingText.c_str()));

	for (int i = 0; i < castleNames.size(); i++) {
		curTest->DrawTitle(400, 200 + i * 30, const_cast<char*>(castleNames[i].c_str()));
	}
	for (int i = 0; i < attackerNames.size(); i++) {
		curTest->DrawTitle(400, 200 + i * 30, const_cast<char*>(attackerNames[i].c_str()));
	}



	glutSwapBuffers();

	if (testSelection != testIndex)
	{
		testIndex = testSelection;
		delete curTest;
		entry = g_testEntries + testIndex;
		curTest = entry->createFcn();
		viewZoom = 4.1772f;
		settings.viewCenter.Set(0.0f, 20.0f);
		Resize(width, height);
	}
}

static void SimulationLoop() {
	
}

