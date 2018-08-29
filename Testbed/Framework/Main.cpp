/*
* Main.cpp
* Author: 不是Raffica
* 魔改: Raffica
*/


#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__) 
#endif

#define WindowTitle "Raffica"

#pragma warning(disable:4459)
#pragma warning(disable:4456)

#include <iostream>
using namespace std;

#include "FullscreenUI.h"
#include "Test.h"
#include "RaImport.h"
#include "RaServer.h"
#include "RaClient.h"
#include "glui/glui.h"
#include "RaRules.h"
#include "RaEffects.h"
#include "RaEasy.h"
#include "hyfEditor.h"
#include "HYFAttackEditor.h"
#include "RaEditor.h"
#include "RaWorld.h"
#include "RaPVP.h"
#include "Render.h"
#include "RaAttackEditor.h"
#include "RaCPUEditor.h"
#include "RaAction.h"


/////////////////////////////////////////////////////////////////////

string Raffica::drawTitleName = "";
string Raffica::drawTitleName2 = "";
string Raffica::belong = "";
Test* Raffica::test = NULL;
bool Raffica::Inited = false;
vector<BasicUserData*> Raffica::userdataList;
map<string, Material*> Raffica::materialList;
vector<Building*> Raffica::buildingList;
vector<RaButton*> Raffica::ButtonList;
int Raffica::effectCNT = 0;
float Raffica::cameraTo = 0;
bool Raffica::followCamera = true;
bool Raffica::groundInited = false;
const float Raffica::eps = 0.01f;
const float Raffica::LIQUID_DAMPING = .1f;
const float Raffica::LIQUID_RADIUS = .5f;
const float Raffica::LIQUID_DENSITY = 10;
int Raffica::PVPdeltaTime = 0;
bool Raffica::PVPisMouseDown = 0;
float Raffica::PVPMouseX = 0.0f, Raffica::PVPMouseY = 0.0f;
string Raffica::PVPKeyboard = "";
const float Raffica::normalDamping = 0.7f;
float Raffica::save_x1, Raffica::save_x2, Raffica::save_y1, Raffica::save_y2;
b2Vec2 RaEditor::mouse1, RaEditor::mouse2;
b2Body* RaEditor::copyTarget;
BreakableUserData* RaEditor::copyUserData;
b2Body* RaEditor::copiedBody;
vector<b2Body*> RaEditor::copyTargets;
vector<BreakableUserData*> RaEditor::copyUserDatas;
vector<b2Body*> RaEditor::copiedBodys;
RaButton* RaEditor::archerBody;
RaButton* RaEditor::saberBody;
RaButton* RaEditor::rocketguardBody;
RaButton* RaEditor::monitorBody;
RaButton* RaEditor::launcherBody;
RaButton* RaEditor::lampBody;
RaButton* RaEditor::capBody;
RaButton* RaEditor::repeaterBody;
RaButton* RaEditor::mainBody;
RaButton* RaEditor::engineBody;
RaButton* RaEditor::rocketattackBody;
RaButton* RaEditor::rengineBody;
RaButton* RaEditor::balancerBody;
RaButton* RaEditor::railgunBody;
int RaPVP::isPVPServer = 0;
int RaPVP::ServerPort = 0;
int RaPVP::serverAccepted = 0;
bool RaPVP::clientConnected = false;
string RaPVP::clientUser = "";
string RaPVP::ServerIP = "";
int Building::count = 0;
vector<RaServer> RaServerThread::servers;

///////////////////////////////////////////////////////////////////////////
extern int SERVER_PORT;
extern int SERVER_PORT2;
extern string SERVER_IP;
extern string username;
extern string password;
extern int isRegister;
extern RaClient client;
extern RaClient chatClient;
extern RaClient PVPClient;
extern thread* chatThread;
///////////////////////////////////////////////////////////////////////////

/////////////////////

// Fullscreen UI object.
FullscreenUI fullscreenUI;
// Used to control the behavior of particle tests.
ParticleParameter particleParameter;

// Set whether to restart the test on particle parameter changes.
// This parameter is re-enabled when the test changes.
void SetRestartOnParticleParameterChange(bool enable)
{
	particleParameter.SetRestartOnChange(enable);
}

// Set the currently selected particle parameter value.  This value must
// match one of the values in k_particleTypes or one of the values
// referenced by particleParameterDef passed to SetParticleParameters().
uint32 SetParticleParameterValue(uint32 value)
{
	const int32 index = particleParameter.FindIndexByValue(value);
	// If the particle type isn't found, so fallback to the first entry in the
	// parameter.
	particleParameter.Set(index >= 0 ? index : 0);
	return particleParameter.GetValue();
}

// Get the currently selected particle parameter value and enable particle
// parameter selection arrows on Android.
uint32 GetParticleParameterValue()
{
	// Enable display of particle type selection arrows.
	fullscreenUI.SetParticleParameterSelectionEnabled(true);
	return particleParameter.GetValue();
}

// Override the default particle parameters for the test.
void SetParticleParameters(
	const ParticleParameter::Definition * const particleParameterDef,
	const uint32 particleParameterDefCount)
{
	particleParameter.SetDefinition(particleParameterDef,
		particleParameterDefCount);
}
/////////////////////

void EditLevel_real() {
	castleNames.clear(); castleNames.shrink_to_fit();
	attackerNames.clear(); attackerNames.shrink_to_fit();

	delete entry;
	delete curTest;
	entry = new TestEntry();
	Raffica::drawTitleName = entry->name = "DefendEditor";
	entry->createFcn = RaDefendEditor::Create;

	curTest = entry->createFcn();
	((RaDefendEditor*)curTest)->init();
	Raffica::checkScreen();
}

void EditLevel() {
	curTest->type = "_defend";
	Raffica::analyseHave();
	Raffica::createHaveCastleButton();
}

void EditLevel(int) {
	static bool kick = 0;
	if(!kick) EditLevel(), kick = !kick;
	else Raffica::destroyHaveCastleButton(), kick = !kick;
}

void EditAttack_real() {
	castleNames.clear(); castleNames.shrink_to_fit();
	attackerNames.clear(); attackerNames.shrink_to_fit();

	delete entry;
	delete curTest;
	entry = new TestEntry();
	Raffica::drawTitleName = entry->name = "AttackEditor";
	entry->createFcn = RaAttackEditor::Create;

	curTest = entry->createFcn();
	((RaAttackEditor*)curTest)->init();
	Raffica::checkScreen();
}

void EditAttack() {
	curTest->type = "_attack";
	Raffica::analyseHave(true);
	Raffica::createHaveAttackerButton();
}

void EditAttack(int) {
	EditAttack();
}

void startPVP() {
	castleNames.clear(); castleNames.shrink_to_fit();
	attackerNames.clear(); attackerNames.shrink_to_fit();

	delete entry;
	delete curTest;
	entry = new TestEntry();
	entry->name = "PVP";
	entry->createFcn = RaPVP::Create;

	curTest = entry->createFcn();
	((RaPVP*)curTest)->init();
	Raffica::checkScreen();
}

void startPVP_real(string IP, int port) {
	castleNames.clear(); castleNames.shrink_to_fit();
	attackerNames.clear(); attackerNames.shrink_to_fit();

	delete entry;
	delete curTest;
	entry = new TestEntry();
	entry->name = "PVP";
	entry->createFcn = RaPVP::Create;

	curTest = entry->createFcn();
	((RaPVP*)curTest)->init();
	Raffica::checkScreen();

	RaPVP::isPVPServer = 2;
	RaPVP::ServerPort = port;
	RaPVP::ServerIP = IP;

	((RaPVP*)curTest)->serverUser = castleName;
}

void startPVP(int) {
	startPVP();
}

void Restart() {
	castleNames.clear(); castleNames.shrink_to_fit();
	attackerNames.clear(); attackerNames.shrink_to_fit();

	mainWinDetermined = false;
	castleNames.clear(); castleNames.shrink_to_fit();
	delete entry;
	delete curTest;
	entry = new TestEntry();
	entry->createFcn = RaWorld::Create;

	curTest = entry->createFcn();
	Raffica::checkScreen();
}

void Restart(int) {
	Restart();
}

void Pause(int) {
	settings.pause = !settings.pause;
}

void Exit(int code) {
	// TODO: freeglut is not building on OSX
#ifdef FREEGLUT
	glutLeaveMainLoop();
#endif
	exit(code);
}

void SingleStep(int) {
	settings.pause = 1;
	settings.singleStep = 1;
}

void chatFunc() {
	while (1) {
		cout << announcement << endl;
		string str = chatClient._recv();
		announcement.append(str);
	}
}

//void raf(int argc, char* argv[]);

void initGUI() {
	glEnable(GL_DEPTH_TEST);

	// Uncomment out the first block of code below, and then the second block,  
	//  to see how they affect line and point drawing.  

	// The following commands should cause points and line to be drawn larger  
	// than a single pixel width.  
	glPointSize(8);
	glLineWidth(5);



	// The following commands should induce OpenGL to create round points and   
	// antialias points and lines.  (This is implementation dependent unfortunately).  
	//RGBA mode antialias need cooperate with blend function.  
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST); // Make round points, not square points  
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);  // Antialias the lines  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(width, height);

	char title[32];
	sprintf(title, WindowTitle);
	mainWindow = glutCreateWindow(title);
	//glutSetOption (GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	GLUI_Master.set_glutReshapeFunc(Resize);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(KeyboardSpecial);
	GLUI_Master.set_glutMouseFunc(Mouse);
#ifdef FREEGLUT
	glutMouseWheelFunc(MouseWheel);
#endif
	glutMotionFunc(MouseMotion);

	glutPassiveMotionFunc(MousePassiveMotion);

	glutKeyboardUpFunc(KeyboardUp);

	glutSpecialUpFunc(KeyboardSpecialUp);
#ifdef RAFFICA_DEBUG
	glui = GLUI_Master.create_glui_subwindow(mainWindow,
		GLUI_SUBWINDOW_RIGHT);

	glui->add_statictext("Tests");
	GLUI_Listbox* testList =
		glui->add_listbox("", &testSelection);

	glui->add_separator();

	GLUI_Spinner* velocityIterationSpinner =
		glui->add_spinner("Vel Iters", GLUI_SPINNER_INT, &settings.velocityIterations);


	GLUI_Spinner* positionIterationSpinner =
		glui->add_spinner("Pos Iters", GLUI_SPINNER_INT, &settings.positionIterations);
	positionIterationSpinner->set_int_limits(0, 100);

	GLUI_Spinner* hertzSpinner =
		glui->add_spinner("Hertz", GLUI_SPINNER_FLOAT, &settingsHz);

	hertzSpinner->set_float_limits(5.0f, 200.0f);

	glui->add_button("Pause", 0, Pause);
	glui->add_button("Single Step", 0, SingleStep);
	glui->add_button("Restart", 0, Restart);
	glui->add_button("Quit", 0, (GLUI_Update_CB)Exit);
	glui->add_button("EditLevel", 0, EditLevel);
	glui->add_button("EditAttack", 0, EditAttack);
	glui->set_main_gfx_window(mainWindow);
#endif

	// Use a timer to control the frame rate.
	glutTimerFunc(framePeriod, Timer, 0);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);

	Raffica::getMaterialList();

	entry = new TestEntry();
	Raffica::drawTitleName = entry->name = "主界面";
	entry->createFcn = RaAttackEditor::Create;
	curTest = entry->createFcn();
	((RaAttackEditor*)curTest)->init();

	mainUseAttack = username;

	glutDisplayFunc(SimulationLoop);
}

void initNetwork() {
	cout << SERVER_IP << ':' << SERVER_PORT << endl;
	client.initSocket(SERVER_IP, SERVER_PORT);
	chatClient.initSocket(SERVER_IP, SERVER_PORT2);
	cout << username << ' ' << password << endl;
	if (!isRegister) {
		client.sendLogin(username, password);					//////註冊？
	}
	else {
		cout << "aaa";
		client.sendRegister(username, password);
	}

	//cout << client.sendMsgByHello(("get " + username + "\\attackinfo.txt").c_str());
	//client.getFile("attackinfo.txt", "");
	Raffica::analyseHave(2);
	chatThread = new thread(chatFunc);
}

void readParam() {
	freopen("param.txt", "r", stdin);
	cin >> isRegister >> SERVER_IP >> SERVER_PORT >> SERVER_PORT2 >> username >> password;
	freopen("CON", "r", stdin);
}

inline void EnableMemLeakCheck() {
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

int main(int argc, char** argv) {
	//raf(argc, argv);
	//return 0;
	cout << "開始運行\n";

	srand(time(NULL));

	EnableMemLeakCheck();
	
	if (argc == 1) {
		readParam();
	}
	cout << "readParam\n";

	testCount = 0;
	while (g_testEntries[testCount].createFcn != NULL) {
		++testCount;
	}

	glutInit(&argc, argv);

	cout << "glutInit\n";

	initNetwork();

	cout << "initNetwork\n";

	initGUI();

	cout << "initGUI\n";

	_CrtDumpMemoryLeaks();
	glutMainLoop();
	
	return 0;
}


/*
glui->add_checkbox("Warm Starting", &settings.enableWarmStarting);
glui->add_checkbox("Time of Impact", &settings.enableContinuous);
glui->add_checkbox("Sub-Stepping", &settings.enableSubStepping);

//glui->add_separator();

GLUI_Panel* drawPanel =	glui->add_panel("Draw");
glui->add_checkbox_to_panel(drawPanel, "Shapes", &settings.drawShapes);
glui->add_checkbox_to_panel(drawPanel, "Joints", &settings.drawJoints);
glui->add_checkbox_to_panel(drawPanel, "AABBs", &settings.drawAABBs);
glui->add_checkbox_to_panel(drawPanel, "Pairs", &settings.drawPairs);
glui->add_checkbox_to_panel(drawPanel, "Contact Points", &settings.drawContactPoints);
glui->add_checkbox_to_panel(drawPanel, "Contact Normals", &settings.drawContactNormals);
glui->add_checkbox_to_panel(drawPanel, "Contact Forces", &settings.drawContactForces);
glui->add_checkbox_to_panel(drawPanel, "Friction Forces", &settings.drawFrictionForces);
glui->add_checkbox_to_panel(drawPanel, "Center of Masses", &settings.drawCOMs);
glui->add_checkbox_to_panel(drawPanel, "Statistics", &settings.drawStats);
glui->add_checkbox_to_panel(drawPanel, "Profile", &settings.drawProfile);

int32 testCount = 2;

TestEntry* e = g_testEntries;
/*
while (e->createFcn)
{
testList->add_item(testCount, e->name);
++testCount;
++e;
}*/