/*
* AUTHOR:Raffica
* 这个头文件的定义是为了使box2d使用更方便
* 所涉及的坐标，(0, 0)为屏幕中心偏下，x轴正方向向右，y轴正方向向上
*/

#pragma once

#define maxUserSize 1000

#include "RaImport.h"
#include "RaClient.h"
#include "RaRules.h"
#include "RaEffects.h"
#include "Test.h"
#include "hyfEasy.h"


/*******************************************************/
/*******************************************************/
/************苟利国家生死以，岂因祸福避趋之*************/
/*******************************************************/
/*******************************************************/

using namespace std;

extern map<int, string> indexFile;
extern Settings settings;
extern float32 viewZoom;
extern int32 width;
extern int32 height;
extern RaClient client;
extern string SERVER_IP;
extern string username;
extern string password;
extern string mainUseAttack;
extern string chattingText;
extern string announcement;
extern string castleName;
extern int framePeriod;
extern vector<string> castleNames;
extern string attackerName;
extern vector<string> attackerNames;
extern RaClient chatClient;
extern bool mainWinDetermined;
extern Test* curTest;

extern TestEntry* entry;

void Resize(int32, int32);

b2Body* getBodyAtMouse(const b2Vec2& p, Test* test);

void EditLevel_real();
void EditAttack_real();
void startPVP_real(string, int);

class Raffica {

private:
	/*saveData*/
	//蠢比代码
	static float save_x1, save_x2, save_y1, save_y2;
	/*saveData*/

public:

	static const unsigned int PUBLIC_IDX =					1;
	static const unsigned int FRIEND_IDX =					2;
	static const unsigned int ENEMY_IDX =					4;
	static const unsigned int MID_IDX =						8;
	static const unsigned int FRIEND_ARROW_IDX =			16;
	static const unsigned int ENEMY_ARROW_IDX =				32;

	static const unsigned int COST_ARCHER =					10000;
	static const unsigned int COST_SABER =					10000;
	static const unsigned int COST_ROCKETDEFENDER =			30000;
	static const unsigned int COST_ROCKETATTACKER =			50000;
	static const unsigned int COST_ENGINE =					30000;
	static const unsigned int COST_BALANCER =				50000;
	static const unsigned int COST_MAIN =					50000;					//基地的价格
	static const unsigned int COST_RENGINE =				50000;
	static const unsigned int COST_RAILGUN =				250000;
	static const unsigned int COST_MONITOR =				50000;
	static const unsigned int COST_LAUNCHER =				100000;
	static const unsigned int COST_LAMP =					1000;
	static const unsigned int COST_CAP =					3000;
	static const unsigned int COST_REPEATER =				3000;

	static const unsigned int effectUpper =					300;

	static const int button_Xoffset = 50000;						//作为button的body的判定比正常body的位置远很多

	static const float LIQUID_RADIUS;
	static const float LIQUID_DAMPING;
	static const float LIQUID_DENSITY;

	static const float eps;


	static const int normalG = 0;					//重力
	static const float normalDamping;				//阻尼

	static Test* test;
	static bool Inited;
	static vector<BasicUserData*> userdataList;
	static vector<Building*> buildingList;
	static map<string, Material*> materialList;
	static vector<RaButton*> ButtonList;
	static int effectCNT;
	static float cameraTo;
	static bool followCamera;

	static bool groundInited;

	static string drawTitleName;
	static string drawTitleName2;
	static string belong;

	static int PVPdeltaTime;
	static bool PVPisMouseDown;
	static float PVPMouseX, PVPMouseY;
	static string PVPKeyboard;





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////防御函数///////////////////////////////////////////////////

	static void balancerWork() {
		float avry = 0;
		int cnt = 0;

		if (test->type != "mainworld") return;

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			DefendUserData* engine;
			if (body->GetUserData()) if (((BasicUserData*)body->GetUserData())->type == "defend") {
				engine = ((DefendUserData*)body->GetUserData());
				if (engine->defendType != "balancer") continue;
				if (body->GetJointList() == 0) continue;

				avry += body->GetPosition().y;
				cnt++;
			}
		}

		avry /= cnt;

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			DefendUserData* engine;
			if (body->GetUserData()) if (((BasicUserData*)body->GetUserData())->type == "defend") {
				engine = ((DefendUserData*)body->GetUserData());
				if (engine->defendType != "balancer") continue;
				if (body->GetJointList() == 0) continue;
				//////////////////////////////
				float delta = body->GetPosition().y - avry;
				float y = (delta < 0 ? 1 : -1) * sqrt(abs(delta)) * 500;
				body->ApplyLinearImpulse(b2Vec2(0, y), body->GetPosition(), true);
			}
		}
	}

	static void engineWork(float x, float y, bool PVPClient = false) {
		float sumx = 0;
		int cnt = 0;
		x *= 2;
		y *= 2;

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			DefendUserData* engine;
			if (body->GetUserData()) if (((BasicUserData*)body->GetUserData())->type == "defend") {
				engine = ((DefendUserData*)body->GetUserData());
				if((PVPClient == false && (body->GetFixtureList()->GetFilterData().categoryBits & FRIEND_IDX))
					|| (PVPClient == true && (body->GetFixtureList()->GetFilterData().categoryBits & ENEMY_IDX))) {
					continue;
				}
				if (engine->defendType != "engine" && engine->defendType != "rengine") continue;
				if (body->GetJointList() == 0) continue;
				if (engine->defendType == "engine") body->ApplyLinearImpulse(b2Vec2(x, 0), body->GetPosition(), true);
				else if (engine->defendType == "rengine") {
					body->ApplyLinearImpulse(b2Vec2(0, y), body->GetPosition(), true);
					vector<int>* tex = new vector<int>();
					tex->push_back(LoadTexture("pic/rj.png"));
					if(y > 0 && rand() > 26000) RaEffect::blow(body->GetPosition(), b2Vec2(0, -500), 200, tex, 2, 0.0f);
				}
				sumx += ((b2Body*)engine->target)->GetPosition().x;
				cnt ++;
			}
		}

		if(cnt && Raffica::followCamera) cameraTo = sumx / cnt + 10*viewZoom;
	}

	static b2Body* shootArrow(b2Body* origin, b2Vec2 to, float radius, unsigned int side = 0u) {
		b2Vec2 pos = origin->GetPosition();

		float x1 = pos.x, y1 = pos.y;
		float x2 = to.x, y2 = to.y;
		float angle = atan2(y2 - y1, x2 - x1);

		bool save;
		save = curTest->type == "PVP" ? true : false;

		b2Body* arrow;
		if (!side) arrow = Raffica::createBox(x1 + cos(angle)*(radius + 1), y1 + sin(angle)*(radius + 1), 1, 0.5f, 3.0f, 0.3f, 0.2f, 0, 1u, 65535u, save);
		else arrow = Raffica::createBox(x1 + cos(angle)*(radius + 1), y1 + sin(angle)*(radius + 1), 1, 0.5f, 3.0f, 0.3f, 0.2f, 0, side, 65535u - side, save);
		arrow->SetTransform(arrow->GetPosition(), angle);
		ExplosiveUserData* bud = new ExplosiveUserData(0);
		vector<int>* te = new vector<int>();
		te->push_back(LoadTexture("pic/material/steelstick.png"));
		bud->width = 2, bud->height = 1;
		bud->Init(9, 2, te);
		bud->sharp = 1000;

		arrow->SetLinearVelocity(b2Vec2(150 * cos(angle) + origin->GetLinearVelocity().x, 150 * sin(angle) + origin->GetLinearVelocity().y));
		arrow->SetBullet(true);

		Raffica::setUserData(arrow, bud);
		return arrow;
	}


	static b2Body* shootRocket(b2Body* origin, b2Vec2 to, float radius, unsigned int side = 0u) {
		b2Vec2 pos = origin->GetPosition();
		if (pos == to) return NULL;

		float x1 = pos.x, y1 = pos.y;
		float x2 = to.x, y2 = to.y;
		float angle = atan2(y2 - y1, x2 - x1);

		b2Body* arrow;

		bool save;
		save = curTest->type == "PVP" ? true : false;
		if (!side) arrow = Raffica::createBox(x1 + cos(angle)*(radius + 1), y1 + sin(angle)*(radius + 1), 2, 1, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535u - FRIEND_IDX, save);
		else arrow = Raffica::createBox(x1 + cos(angle)*(radius + 1), y1 + sin(angle)*(radius + 1), 2, 1, 3.0f, 0.3f, 0.2f, 0, ENEMY_IDX, 65535u - ENEMY_IDX, save);
		arrow->SetTransform(arrow->GetPosition(), angle);
		ExplosiveUserData* bud = new ExplosiveUserData(2000);
		vector<int>* te = new vector<int>();
		te->push_back(LoadTexture("pic/material/steelstick.png"));
		bud->width = 4, bud->height = 2;
		bud->Init(9, 20, te);
		bud->sharp = 1000;
		bud->feature = "rocket";

		arrow->SetLinearVelocity(b2Vec2(100 * cos(angle) + origin->GetLinearVelocity().x, 100 * sin(angle) + origin->GetLinearVelocity().y));
		arrow->SetBullet(true);

		Raffica::setUserData(arrow, bud);
		/*
		b2ParticleGroup* x = createParticle(b2Vec2(origin->GetPosition().x + cos(angle)*(radius + 3) - 2, origin->GetPosition().y + sin(angle) * (radius + 3) - 2), b2Vec2(origin->GetPosition().x + cos(angle) * (radius + 3) + 2, origin->GetPosition().y + sin(angle) * (radius + 3) + 2), 20, 100, 20, 20, 255);
		x->ApplyLinearImpulse(b2Vec2(800000 * cos(angle) + origin->GetLinearVelocity().x, 800000 * sin(angle) + origin->GetLinearVelocity().y));
		*/
		return arrow;
	}
	
	static void shootRailgun(b2Body* origin, b2Vec2 to, float radius, unsigned int side = 0u) {
		b2Vec2 pos = origin->GetPosition();

		float x1 = pos.x, y1 = pos.y;
		float x2 = to.x, y2 = to.y;
		float angle = atan2(y2 - y1, x2 - x1);

		b2Vec2 normalized = to-pos;
		float dis = normalized.Length();
		normalized.x *= 1/dis*60000;
		normalized.y *= 1/dis*60000;

		b2Body* target = queryLook(origin, normalized, side ? (FRIEND_IDX | PUBLIC_IDX) : (ENEMY_IDX | PUBLIC_IDX));
		if (!target) return;
		b2Vec2 _pos = queryLookPosition(origin, normalized, side ? (FRIEND_IDX | PUBLIC_IDX) : (ENEMY_IDX | PUBLIC_IDX));
		float len = (_pos - pos).Length();

		b2Body* effect = Raffica::createBox((pos.x + _pos.x) / 2, (pos.y + _pos.y) / 2, 100, 10, 3.0f, 0.3f, 0.2f, 0, 0x00, 0x00);
		ExplosiveEffectUserData* eud = new ExplosiveEffectUserData(64);
		vector<int>* tex = new vector<int>;
		tex->push_back(LoadTexture("pic/material/steelstick.png"));
		eud->Init(len, 5.0f, 64, 10, tex);
		setUserData(effect, eud);
		eud->widthReduce = false;
		effect->SetTransform(effect->GetPosition(), angle);

		target->ApplyLinearImpulse(normalized, _pos, true);
		BreakableUserData* bud = (BreakableUserData*)target->GetUserData();
		if (100000 > bud->defend) bud->hp -= 100000 - bud->defend;
		explosive(_pos, 3000);
		//cout << target->GetPosition().x << ' ' << target->GetPosition().y << endl;
	}

	static void aimWork(float x, float y, bool PVPClient = false) {
		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			DefendUserData* engine;
			if (body->GetUserData()) if (((BasicUserData*)body->GetUserData())->type == "defend") {
				if ((!PVPClient && (body->GetFixtureList()->GetFilterData().categoryBits & ENEMY_IDX)) ||
					(PVPClient && (body->GetFixtureList()->GetFilterData().categoryBits & FRIEND_IDX))) {
					engine = ((DefendUserData*)body->GetUserData());
					if (engine->defendType != "rocketattack" && engine->defendType != "railgun") continue;
					if (engine->skillCD != 200) continue;
					if (engine->defendType == "rocketattack")	shootRocket(body, b2Vec2(x, y), engine->width*1.414f, (!PVPClient) ? ENEMY_IDX : 0);
					if (engine->defendType == "railgun")		shootRailgun(body, b2Vec2(x, y), engine->width * 1.414f, (!PVPClient) ? ENEMY_IDX : 0);
					engine->skillCD = 0;
					break;
				}
			}
		}
	}


	static void archerDefender(DefendUserData* ud) {

		ud->defendType = "archer";
		
		BreakableUserData* enemy;
		b2Body* _this = (b2Body*)ud->target;
		int &cnt = ud->skillCD;
		float radius = max(ud->width, ud->height) / 2 * 1.41f;

		if (cnt >= 80) cnt = 79;
		cnt++;

		if (ud->attack) {
			for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
				if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
				else continue;
				if (enemy->attack) continue;
				b2Filter f = body->GetFixtureList()->GetFilterData();
				if (!(f.categoryBits & FRIEND_IDX)) continue;

				//if (enemy->type != "defend") continue;

				//if (!queryLook(_this, body)) return;

				if (cnt == 80 && rand() < 7000) {
					cnt = 0;

					shootArrow(_this, body->GetPosition(), radius, ENEMY_ARROW_IDX);

				}
			}
			return;
		}

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
			else continue;
			if (!enemy->attack) continue;
			//else if(queryLook(_this, body)) {
			{
				if (cnt == 80 && rand() < 7000) {
					cnt = 0;

					shootArrow(_this, body->GetPosition(), radius, FRIEND_ARROW_IDX);

				}
			}
		}
	}

	static void saberDefender(DefendUserData* ud) {
		ud->defendType = "saber";

		BreakableUserData* enemy;
		b2Body* _this = (b2Body*)ud->target;
		int &cnt = ud->skillCD;
		float radius = max(ud->width, ud->height) / 2 * 1.41f;
		bool enemyExist = false;

		if (cnt == 50) cnt--;
		cnt++;


		if (ud->attack) {
			for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {

				if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
				else continue;
				if (enemy->attack) continue;
				//if (enemy->type != "defend") continue;

				enemyExist = true;

				if (cnt == 50 && rand() < 7000) {

					ud->sharp = 30;
					ud->defend = 230000;

					cnt = 0;

					float x1 = _this->GetPosition().x, y1 = _this->GetPosition().y;
					float x2 = body->GetPosition().x, y2 = body->GetPosition().y;
					float angle = atan2(y2 - y1, x2 - x1);

					((b2Body*)ud->target)->SetLinearVelocity(b2Vec2(150 * cos(angle), 150 * sin(angle)));
				}
			}
			return;
		}

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
			else continue;
			if (!enemy->attack) continue;
			else if (queryLook(_this, body)) {
				enemyExist = true;

				if (cnt == 50 && rand() < 7000) {

					ud->sharp = 30;
					ud->defend = 230000;

					cnt = 0;

					float x1 = _this->GetPosition().x, y1 = _this->GetPosition().y;
					float x2 = body->GetPosition().x, y2 = body->GetPosition().y;
					float angle = atan2(y2 - y1, x2 - x1);

					((b2Body*)ud->target)->SetLinearVelocity(b2Vec2(30 * cos(angle), 30 * sin(angle)));
				}
			}
		}
		if (!enemyExist) ud->sharp = 1, ud->defend = 3002;
	}

	static void rocketguardDefender(DefendUserData* ud) {
		ud->defendType = "rocketguard";

		BreakableUserData* enemy;
		b2Body* _this = (b2Body*)ud->target;
		int &cnt = ud->skillCD;
		float radius = max(ud->width, ud->height) / 2 * 1.41f;

		if (cnt == 200) cnt--;
		cnt++;


		if (ud->attack) {
			for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {

				if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
				else continue;
				if (enemy->attack) continue;
				//if (enemy->type != "defend") continue;

				b2Filter f = body->GetFixtureList()->GetFilterData();
				if (!(f.categoryBits & FRIEND_IDX)) continue;

				if (cnt == 200 && rand() < 7000) {
					shootRocket(_this, body->GetPosition(), radius, ENEMY_ARROW_IDX);
					cnt = 0;
				}

			}
			return;
		}

		for (b2Body* body = test->m_world->GetBodyList(); body; body = body->GetNext()) {
			if (body->GetUserData() && ((BasicUserData*)body->GetUserData())->breakable) enemy = (BreakableUserData*)body->GetUserData();
			else continue;
			if (!enemy->attack) continue;
			//else if (queryLook(_this, body)) {
			{
				if (cnt == 200 && rand() < 7000) {
					cnt = 0;

					shootRocket(_this, body->GetPosition(), radius, FRIEND_ARROW_IDX);
				}
			}
		}
	}

	static void launcherDefender(DefendUserData* ud) {

		ud->defendType = "launcher";
		if (ud->RStrength > 0) {

			b2Body* b = (b2Body*)ud->target;
			int cnt = 0;
			for (auto i = b->GetJointList(); i;) {
				auto j = i->next;
				if (!i->joint->GetUserData()) continue;
				auto jud = (DistanceJointUserData*)i->joint->GetUserData();
				if (jud->feature == "NONE" || jud->feature == "none") {
					cnt++;
				}
				i = j;
			}
			if (cnt == 0) return;
			for (auto i = b->GetJointList(); i;) {

				auto j = i->next;
				if (!i->joint->GetUserData()) continue;
				auto jud = (DistanceJointUserData*)i->joint->GetUserData();

				if (jud->feature == "NONE" || jud->feature == "none") {

					b2Body* b2;
					if (i->joint->GetBodyA() == b) b2 = i->joint->GetBodyB();
					else b2 = i->joint->GetBodyA();
					b2Vec2 v = (b2->GetPosition() - b->GetPosition());
					v.x *= 1/v.Length()*ud->RStrength*20000 / cnt;
					v.y *= 1/v.Length()*ud->RStrength*20000 / cnt;
					b2->ApplyLinearImpulse(v, b2->GetPosition(), true);
					delete jud;
					curTest->m_world->DestroyJoint(i->joint);
				}
				i = j;
			}
		}
	}

	static void lampDefender(DefendUserData* ud) {
		ud->defendType = "lamp";
		if (ud->RStrength > 0) {
			(*ud->textureIndexs)[0] = LoadTexture("pic/lamp2.png");
		} else (*ud->textureIndexs)[0] = LoadTexture("pic/lamp.png");
	}

	static void capDefender(DefendUserData* ud) {
		ud->defendType = "cap";
	}

	static void repeaterDefender(DefendUserData* ud) {
		ud->defendType = "repeater";
	}

	static void monitorDefender(DefendUserData* ud) {
		ud->defendType = "monitor";
	}

	static void engineDefender(DefendUserData* ud) {
		ud->defendType = "engine";
	}

	static void rengineDefender(DefendUserData* ud) {
		ud->defendType = "rengine";
	}

	static void balancerDefender(DefendUserData* ud) {
		ud->defendType = "balancer";
	}

	static void mainDefender(DefendUserData* ud) {
		ud->defendType = "main";
		if(ud->attack) curTest->attackMainAlive++;
		else curTest->mainAlive++;
	}

	static void engineExplode(DefendUserData* ud) {
		explosive(((b2Body*)ud->target)->GetPosition(), 300);
	}

	static void rocketattackDefender(DefendUserData* ud) {
		ud->defendType = "rocketattack";
		if (ud->skillCD != 200) ud->skillCD++;
	}

	static void railgunDefender(DefendUserData* ud) {
		ud->defendType = "railgun";
		if (ud->skillCD != 200) ud->skillCD++;
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	/*                                          queryLook(b1, b2)                                                   */
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//功能：查询b1和b2之间能否互相看见

	static bool queryLook(b2Body* b1, b2Body* b2) {
		b2RayCastInput rci;
		rci.p1 = b1->GetPosition();
		rci.p2 = b2->GetPosition();
		rci.maxFraction = 1.0f;
		for (b2Body* i = test->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (i == b1 || i == b2) continue;
			b2RayCastOutput out;
			if(i->GetFixtureList()) if (i->GetFixtureList()->RayCast(&out, rci, 0)) {
				return false;
			}
		}
		return true;
	}

	static b2Body* queryLook(b2Body* b1, b2Vec2 eye, unsigned int target = FRIEND_IDX | PUBLIC_IDX) {
		b2RayCastInput rci;
		float dis = 100000.0f;
		b2Body* ret = 0;

		rci.p1 = b1->GetPosition();
		rci.p2 = b1->GetPosition() + eye;
		rci.maxFraction = 10.0f;

		for (b2Body* i = test->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (i == b1) continue;
			b2RayCastOutput out;
			if (i->GetFixtureList()) if (i->GetFixtureList()->RayCast(&out, rci, 0)) {
				float len = ((out.fraction * eye + b1->GetPosition()) - b1->GetPosition()).Length();
				if (dis > len && (i->GetFixtureList()->GetFilterData().categoryBits & target)) {
					dis = len;
					ret = i;
				}
			}
		}
		return ret;
	}

	static b2Vec2 queryLookPosition(b2Body* b1, b2Vec2 eye, unsigned int target = FRIEND_IDX | PUBLIC_IDX) {
		b2RayCastInput rci;
		float dis = 100000.0f;
		b2Vec2 ret;

		rci.p1 = b1->GetPosition();
		rci.p2 = b1->GetPosition() + eye;
		rci.maxFraction = 10.0f;

		for (b2Body* i = test->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (i == b1) continue;
			b2RayCastOutput out;
			if (i->GetFixtureList()) if (i->GetFixtureList()->RayCast(&out, rci, 0)) {
				float len = ((out.fraction * eye + b1->GetPosition()) - b1->GetPosition()).Length();
				if (dis > len && (i->GetFixtureList()->GetFilterData().categoryBits & target)) {
					dis = len;
					ret = out.fraction * eye + b1->GetPosition();
				}
			}
		}
		return ret;
	}

	static void queryAABB(vector<b2Body*>* target, const b2Vec2& l, const b2Vec2& u) {
		b2Vec2 low, up;
		low.x = min(l.x, u.x), low.y = min(l.y, u.y);
		up.x = max(l.x, u.x), up.y = max(l.y, u.y);
		for (b2Body* i = test->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (i->GetPosition().x > low.x && i->GetPosition().x < up.x && i->GetPosition().y > low.y &&  i->GetPosition().y < up.y && i->GetFixtureList() && i->GetFixtureList()->GetDensity() != 0) {
				target->push_back(i);
			}
		}
	}

	static void queryAABBJoint(vector<b2Joint*>* target, const b2Vec2& l, const b2Vec2& u) {
		b2Vec2 low, up;
		low.x = min(l.x, u.x), low.y = min(l.y, u.y);
		up.x = max(l.x, u.x), up.y = max(l.y, u.y);
		for (b2Joint* i = test->m_world->GetJointList(); i; i = i->GetNext()) {
			float x = (i->GetAnchorA().x+ i->GetAnchorB().x)/2;
			float y = (i->GetAnchorA().y + i->GetAnchorB().y)/2;
			if (x > low.x && x < up.x && y > low.y && y < up.y) {
				target->push_back(i);
			}
		}
	}

	static void explosive(b2Vec2 center, float power) {
		if ((curTest->type != "mainworld" && curTest->type != "PVP") && strcmp(entry->name, "主界面")) {
			return;
		}
		if (power <= 0.000001f) return;
		for (b2Body* i = test->m_world->GetBodyList(); i; i = i->GetNext()) {
			float deltax = (center.x - i->GetPosition().x);
			float deltay = (center.y - i->GetPosition().y);
			float distance = (center-i->GetPosition()).LengthSquared();
			if (distance < 0.01f) continue;
			i->ApplyLinearImpulse(-b2Vec2(deltax*power / distance, deltay*power / distance), i->GetPosition(), true);
			if (i->GetUserData()) if(((BasicUserData*)i->GetUserData())->breakable) {
				auto bud = ((BreakableUserData*)i->GetUserData());
				if (bud->type == "explosive") continue;
				float dmg = power*30/distance * min(i->GetMass()/i->GetFixtureList()->GetDensity(), power/10) - bud->defend;
				if(dmg > 0) ((BreakableUserData*)i->GetUserData())->hp -= dmg;
			}
		}
		vector<int>* tex = new vector<int>();
		vector<int>* tex2 = new vector<int>();
		tex->push_back(LoadTexture("pic/rj2.png"));
		tex->push_back(LoadTexture("pic/rj.png"));
		tex2->push_back(LoadTexture("pic/smoke2.png"));
		tex2->push_back(LoadTexture("pic/smoke.png"));
		RaEffect::explosive(center, power / 6, tex, (int)sqrt(power)/5);
		RaEffect::smoke(center, power / 6, tex2, (int)sqrt(power)/20);
		RaEffect::smoke(center, power / 6, tex, (int)sqrt(power) / 20);
	}
	
	static void virtualCopy(b2Body*& copyTarget, b2Body*& copiedBody, BreakableUserData*& copyUserData) {
		copyUserData = new BreakableUserData(*(BreakableUserData*)copyTarget->GetUserData());

		b2Filter f;
		f.categoryBits = 0, f.maskBits = 0;

		if(copyTarget->GetFixtureList()->GetShape()->GetType() == 0) {				//球
			copiedBody = Raffica::createBall(copyTarget->GetPosition().x, copyTarget->GetPosition().y,
			((UserData*)(copyTarget->GetUserData()))->width/2, copyTarget->GetFixtureList()->GetDensity(),
			copyTarget->GetFixtureList()->GetFriction(), copyTarget->GetFixtureList()->GetRestitution(), 0, 1U, 65535U, true);
			copiedBody->GetFixtureList()->SetFilterData(f);
		} else {																	//长方形
			copiedBody = Raffica::createBox(copyTarget->GetPosition().x, copyTarget->GetPosition().y,
			((UserData*)(copyTarget->GetUserData()))->width/2, ((UserData*)(copyTarget->GetUserData()))->height/2, copyTarget->GetFixtureList()->GetDensity(),
			copyTarget->GetFixtureList()->GetFriction(), copyTarget->GetFixtureList()->GetRestitution(), 0, 1U, 65535U, true);
			copiedBody->GetFixtureList()->SetFilterData(f);
			copiedBody->SetTransform(copiedBody->GetPosition(), copyTarget->GetAngle());
		}
		Raffica::setUserData(copiedBody, copyUserData);
		
	}

	static void handleChat(unsigned char key) {
		curTest->inputing = true;
		if (key != 8) {
			chattingText += key;					//8是backspace
		}
		else if (!chattingText.empty()) {
			vector<wchar_t> wstring;
			wstring.resize(chattingText.length() + 1);
			MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, chattingText.c_str(), -1, &wstring[0], chattingText.length());
			if (chattingText[chattingText.size() - 1] > 128 || chattingText[chattingText.size() - 1] < 0) chattingText.pop_back();
			chattingText.pop_back();
		}

		if (key == '\r') {							//回车
			chatClient._send((username + ": " + chattingText).c_str());
			chattingText.clear();
		}

		if (chattingText.size() == 41) chattingText.pop_back();		//一行不要超过40个字

	}


	static b2ParticleGroup* createParticle(b2Vec2 m1, b2Vec2 m2, float live = 0, 
		float r = 200, float g = 100, float b = 100, float alpha = 100) {

		b2ParticleGroupDef pd;
		b2PolygonShape shape;
		
		shape.SetAsBox(abs(m2.x - m1.x)/2, abs(m2.y - m1.y)/2);
		pd.shape = &shape;
		pd.flags = b2_viscousParticle;
		pd.lifetime = live;
		//pd.angle = -0.5f;
		//pd.angularVelocity = 2.0f;
		pd.position.Set((m1.x + m2.x) / 2, (m1.y + m2.y) / 2);
		//pd.color.Set(0, 255, 0, 255);
		pd.color.Set(r, g, b, alpha);
		//cout << "创建液体" << r << ' ' << g << ' ' << b << endl;
		//curTest->m_particleSystem->SetColorBuffer(&pd.color, 256);
		b2ParticleGroup* ret = curTest->m_particleSystem->CreateParticleGroup(pd);
		return ret;
	}

	static b2Body* createBox(float x, float y, float width, float height,
					float density = 3.0f, float friction = 0.3f, float rest = 0.2, BasicUserData* userdata = NULL,
					unsigned int side = PUBLIC_IDX, unsigned int maskbit = 0xFFFF, bool save = false) {
		if(!Inited) throw "unInited!";

		b2BodyDef def;
		b2FixtureDef fdef;
		b2PolygonShape sdef;

		def.position.Set(x, y);
		if(density) def.type = b2_dynamicBody; else def.type = b2_staticBody;
		fdef.density = density;
		fdef.friction = friction;
		fdef.restitution = rest;
		fdef.filter.categoryBits = side;
		fdef.filter.maskBits = maskbit;
		sdef.SetAsBox(width, height);

		fdef.shape = &sdef;
		def.linearDamping = normalDamping;
		def.angularDamping = normalDamping;
		b2Body *tmp = test->m_world->CreateBody(&def);
		tmp->CreateFixture(&fdef);
		if(userdata) setUserData(tmp, userdata);

		if(save) {
			Building* b = new Building();
			b->x = x, b->y = y, b->width = width, b->density = density, b->height = height, b->friction = friction;
			b->rest = rest, b->side = side, b->maskbit = maskbit;
			b->name = "box";
			b->body = tmp;
			b->userdata = (BasicUserData*)tmp->GetUserData();
			buildingList.push_back(b);
		}
	
		return tmp;
	}

	static b2Body* createBall(float x, float y, float radius,
					float density = 3.0f, float friction = 0.3f, float rest = 0.2, BasicUserData* userdata = NULL,
					unsigned int side = PUBLIC_IDX, unsigned int maskbit = 0xFFFF, bool save = false) {
		if(!Inited) throw "unInited!";

		b2BodyDef def;
		b2FixtureDef fdef;
		b2CircleShape sdef;

		def.position.Set(x, y);
		if(density) def.type = b2_dynamicBody; else def.type = b2_staticBody;
		fdef.density = density;
		fdef.friction = friction;
		fdef.restitution = rest;
		fdef.filter.categoryBits = side;
		fdef.filter.maskBits = maskbit;
		sdef.m_radius = radius;

		fdef.shape = &sdef;
		def.linearDamping = 0.3f;
		def.angularDamping = 0.3f;
		b2Body *tmp = test->m_world->CreateBody(&def);
		tmp->CreateFixture(&fdef);
		if(userdata) setUserData(tmp, userdata);
	
		if(save) {
			Building* b = new Building();
			b->x = x, b->y = y, b->radius = radius, b->density = density, b->friction = friction;
			b->rest = rest, b->side = side, b->maskbit = maskbit;
			b->name = "circle";
			b->body = tmp;
			b->userdata = (BasicUserData*)tmp->GetUserData();
			buildingList.push_back(b);
		}

		return tmp;
	}
	
	static b2Body* createPolygon(vector<b2Vec2>& pos, float density = 3.0f, 
								float friction = 0.3f, float rest = 0.2, BasicUserData* userdata = NULL, 
								unsigned int side = PUBLIC_IDX, unsigned int maskbit = 0xFFFF, bool save = false) {
		if(!Inited) throw "unInited!";

		b2BodyDef def;
		b2FixtureDef fdef;
		b2PolygonShape sdef;

		def.position.Set(0, 0);
		if(density) def.type = b2_dynamicBody; else def.type = b2_staticBody;
		fdef.density = density;
		fdef.friction = friction;
		fdef.restitution = rest;
		fdef.filter.categoryBits = side;
		fdef.filter.maskBits = maskbit;

		sdef.Set(&pos[0], (int32)pos.size());
		fdef.shape = &sdef;
		b2Body* tmp = test->m_world->CreateBody(&def);
		tmp->CreateFixture(&fdef);
		if(userdata) setUserData(tmp, userdata);
		
		if(save) {
			Building* b = new Building();
			b->density = density, b->friction = friction;
			b->rest = rest, b->side = side, b->maskbit = maskbit;
			b->vertex = pos;
			b->name = "polygon";
			b->body = tmp;
			b->userdata = (BasicUserData*)tmp->GetUserData();
			buildingList.push_back(b);
		}

		return tmp;
	}

	static RaButton* createButton(float x, float y, float width, float height, vector<string> tex, string name) {
		b2Body* body = createBox(x, y, width, height, 0, 0, 0, 0, 0, 65535);
		RaButton* button = new RaButton(body, name, tex);
		unsigned int t = LoadTexture(tex[0].c_str());
		UserData* ud = new UserData();
		ud->name = name;
		ud->isButton = true;
		ud->button = button;
		ud->buttonX = x, ud->buttonY = y;
		button->body = body;
		button->name = name;
		button->originHeight = height;
		button->originWidth = width;
		ButtonList.push_back(button);

		Raffica::setUserDataByTexture(body, ud, t, width*2, height*2);
		return button;
	}

	static b2Joint* createRotJoint(b2Body* body1, b2Body* body2, b2Vec2 anc, float lowerBound = INF, float upperBound = INF, bool collide = true, bool save = false) {
		b2RevoluteJointDef def;
		def.Initialize(body1, body2, anc);
		def.collideConnected = collide;
		if(lowerBound < 10000 && upperBound < 10000) def.enableLimit = true;
		def.upperAngle = upperBound;
		def.lowerAngle = lowerBound;
		b2Joint* res = test->m_world->CreateJoint(&def);

		if(save) {
			Building* b = new Building();
			b->body1 = body1, b->body2 = body2;
			b->anc1 = anc;
			b->name = "rotjoint";
			b->joint = res;
			b->userdata = (BasicUserData*)res->GetUserData();
			buildingList.push_back(b);
		}

		return res;
	}

	static b2Joint* createDisJoint(b2Body* body1, b2Body* body2, b2Vec2 anc1, b2Vec2 anc2, bool collide = true, bool save = false) {
		b2DistanceJointDef def;
		def.Initialize(body1, body2, anc1, anc2);
		def.collideConnected = collide;
		b2Joint* res = test->m_world->CreateJoint(&def);
		
		if(save) {
			Building* b = new Building();
			b->body1 = body1, b->body2 = body2;
			b->anc1 = anc1;
			b->anc2 = anc2;
			b->name = "disjoint";
			b->joint = res;
			b->userdata = (BasicUserData*)res->GetUserData();
			buildingList.push_back(b);
		}

		return res;
	}

	static void setUserData(b2Joint* joint, DistanceJointUserData* userdata) {
		userdata->target = joint;
		joint->SetUserData(userdata);
		userdataList.push_back(userdata);
	}

	static void setUserData(b2Body* body, BasicUserData* userdata) {
		body->SetUserData(userdata);
		userdata->target = body;
		userdataList.push_back(userdata);
	}

	static void setUserDataByTexture(b2Body* body, UserData* userdata, int ID, float width, float height) {
		userdata->Init(width, height);
		setUserData(body, userdata);
		userdata->textureIndex = ID;
	}

	//最后一个参数是面积

	static void setMaterial(b2Body* body, string material, float width, float height, float size, bool ball = false) {
		Material* m = materialList[material];
		BreakableUserData* bud = new BreakableUserData();
		float hp = m->hp * size / 12;
		bud->textureIndexs->assign(m->textures->begin(), m->textures->end());
		bud->Init(hp, m->defend, m->textures);
		bud->enableTile = m->enableTile;
		bud->tileX = m->width, bud->tileY = m->height;
		bud->width = width, bud->height = height;
		bud->ball = ball;
		if (material == "spine") {
			bud->feature = "spine";
			bud->sharp = 10;
		}
		if (material == "raffica") {
			bud->feature = "raffica";
		}
		if (material == "tnt") {
			bud->feature = "tnt";
		}
		setUserData(body, bud);
	}

	static b2Body* createBoxByMaterial(b2Vec2 ld, b2Vec2 ru, string material) {
		b2Vec2 mouse1 = ld;
		b2Vec2 mouse2 = ru;

		Material* m;
		m = Raffica::materialList[material];
		float width, height;
		width = abs(mouse1.x - mouse2.x), height = abs(mouse1.y - mouse2.y);
		b2Body* body = Raffica::createBox((mouse1.x + mouse2.x) / 2, (mouse1.y + mouse2.y) / 2, width / 2, height / 2, m->density, m->friction, m->rest, 0, 1, 65535, true);
		Raffica::setMaterial(body, material, width, height, width*height);

		return body;
	}

	static b2Body* createBallByMaterial(b2Vec2 center, b2Vec2 border, string material) {
		b2Vec2 mouse1 = center;
		b2Vec2 mouse2 = border;

		Material* m;
		m = Raffica::materialList[material];
		float r = sqrt((mouse1.x - mouse2.x) * (mouse1.x - mouse2.x) + (mouse1.y - mouse2.y) * (mouse1.y - mouse2.y));
		b2Body* body = Raffica::createBall(mouse1.x, mouse1.y, r, m->density, m->friction, m->rest, 0, 1, 65535, true);
		Raffica::setMaterial(body, material, (float)(r*2.0f), (float)(r*2.0f), (float)(r*r*3.14), true);
		
		return body;
	}

	static b2Joint* createDisJointByAnc(b2Vec2 anc1, b2Vec2 anc2, float width, string name) {
		b2Body* copiedBody = getBodyAtMouse(anc1, test);
		b2Body* copyTarget = getBodyAtMouse(anc2, test);

		if (copiedBody == 0 || copyTarget == 0) return 0;
		if (copiedBody == copyTarget) return 0;

		b2Joint* j = Raffica::createDisJoint(copiedBody, copyTarget, anc1, anc2, true, true);

		DistanceJointUserData* dud = new DistanceJointUserData();
		dud->width = width;
		dud->textureIndex = LoadTexture(name.c_str());
		Raffica::setUserData(j, dud);

		return j;
	}

	static void getMaterialList() {
		string name, url;
		float x, y, hp, defend, density, rest, friction;

		freopen("materials.txt", "r", stdin);
		cin.clear();
		cin.seekg(0, ios::beg);

		while(cin >> name >> density >> friction >> rest >> x >> y >> hp >> defend) {
			Material* m = new Material();
			m->width = x, m->height = y, m->hp = hp, m->defend = defend, m->density = density, m->friction = friction, m->rest = rest;
			if (abs(x) < 0.0001f || abs(y) < 0.0001f) m->enableTile = false;
			else m->enableTile = true;
			m->textures = new vector<int>(); m->name = name;
			while(1) {
				cin >> url;
				if(url != "end") {
					m->textures->push_back(LoadTexture(url.c_str()));
					m->urls.push_back(url); 
				} else break;
			}
			cin >> m->buttonUpURL >> m->buttonDownURL;
			materialList[name] = m;
			cout << name << endl;
		}

		freopen("CON", "r", stdin);
		cin.clear();
		cin.seekg(0, ios::beg);
	}

	static RaCar* createCar(float _x, float _y, 
						  float frontW = 3, float frontH = 5.2,
						  float backW = 12.5, float backH = 0.25,
						  float tailW = 1, float tailH = 2.5,
						  float wheelR = 1.6, int side = MID_IDX, bool pos = false) {
		RaCar *res = new RaCar();
		res->Init(frontW, frontH, backW, backH, tailW, tailH, wheelR, side, pos);
		res->front = createBox(_x + (pos?0:1) * backW + (pos ? frontW/2 : -frontW/2), _y - frontH/2, frontW/2, frontH/2, 6, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->back = createBox(_x + backW/2, _y - frontH - backH/2, backW/2, backH/2, 6, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->wheelLeft = createBall(_x + wheelR, _y - frontH - backH - wheelR, wheelR, 6, 1.5f, 0.4f, NULL, side, 0xFFFF - 16384 - side);
		res->wheelRight = createBall(_x - wheelR + backW, _y - frontH - backH - wheelR, wheelR, 6, 1.5f, 0.4f, NULL, side, 0xFFFF - 16384 - side);
		res->wheelHoldLeft = createBox(_x + wheelR, _y - frontH - backH - wheelR, 0.7f, 0.7f, 13, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->wheelHoldRight = createBox(_x + backW - wheelR, _y - frontH - backH - wheelR, 0.7f, 0.7f, 13, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->tail = createBox(_x + (pos ? 1 : 0) * backW + (pos ? -tailW / 2 : tailW / 2), _y - frontH + tailH / 2, tailW/2, tailH/2, 6, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		createRotJoint(res->back, res->wheelHoldLeft, b2Vec2(_x + wheelR, _y - backH - frontH), 0, 0);
		createRotJoint(res->back, res->wheelHoldRight, b2Vec2(_x + backW - wheelR, _y - backH - frontH), 0, 0);
		createRotJoint(res->wheelHoldLeft, res->wheelLeft, b2Vec2(_x + wheelR, _y - backH - frontH - wheelR));
		createRotJoint(res->wheelHoldRight, res->wheelRight, b2Vec2(_x + backW - wheelR, _y - backH - frontH - wheelR));
		createRotJoint(res->back, res->front, b2Vec2(_x + (pos ? 0 : 1) * backW + (pos ? frontW / 2 : -frontW / 2), _y - frontH), 0, 0, false);
		createRotJoint(res->back, res->tail, b2Vec2(_x + (pos ? 1 : 0) * backW + (pos ? -tailW / 2 : tailW / 2), _y - frontH), 0, 0, false);
		res->wheelR = wheelR;
		res->frontWidth = frontW;
		res->frontHeight = frontH;
		res->position = pos;
		res->side = side;
		res->tailWidth = tailW;
		res->tailHeight = tailH;
		res->backHeight = backH;
		res->backWidth = backW;
		return res;
	}

	static RaHuman* createHuman(float _x, float _y, 
								 float headR = 1.0f, float bodyW = 1.5f, float bodyH = 3.0f,
								 float armW = 0.6f, float armH = 3.6f, 
								 float legW = 0.6f, float legH = 3.6f, 
								 unsigned int side = FRIEND_IDX) {
		RaHuman* res = new RaHuman();
		res->head = createBall(_x + bodyW / 2, _y - headR, headR, 3, 0.3f, 0.3f, NULL, side, 0xFFFF - 16384 - side);
		res->body = createBox(_x + bodyW / 2, _y - headR * 2 - bodyH / 2, bodyW/2, bodyH/2, 3.0f, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		createRotJoint(res->body, res->head, b2Vec2(_x + bodyW / 2, _y - headR * 2), -3.14f / 6, 3.14f / 6);
		res->l_arm1 = createBox(_x - armW / 2 + bodyW / 2, _y - headR * 2 - armH / 4, armW/2, armH/4, 1, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->l_arm2 = createBox(_x - armW / 2 + bodyW / 2, _y - headR * 2 - armH / 4 * 3, armW/2, armH/4, 1, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->r_arm1 = createBox(_x + bodyW / 2 + armW / 2, _y - headR * 2 - armH / 4, armW/2, armH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->r_arm2 = createBox(_x + bodyW / 2 + armW / 2, _y - headR * 2 - armH / 4 * 3, armW/2, armH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->l_leg1 = createBox(_x + bodyW / 2 + legW / 2, _y - headR * 2 - bodyH - legH / 4, legW/2, legH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->l_leg2 = createBox(_x + bodyW / 2 + legW / 2, _y - headR * 2 - bodyH - legH / 4 * 3, legW/2, legH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->r_leg1 = createBox(_x + bodyW / 2 - legW / 2, _y - headR * 2 - bodyH - legH / 4, legW/2, legH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->r_leg2 = createBox(_x + bodyW / 2 - legW / 2, _y - headR * 2 - bodyH - legH / 4 * 3, legW/2, legH/4, 3, 0.3f, 0.2f, NULL, side, 0xFFFF - 16384 - side);
		res->l_arm_joint1 = createRotJoint(res->body, res->l_arm1, b2Vec2(_x - armW / 2 + bodyW / 2, _y - headR * 2));
		res->l_arm_joint2 = createRotJoint(res->l_arm2, res->l_arm1, b2Vec2(_x - armW / 2 + bodyW / 2, _y - headR * 2 - armH / 2));
		res->r_arm_joint1 = createRotJoint(res->body, res->r_arm1, b2Vec2(_x + armW / 2 + bodyW / 2, _y - headR * 2));
		res->r_arm_joint2 = createRotJoint(res->r_arm2, res->r_arm1, b2Vec2(_x + armW / 2 + bodyW / 2, _y - headR * 2 - armH / 2));
		createRotJoint(res->body, res->l_leg1, b2Vec2(_x + legW / 2 + bodyW / 2, _y - headR * 2 - bodyH), -3.14f / 3 * 2, 3.14f / 3 * 2);
		createRotJoint(res->l_leg2, res->l_leg1, b2Vec2(_x + legW / 2 + bodyW / 2, _y - headR * 2 - bodyH - legH / 2), -3.14f / 3 * 2, 3.14f / 3 * 2);
		createRotJoint(res->body, res->r_leg1, b2Vec2(_x - legW / 2 + bodyW / 2, _y - headR * 2 - bodyH), -3.14f / 3 * 2, 3.14f / 3 * 2);
		createRotJoint(res->r_leg2, res->r_leg1, b2Vec2(_x - legW / 2 + bodyW / 2, _y - headR * 2 - bodyH - legH / 2), -3.14f / 3 * 2, 3.14f / 3 * 2);
		res->headR = headR;
		res->bodyW = bodyW;
		res->bodyH = bodyH;
		res->legH = legH;
		res->legW = legW;
		res->armH = armH;
		res->armW = armW;
		res->side = side;
		return res;
	}

	static void makeWall(float width, float height, float x = 100) {
		b2Body* tmp = Raffica::createBox(-width/2-x, height, 30, height+300, 0, 3, .3f, NULL, 1, 65535);
		BreakableUserData* gud = new BreakableUserData();
		vector<int>* gtex = new vector<int>();
		gtex->push_back(LoadTexture("pic/ground.bmp"));
		gud->Init(100000000, 10000000, gtex);
		setUserDataByTexture(tmp, gud, 1, 60, height*2 + 600);
		b2Body* tmp2 = Raffica::createBox(width / 2, height, 30, height+300, 0, 3, .3f, NULL, 1, 65535);
		BreakableUserData* gud2 = new BreakableUserData();
		vector<int>* gtex2 = new vector<int>();
		gtex2->push_back(LoadTexture("pic/ground.bmp"));
		gud2->Init(100000000, 10000000, gtex2);
		setUserDataByTexture(tmp2, gud2, 1, 60, height*2 + 600);
		b2Body* tmp3 = Raffica::createBox(0, height*2, width*3, 30, 0, 3, .3f, NULL, 1, 65535);
		BreakableUserData* gud3 = new BreakableUserData();
		vector<int>* gtex3 = new vector<int>();
		gtex3->push_back(LoadTexture("pic/ground.bmp"));
		gud3->Init(100000000, 10000000, gtex3);
		setUserDataByTexture(tmp3, gud3, 1, width*3, 60);
	}

	static void checkScreen() {
		//if (settings.viewCenter.x > 0) settings.viewCenter.x = min(settings.viewCenter.x, test->rightBorder - 30.0f*viewZoom);
		//else 
			//settings.viewCenter.x = max(settings.viewCenter.x, test->leftBorder + 30.0f*viewZoom);
		if (cameraTo > 50) cameraTo = min(cameraTo, test->rightBorder - 30.0f*viewZoom);
		else cameraTo = max(cameraTo, test->leftBorder + 30.0f*viewZoom);
		Resize(width, height);
	}

	static double calculateCost(double mass, double defend, double hp, string feature = "none") {
		if (feature == "spine") return (mass * defend + 0.5*hp) / 10000;
		if (feature == "raffica") return (mass * defend + 0.5*hp) / 30000;
		if (defend == 3001) return Raffica::COST_ARCHER;
		if (defend == 3002) return Raffica::COST_SABER;
		if (defend == 3003) return Raffica::COST_ROCKETDEFENDER;
		if (defend == 3004) return Raffica::COST_ENGINE;
		if (defend == 3005) return Raffica::COST_RENGINE;
		if (defend == 3006) return Raffica::COST_ROCKETATTACKER;
		if (defend == 3007) return Raffica::COST_BALANCER;
		if (defend == 3008) return Raffica::COST_MAIN;
		if (defend == 3009) return Raffica::COST_RAILGUN;
		if (defend == 30010)return Raffica::COST_MONITOR;
		if (defend == 3011) return Raffica::COST_LAUNCHER;
		if (defend == 3012) return Raffica::COST_LAMP;
		if (defend == 3013) return Raffica::COST_CAP;
		if (defend == 3014) return Raffica::COST_REPEATER;
		return (mass * defend + 0.5*hp) / 40000;
	}


	/*
	* createHaveAttackerButton()
	*
	* 分析完战车后要创建一些按钮来表示我选择了哪个战车
	*/

	static void attackerButtonUp(const b2Vec2& p, string* mode, const RaButton* button) {			//这个显然是个临时函数

		attackerName = attackerNames[button->name.c_str()[0]];
		EditAttack_real();

		ifstream ifs("have.txt");
		ifs.seekg(0);
		string name;
		string win, lose;
		vector<string> s;
		castleNames.clear();
		castleNames.shrink_to_fit();

		while (1) {
			ifs >> name;
			s.push_back(name);
			if (name == "RaEnd") break;
		}
		while (1) {
			ifs >> name;
			s.push_back(name);
			if (name == "RaEnd") break;
		}
		ifs >> name;
		ifs.close();

		ofstream ofs("have.txt");
		for (int i = 0; i < s.size(); i++) ofs << s[i] << endl;

		ofs << attackerName;
		ofs.close();

		client.sendMsgByHello(("send " + username + ".txt").c_str());
		client.sendFile("have.txt");

		mainUseAttack = attackerName;
	}

	/*
	* attackerRegisterUp()
	*/
	/*
	static void attackerRegisterButtonUp(const b2Vec2& p, string* mode, const RaButton* button) {			//这个显然是个临时函数

		attackerName = attackerNames[button->name.c_str()[0]];
		ifstream ifs("have.txt");
		ifs.seekg(0);
		string name;
		string win, lose;
		vector<string> s;
		castleNames.clear();
		castleNames.shrink_to_fit();

		while (1) {
			ifs >> name;
			s.push_back(name);
			if (name == "RaEnd") break;
		}
		while (1) {
			ifs >> name;
			s.push_back(name);
			if (name == "RaEnd") break;
		}
		ifs >> name;
		ifs.close();

		ofstream _ofs;
		_ofs.open("write.txt");
		_ofs << username; _ofs.close();
		client.sendMsgByHello(("send " + username + "\\attackinfo.txt").c_str());
		client.sendFile("write.txt");

		string newCastleName = (username + to_string(s.size()));
		s.push_back(newCastleName);

		_ofs.open("write.txt");
		_ofs << username; _ofs.close();
		client.sendMsgByHello(("send " + newCastleName + "\\info.txt").c_str());
		client.sendFile("write.txt");

		_ofs.open("write.txt");
		_ofs << 0; _ofs.close();
		client.sendMsgByHello(("send " + newCastleName + "\\PVP.txt").c_str());
		client.sendFile("write.txt");

		_ofs.open("write.txt");
		_ofs << "0 0 0"; _ofs.close();
		client.sendMsgByHello(("send " + newCastleName + "\\win.txt").c_str());
		client.sendFile("write.txt");

		ofstream ofs("have.txt");
		for (int i = 0; i < s.size(); i++) ofs << s[i] << endl;

		ofs << attackerName;
		ofs.close();

		client.sendMsgByHello(("send " + username + ".txt").c_str());
		client.sendFile("have.txt");

		mainUseAttack = attackerName;

	}*/

	static void createHaveAttackerButton(bool kick = false) {			//kick似乎是废弃的设定
		mainWinDetermined = true;
		static vector<RaButton*> attackerButtons;

		b2Body* j;

		for (b2Body* i = curTest->m_world->GetBodyList(); i;) {
			j = i->GetNext();
			delete (UserData*)i->GetUserData();
			i = j;
		}

		ButtonList.clear(); ButtonList.shrink_to_fit();

		vector<string> name;
		name.push_back("pic/rj.bmp");
		name.push_back("pic/rj2.bmp");

		for (int i = 0; i < attackerNames.size(); i++) {

			string tmp;
			tmp.push_back((char)i);

			RaButton* b = createButton(-16, 11 - i * 2, 1.5f, 0.7f, name, tmp);
			attackerButtons.push_back(b);
			b->setPressUp(attackerButtonUp);
			((BreakableUserData*)b->body->GetUserData())->tip = ("选择攻城车" + attackerNames[i]);
		}
		//RaButton* b = createButton(-22, 11, 1.5f, 0.7f, name, "register");
		//b->setPressUp(attackerRegisterButtonUp);
	}

	static void destroyHaveAttackerButton() {
		analyseHave(true);
		createHaveAttackerButton();
	}

	/*
	* createHaveCastleButton()
	*
	* 分析完地图后要创建一些按钮来表示我选择了哪个地图
	*/

	static void castleButtonUp(const b2Vec2& p, string* mode, const RaButton* button) {			//这个显然是个临时函数

		castleName = castleNames[button->name.c_str()[0]];
		EditLevel_real();

	}

	static void createHaveCastleButton(bool kick = false) {			//kick似乎是废弃的设定

		mainWinDetermined = true;
		static vector<RaButton*> castleButtons;

		b2Body* j;
		
		for (b2Body* i = curTest->m_world->GetBodyList(); i;) {
			j = i->GetNext();
			delete (UserData*)i->GetUserData();
			i = j;
		}
		ButtonList.clear(); ButtonList.shrink_to_fit();
		for (int i = 0; i < castleNames.size(); i++) {
			vector<string> name;
			name.push_back("pic/rj.bmp");
			name.push_back("pic/rj2.bmp");

			string tmp;
			tmp.push_back((char)i);

			RaButton* b = createButton(-16, 11 - i*2, 1.5f, 0.7f, name, tmp);
			castleButtons.push_back(b);
			b->setPressUp(castleButtonUp);
		}
	}

	static void destroyHaveCastleButton() {
		analyseHave();
		createHaveCastleButton();
	}

	/*
	* analyseHave()
	*
	* 分析玩家有哪些地图
	*/

	static void analyseHave(int flag = 0) {			//false时是castle true时是attacker
		client.sendMsgByHello(("get " + username + ".txt").c_str());
		client.getFile("have.txt", "");
		ifstream ifs("have.txt");
		ifs.seekg(0);
		string name;
		string win, lose;
		castleNames.clear();
		castleNames.shrink_to_fit();
		while (1) {
			ifs >> name;
			if (name == "RaEnd") break;
			if(flag == 0) castleNames.push_back(name);
		}
		while (1) {
			ifs >> name;
			if (name == "RaEnd") break;
			if (flag == 1) attackerNames.push_back(name);
		}
		ifs >> name;
		mainUseAttack = name;
		cout << "正在分析拥有的城堡" << name << "\n";
	}

	/*
	* analyseHave()
	*
	* 分析玩家有哪些地图
	*/

	static void analyseUser(int flag = 0) {			//false时是castle true时是attacker
		client.sendMsgByHello("getDir 1");
		client.sendMsgByHello("get o");
		client.getFile("o.txt");
		ifstream fin("o.txt");

		castleNames.clear();
		castleNames.shrink_to_fit();
		string user;

		while (!fin.eof()) {
			user = "";
			fin >> user;
			if (user == "") break;
			if (user == username) continue;
			castleNames.push_back(user);
			cout << "用戶名：" << user << endl;
		}
	}
	/*
	* createHaveCastleButton()
	*
	* 分析完地图后要创建一些按钮来表示我选择了哪个地图
	*/

	static void PVPUserButtonUp(const b2Vec2& p, string* mode, const RaButton* button) {			//这个显然是个临时函数
		castleName = castleNames[button->name.c_str()[0]];

		int flag = 0;
		string IP;
		client.sendMsgByHello(("get " + castleName + "\\PVP.txt").c_str());
		client.getFile("PVP.txt");
		ifstream ifs("PVP.txt");
		ifs >> flag;
		
		if (flag) {
			int port;
			ifs >> port >> IP;
			startPVP_real(IP, port);
		}

		ifs.close();

	}

	static void createPVPUserButton(bool kick = false) {			//kick似乎是废弃的设定

		mainWinDetermined = true;
		static vector<RaButton*> castleButtons;

		b2Body* j;

		for (b2Body* i = curTest->m_world->GetBodyList(); i;) {
			j = i->GetNext();
			delete (UserData*)i->GetUserData();
			i = j;
		}
		ButtonList.clear(); ButtonList.shrink_to_fit();
		for (int i = 0; i < castleNames.size(); i++) {
			vector<string> name;
			name.push_back("pic/rj.bmp");
			name.push_back("pic/rj2.bmp");

			string tmp;
			tmp.push_back((char)i);

			RaButton* b = createButton(-16, 11 - i * 2, 1.5f, 0.7f, name, tmp);
			castleButtons.push_back(b);
			b->setPressUp(PVPUserButtonUp);
		}
	}

	/*
	*  matchWorld()
	* 匹配一個玩家作爲info.txt
	*/

	static void matchWorld() {
		client.sendMsgByHello("getDir 1");
		client.sendMsgByHello("get o");
		client.getFile("o.txt");
		ifstream fin("o.txt");
		int i = 0;
		string user[maxUserSize];

		while (!fin.eof()) {
			fin >> user[i];
			i++;
		}
		
		int index = rand() * (i - 1) / 32767;

		drawTitleName = entry->name = const_cast<char*>(user[index].c_str());

		while (drawTitleName == username) {
			index = rand() * (i - 1) / 32767;

			drawTitleName = entry->name = const_cast<char*>(user[index].c_str());
		}

		cout << drawTitleName << ' ' << i << ' ' << index << ' ' << "来自最上层的指令" << endl;
		/////////////////////
		//获取匹配到玩家的胜负 尝试数

		cout << client.sendMsgByHello(("get " + user[index] + "\\win.txt").c_str());
		client.getFile("win.txt");
		int win, lose, trial;
		ifstream ifs("win.txt");
		ifs >> win >> lose >> trial;
		ifs.close();

		ofstream ofs("win.txt");
		ofs << win << ' ' << lose << ' ' << trial + 1;
		ofs.close();
		cout << client.sendMsgByHello(("send " + user[index] + "\\win.txt").c_str());
		client.sendFile("win.txt");

		stringstream ss;
		ss << "win:" << win << ",lose:" << lose << ",trial:" << trial;
		ss >> drawTitleName2;

		/////////////////////

		cout << client.sendMsgByHello(("get " + user[index] + "\\info.txt").c_str());
		client.getFile("info.txt");
	}

	/*
	* b2Body* findLastBody(b2Vec2 pos)
	* 在PVP中发现lastposition为pos的Body
	* 如果没有的话返回0
	* 如果要创造新物体的话返回1
	*/
	
	static b2Body* findLastBody(b2Vec2 pos) {
		//cout << "findLastBody() " << pos.x << ' ' << pos.y << endl;
		if (pos.x == -1000 && pos.y == -1000) return (b2Body*)1;
		for (b2Body* b = curTest->m_world->GetBodyList(); b; b = b->GetNext()) {
			auto ud = (BreakableUserData*)b->GetUserData();
			if (!ud) continue;
			if (fabs(ud->lastPosition.x-pos.x) < eps && fabs(ud->lastPosition.y - pos.y) < eps) {
				//cout << "找到了" << pos.x << ' ' << pos.y << "\n";
				return b;
			}
		}
		//cout << "正在找" << pos.x << ' ' << pos.y << endl;
		return 0;
	}

	static int loadPVP_func(bool PVP, b2Body*& lastBody, ifstream& fin) {				//1 创建刚体 0 寻找失败 2 改变刚体位置
		if (!PVP) return 1;
		float _x1, _y1, _x2, _y2, angle;
		fin >> _x1 >> _y1 >> _x2 >> _y2 >> angle;
		//cout << _x1 << ' ' << _y1 << ' ' << _x2 << ' ' << _y2 << ' ' << angle << endl;
		save_x1 = _x1, save_x2 = _x2, save_y1 = _y1, save_y2 = _y2;
		b2Body* x = findLastBody(b2Vec2(_x1, _y1));
		lastBody = x;
		if (x == 0) {
			cout << "需要创建的物体! " << _x2 << ' ' << _y2 << endl;
			return 1;
		}
		if ((int)x == 1) {
			cout << "需要创建的物体! " << _x2 << ' ' << _y2 << endl;
			return 1;
		}
		((BreakableUserData*)x->GetUserData())->lastPosition.x = _x2;
		((BreakableUserData*)x->GetUserData())->lastPosition.y = _y2;
		//cout << "找到了! " << _x2 << ' ' << _y2 << endl;
		x->SetLinearVelocity(b2Vec2((_x2 - _x1) * (1000/(double)framePeriod) / PVPdeltaTime, (_y2 - _y1) * 60 / PVPdeltaTime));
		((BreakableUserData*)x->GetUserData())->PVPdeleted = false;
		//cout << PVPdeltaTime << endl;
		x->SetTransform(b2Vec2(_x2, _y2), angle);
		return 2;
	}

	/*
	* 读取info.txt里面的配置来生成世界
	* PVP中，如果lastposition是(-1000, -1000)，说明需要加载这个body
	*/

	static void loadWorld(Test* test, string filename, bool attack = false, float defendDeltax = 0, bool PVP = false) {
		string type, userdataType, url;
		string feature;
		float hp, defend, width, tileX, tileY;
		BreakableUserData* bud;
		DefendUserData* ud;
		b2Body* lastBody = (b2Body*)0xABCDEF;		//pvp, default:0xABCDEF
		float pa[100];								//pa stands for params

		float attackDeltax = 150;

		double cost = 0;

		b2World*& m_world = test->m_world;

		m_world->SetGravity(b2Vec2(0.0f, (float)Raffica::normalG));
		ifstream fin(filename.c_str());
		fin.clear();
		fin.seekg(0, ios::beg);

		if(PVP) for (auto x = m_world->GetBodyList(); x; x = x->GetNext()) {
			if(x->GetUserData()) ((BreakableUserData*)x->GetUserData())->PVPdeleted = true;
		}

		//box 0 0 40 2 0 3 0.3 1 65535 0
		//	breakable 100000 1e+06 2 3 pic / ground.bmp end
		if (!groundInited) {
			b2Body* tmp = Raffica::createBox(0, -60, 1500, 22, 0, 3, .3f, NULL, 1, 65535);
			BreakableUserData* gud = new BreakableUserData();
			vector<int>* gtex = new vector<int>();
			gtex->push_back(LoadTexture("pic/nightground.png"));
			gud->Init(100000000, 10000000, gtex);
			setUserDataByTexture(tmp, gud, 1, 3000, 44);					//地板
			groundInited = true;
		}

		fin >> type;
		if(!attack) belong = type;										//这个城堡或攻城车是谁的

		while(fin >> type) {

			lastBody = (b2Body*)0xABCDEF;

			b2Body* tmp = 0;
			b2Joint* tmp2 = 0;
			
			if (type == "wa") {											//water
				float x, y;
				fin >> x >> y;
				createParticle(b2Vec2(x - attack * attackDeltax, y), b2Vec2(x - attack * attackDeltax + .5f, y + .5f), 0);
			}

			if(type == "static") {														//背景Userdata
				float x, y, width, height, angle;
				fin >> x >> y >> width >> height >> angle;
				fin >> url;
				StaticUserData* sud = new StaticUserData();
				sud->Init(width, height, x, y, 0);
				sud->setTexture(LoadTexture(url.c_str()));
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if(type == "box") {													//箱子
				for (int i = 0; i < 10; i++) fin >> pa[i];
				if (attack) pa[0] -= attackDeltax;
				else pa[0] += defendDeltax;
				tmp = Raffica::createBox(pa[0], pa[1], pa[2], pa[3], pa[4], pa[5], pa[6], NULL, (unsigned int)pa[7], (unsigned int)pa[8]);
				//x     y      width  height densityfric   rest
				tmp->SetTransform(tmp->GetPosition(), pa[9]);
				bud = new BreakableUserData();

				fin >> userdataType;

				if (userdataType == "none") continue;
				if (userdataType == "breakable") {
					fin >> hp >> defend >> tileX >> tileY;
					vector<int>* urls = new vector<int>();
					while (fin >> url) {
						if (url != "end") {
							int tmp = LoadTexture(url.c_str());
							urls->push_back(tmp);
						}
						else break;
					}
					bud->Init(hp, defend, urls);
					bud->tileX = tileX, bud->tileY = tileY;
					if (tileX == 0 || tileY == 0) bud->enableTile = false;
					else bud->enableTile = true;
					Raffica::setUserDataByTexture(tmp, bud, 1, pa[2] * 2, pa[3] * 2);
					bud->attack = attack;
				}

				fin >> feature;
				lastBody = tmp;

				int ret = loadPVP_func(PVP, lastBody, fin);
				if (ret == 0 || ret == 2) {
					delete bud;
					bud = 0; tmp = 0;
					((BreakableUserData*)lastBody->GetUserData())->hp = hp;
				}
				else {
					if ((int)lastBody == 1) lastBody = tmp;
					Building* build = new Building();
					build->x = pa[0], build->y = pa[1], build->width = pa[2], build->height = pa[3],
					build->density = pa[4], build->friction = pa[5], build->rest = pa[6],
					build->side = (unsigned int)pa[7], build->maskbit = (unsigned int)pa[8];
					build->name = "box";
					build->body = tmp;
					build->userdata = bud;
					buildingList.push_back(build);

					cost = calculateCost(tmp->GetMass(), bud->defend, bud->maxhp);
				}
			}
			
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if(type == "polygon") {														//多边形，可能要废弃的设定
				int n;
				float tmpx, tmpy;
				float density, friction, rest;
				unsigned int side, mask;
				vector<b2Vec2> pos;
				if (1) {
					fin >> density >> friction >> rest >> side >> mask;
					fin >> n;

					assert(n >= 3);
					for (int i = 0; i < n; i++) {
						fin >> tmpx >> tmpy;
						pos.push_back(*new b2Vec2(tmpx, tmpy));
						if (attack) tmpx -= attackDeltax;
						else tmpx += defendDeltax;
					}

					tmp = Raffica::createPolygon(pos, density, friction, rest, 0, side, mask);
					bud = new BreakableUserData();

					fin >> userdataType;
					if (userdataType == "breakable") {
						fin >> hp >> defend >> tileX >> tileY;
						bud->polygon = true;
						for (int i = 0; i < n; i++) bud->vertexs.push_back(pos[i]);
						vector<int>* urls = new vector<int>();
						while (fin >> url) {
							if (url != "end") {
								urls->push_back(LoadTexture(url.c_str()));
							}
							else break;
						}
						bud->Init(hp, defend, urls);
						bud->tileX = tileX, bud->tileY = tileY;
						if (tileX == 0 || tileY == 0) bud->enableTile = false;
						else bud->enableTile = true;
						Raffica::setUserDataByTexture(tmp, bud, 1, 0, 0);
						bud->attack = attack;
					}
					fin >> feature;
					lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete bud;
						bud = 0; tmp = 0;
					}
					else {
						if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
						build->x = tmpx, build->y = tmpy,
							build->density = density, build->friction = friction, build->rest = rest,
							build->side = side, build->maskbit = mask;
						build->vertex.assign(pos.begin(), pos.end());
						buildingList.push_back(build);
						build->body = tmp;
						build->userdata = bud;
						build->name = "polygon";

						cost = calculateCost(tmp->GetMass(), bud->defend, bud->maxhp);
					}
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			else if(type == "circle") {																		//球体
				float x, y, density, friction, rest, radius;
				unsigned int side, mask;
				fin >> x >> y >> radius;
				fin >> density >> friction >> rest >> side >> mask;
				fin >> userdataType;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;

				if (1) {
					tmp = Raffica::createBall(x, y, radius, density, friction, rest, 0, side, mask);
					bud = new BreakableUserData();
					bud->ball = true;
					bud->enableTile = true;

					if (userdataType == "breakable") {
						fin >> hp >> defend >> tileX >> tileY;
						vector<int> *urls = new vector<int>();
						while (fin >> url) {
							if (url != "end") {
								urls->push_back(LoadTexture(url.c_str()));
							}
							else break;
						}
						bud->Init(hp, defend, urls);
						bud->tileX = tileX, bud->tileY = tileY;
						if (tileX == 0 || tileY == 0) bud->enableTile = false;
						else bud->enableTile = true;
						Raffica::setUserDataByTexture(tmp, bud, 1, radius * 2, radius * 2);
						bud->attack = attack;
					}

					fin >> feature;
					lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete bud;
						bud = 0; tmp = 0;
						((BreakableUserData*)lastBody->GetUserData())->hp = hp;
					}
					else {
						if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
						build->x = x, build->y = y, build->radius = radius;
						build->density = density, build->friction = friction, build->rest = rest,
							build->side = side, build->maskbit = mask, build->body = tmp;
						build->name = "circle";
						build->userdata = bud;
						buildingList.push_back(build);

						cost = calculateCost(tmp->GetMass(), bud->defend, bud->maxhp);
					}
				}
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			else if(type == "disjoint") {																//距离关节
				int body1, body2, collide;
				BasicUserData *bd1, *bd2;
				float x1, y1, x2, y2;
				b2Vec2 anc1, anc2;
				fin >> body1 >> body2 >> x1 >> y1 >> x2 >> y2 >> collide;
				if (attack) x1 -= attackDeltax;
				else x1 += defendDeltax;
				if (attack) x2 -= attackDeltax;
				else x2 += defendDeltax;
				anc1 = b2Vec2(x1, y1);
				anc2 = b2Vec2(x2, y2);

				if (getBodyAtMouse(anc1, test) == NULL || getBodyAtMouse(anc2, test) == NULL) {
					cout << "读取关节信息时可能出错\n";
					continue;
				}

				bd1 = (BasicUserData*)(getBodyAtMouse(anc1, test)->GetUserData());
				bd2 = (BasicUserData*)(getBodyAtMouse(anc2, test)->GetUserData());

				if (bd1 == bd2) {
					cout << "读取关节信息时可能出错\n";
					continue;
				}
				
				b2DistanceJoint* joint = (b2DistanceJoint*)Raffica::createDisJoint((b2Body*)bd1->target, (b2Body*)bd2->target, anc1, anc2, collide?true:false);
				DistanceJointUserData* djud = new DistanceJointUserData();
				tmp2 = joint;

				fin >> userdataType;
				
				if(userdataType == "none") continue;
				else if(userdataType == "disjoint") {
					fin >> width >> url;
					djud->textureIndex = LoadTexture(url.c_str());
					djud->width = width;
					Raffica::setUserData(joint, djud);
				}
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				build->anc1 = b2Vec2(x1, y1), build->anc2 = b2Vec2(x2, y2);
				build->body1 = (b2Body*)bd1->target, build->body2 = (b2Body*)bd2->target;
				build->collide = collide?true:false;
				build->joint = joint;
				build->name = "disjoint";
				build->userdata = djud;
				buildingList.push_back(build);
				
				cost = 0;
			}
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			////////**************************************防御类Body****************************************************////////
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//****************************************************************************************************************//
			////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if(type == "archer") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = archerDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/archer.png"));

					ud->Init(4000 + width*height * 300, 3001, textures);
					ud->attack = attack;
					ud->skillCD = 100;
					Raffica::setUserData(tmp, ud);

					fin >> feature;
					lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "archer";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3001, 0);
					}
				}
			}
			if (type == "saber") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = saberDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/saber.bmp"));

					ud->Init(4000 + width*height * 300, 3002, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 50;

					fin >> feature;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "saber";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3002, 0);
					}
				}
			}
			if (type == "rocketguard") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = rocketguardDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/rocketguard.bmp"));

					ud->Init(4000 + width*height * 300, 3003, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "rocketguard";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3003, 0);
					}
				}
			}

			if (type == "monitor") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = monitorDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/monitor.bmp"));

					ud->Init(400000 + width*height * 300, 30010, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "monitor";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 30010, 0);
					}
				}
			}

			if (type == "launcher") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = launcherDefender;				///
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/launcher.bmp"));

					ud->Init(4300 + width*height * 300, 3011, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "launcher";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3011, 0);
					}
				}
			}

			if (type == "lamp") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = lampDefender;				///
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/lamp.png"));

					ud->Init(4300 + width*height * 300, 3012, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "lamp";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3012, 0);
					}
				}
			}

			if (type == "cap") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = capDefender;				///
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/racap.bmp"));

					ud->Init(4300 + width*height * 300, 3013, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "cap";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3013, 0);
					}
				}
			}

			if (type == "repeater") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = repeaterDefender;				///
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/repeater.bmp"));

					ud->Init(4300 + width*height * 300, 3014, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "repeater";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3014, 0);
					}
				}
			}

			if (type == "engine") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = engineDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/engine.bmp"));

					ud->Init(4000 + width*height * 300, 3004, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->destroyDefenderFunc = engineExplode;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "engine";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3004, 0);
					}
				}
			}
			if (type == "rengine") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = rengineDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/rengine.bmp"));

					ud->Init(4000 + width*height * 300, 3005, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->destroyDefenderFunc = engineExplode;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "rengine";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3005, 0);
					}
				}
			}
			if (type == "rocketattack") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = rocketattackDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/rocketattack.bmp"));

					ud->Init(4000 + width*height * 300, 3006, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "rocketattack";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3006, 0);
					}
				}
			}

			if (type == "balancer") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = balancerDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/balancer.bmp"));

					ud->Init(4000 + width*height * 300, 3007, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "balancer";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3007, 0);
					}
				}
			}


			if (type == "main") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = mainDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/main.bmp"));

					ud->Init(4000 + width*height * 300, 3008, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "main";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3008, 0);
					}
				}
			}

			if (type == "railgun") {
				float x, y, width, height;
				fin >> x >> y >> width >> height;
				if (attack) x -= attackDeltax;
				else x += defendDeltax;
				if ((int)lastBody == 1) lastBody = tmp; Building* build = new Building();
				if (1) {
					tmp = Raffica::createBox(x, y, width, height, 3.0f, 0.3f, 0.2f, 0, FRIEND_IDX, 65535 - FRIEND_ARROW_IDX);
					ud = new DefendUserData();
					ud->defenderFunc = railgunDefender;
					ud->width = width * 2, ud->height = height * 2;

					vector<int>* textures = new vector<int>();
					textures->push_back(LoadTexture("pic/misaka.png"));

					ud->Init(4000 + width*height * 300, 3009, textures);
					Raffica::setUserData(tmp, ud);
					ud->attack = attack;
					ud->skillCD = 200;

					fin >> feature; lastBody = tmp;
					int ret = loadPVP_func(PVP, lastBody, fin);
					if (ret == 0 || ret == 2) {
						delete ud;
						ud = 0; tmp = 0;
					}
					else {
						build->body = tmp;
						build->userdata = ud;
						build->name = "railgun";

						Raffica::buildingList.push_back(build);

						cost = calculateCost(0, 3009, 0);
					}
				}
			}

			if (feature == "none" || feature == "NONE") {

			}
			else if (feature == "spine") {
				if(tmp) ((UserData*)tmp->GetUserData())->sharp = 10;
				cost *= 4;
			}
			if (tmp) if(tmp->GetUserData())
			((UserData*)tmp->GetUserData())->feature = feature;
			if (tmp2) if (tmp2->GetUserData()) {
				((UserData*)tmp2->GetUserData())->feature = feature;
			}

			if (!PVP) {
				if (attack) {
					if (!tmp && type != "disjoint") {
						cout << "在读取" << filename << "时可能出现问题" << "\n";
						continue;
					}
					if (type == "disjoint") continue;
					b2Filter f;
					f.categoryBits = ENEMY_IDX;
					f.maskBits = 65535u - ENEMY_ARROW_IDX;
					tmp->GetFixtureList()->SetFilterData(f);
				}
				else {
					if (!tmp && type != "disjoint") {
						cout << "在读取" << filename << "时可能出现问题" << "\n";
						continue;
					}
					if (type == "disjoint") continue;
					b2Filter f;
					f.categoryBits = FRIEND_IDX;
					f.maskBits = 65535u - FRIEND_ARROW_IDX;
					tmp->GetFixtureList()->SetFilterData(f);
				}
			}
			else {
				if (type == "disjoint") continue;
				unsigned int side1, side2;
				fin >> side1 >> side2;
				b2Filter f;
				f.categoryBits = side1;
				f.maskBits = side2;
				if (lastBody) lastBody->GetFixtureList()->SetFilterData(f);
			}
			if (lastBody && lastBody != (b2Body*)0xABCDEF) {
				((BreakableUserData*)lastBody->GetUserData())->lastPosition = lastBody->GetPosition();
				((BreakableUserData*)lastBody->GetUserData())->PVPdeleted = false;
			}
			test->totalCost += cost;

			if (tmp) {
				((BreakableUserData*)tmp->GetUserData())->lastPosition.x = save_x2;
				((BreakableUserData*)tmp->GetUserData())->lastPosition.y = save_y2;
			}


		}

		if(PVP) for (auto x = m_world->GetBodyList(); x; x = x->GetNext()) {
			if(x->GetUserData() && ((BasicUserData*)x->GetUserData())->breakable == true) {
				auto _x = ((BreakableUserData*)x->GetUserData());
				if (_x->PVPdeleted && _x->type != "explosiveeffect" && _x->defend < 10000000 && _x->button == 0) {
					_x->hp = -10;
					if (_x->feature == "rocket") {
						if (_x->defend > 4) {
							//cout << "火箭大爆炸！！！" << ((b2Body*)this->target)->GetPosition().x << ' ' << ((b2Body*)this->target)->GetPosition().y << "\n";
							Raffica::explosive(((b2Body*)_x->target)->GetPosition(), 2000);
						}
					}
					//_x->Display();
				}
			}
		}

		//cout << "loadWorld Finished\n";

		fin.close();

	}

	static void saveUserData(BasicUserData* ud, ofstream& ofs) {
		if(ud->type == BasicUserData::BREAKABLE) {
			ofs << "breakable ";
			BreakableUserData* bud = (BreakableUserData*)ud;
			ofs << bud->hp << ' ' << bud->defend << ' ' << ud->tileX << ' ' << ud->tileY << ' ';
			for (unsigned int j = 0; j < bud->textureIndexs->size(); j++) ofs << indexFile[(*bud->textureIndexs)[j]] << ' ';
			ofs << "end" << endl;
		} else if(ud->type == BasicUserData::DISTANCEJOINT) {
			DistanceJointUserData* dud = (DistanceJointUserData*)ud;
			ofs << "disjoint " << dud->width << ' ' << indexFile[dud->textureIndex] << endl;
		}
		else if (ud->type == BasicUserData::EXPLOSIVE) {
			ofs << "breakable ";
			BreakableUserData* bud = (BreakableUserData*)ud;
			ofs << bud->hp << ' ' << bud->defend << ' ' << ud->tileX << ' ' << ud->tileY << ' ';
			for (unsigned int j = 0; j < bud->textureIndexs->size(); j++) ofs << indexFile[(*bud->textureIndexs)[j]] << ' ';
			ofs << "end" << endl;
		}
		ofs << ud->feature << endl;
	}
	/*
	static void saveWorld() {			//废弃的功能
		freopen("info.txt", "w", stdout);
		cout.clear();
		cout.seekp(0, ios::beg);
		cout << "static 0 20 80 80 0 pic/sky.bmp\n";				//背景
		for(unsigned int i = 0;i < buildingList.size();i ++) {
			Building* b = buildingList[i];
			BasicUserData* ud = 0;
			if(b->name == "disjoint") {
				ud = (DistanceJointUserData*)b->joint->GetUserData();
			} else ud = (BasicUserData*)(b->body->GetUserData());
			if(b->name == "box") {
				cout << "box " << b->x << ' ' << b->y << ' ' << b->width << ' ' << b->height << ' ' << 
				b->density << ' ' << b->friction << ' ' << b->rest << ' ' << b->side << ' ' << b->maskbit << " 0" << '\n';
				saveUserData(ud, ofs);
				
			} else if(b->name == "circle") {
				cout << "circle " << b->x << ' ' << b->y << ' ' << b->radius << ' ' << b->density << ' '
					<< b->friction << ' ' << b->rest << ' ' << b->side << ' ' << b->maskbit << endl;
				saveUserData(ud, ofs);
			} else if(b->name == "polygon") {
				cout << "polygon " << b->density << ' ' << b->friction << ' ' << b->rest 
					<< ' ' << b->side << ' ' << b->maskbit << ' ' << b->vertex.size() << ' ';
				for(unsigned int j = 0;j < b->vertex.size();j ++) cout << b->vertex[j].x << ' ' << b->vertex[j].y << ' ';
				saveUserData(ud, ofs);
			}
		}
		freopen("CON", "w", stdout);
	}
	*/

	/*
	* savenowWorld(string filename, string _username = username, bool PVP = false)
	* 保存当前世界 filename 文件名 _username 城堡名或攻城车名 PVP 是否是PVP
	* 如果是PVP世界的话，需要多输出lastposition
	*/
	static void savenowWorld(string filename, string _username = username, bool PVP = false) {

		
		if (curTest->mainAlive == 0 && !PVP) {
			announcement.append("基地个数应该大于0！\n\r");
			return;
		}

		//freopen(filename.c_str(), "w", stdout);
		ofstream ofs(filename.c_str());
		//freopen("CON", "w", stdout);
		
		//cout << "static 0 20 80 80 0 pic/sky.bmp\n";				//背景

		ofs << username << "\n";
		//cout << "buildingList.size() = " << buildingList.size() << endl;

		for(unsigned int i = 0;i < buildingList.size();i ++) {
			Building* b = buildingList[i];
			BasicUserData* ud = 0;

			if (b->name != "disjoint" && !b->body) {
				continue;
			}

			if(b->name == "archer") {
				ofs << "archer " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width/2 << ' ' << ((UserData*)b->body->GetUserData())->height/2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "saber") {
				ofs << "saber " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "rocketguard") {
				ofs << "rocketguard " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "monitor") {
				ofs << "monitor " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "launcher") {
				ofs << "launcher " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "lamp") {
				ofs << "lamp " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "cap") {
				ofs << "cap " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "repeater") {
				ofs << "repeater " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "engine") {
				ofs << "engine " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "rengine") {
				ofs << "rengine " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "rocketattack") {
				ofs << "rocketattack " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "balancer") {
				ofs << "balancer " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "railgun") {
				ofs << "railgun " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "main") {
				ofs << "main " << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << ' '
					<< ((UserData*)b->body->GetUserData())->width / 2 << ' ' << ((UserData*)b->body->GetUserData())->height / 2 << ' ' << ((UserData*)b->body->GetUserData())->feature << endl;
			}
			else if (b->name == "disjoint" && !PVP) {
				ofs << "disjoint " << 0 << ' ' << 0
					<< ' ' << b->joint->GetAnchorA().x << ' ' << b->joint->GetAnchorA().y << ' ' << b->joint->GetAnchorB().x << ' ' << b->joint->GetAnchorB().y <<  ' ' << true << endl;
				if(b->joint->GetUserData()) {
					DistanceJointUserData* dud = (DistanceJointUserData*)b->joint->GetUserData();
					saveUserData(dud, ofs);
				} else ofs << "none\n";
				continue;
				
			} else if(b->name != "disjoint") {
				ud = (BasicUserData*)(b->body->GetUserData());
			}
			if(b->name == "box") {
				if(!ud) continue;
				b2Body* body = (b2Body*)(ud->target);
				//if(body->GetPosition().y < -2000) continue;
				ofs << "box " << body->GetPosition().x << ' ' << body->GetPosition().y << ' ' << b->width << ' ' << b->height << ' ' << 
				b->density << ' ' << b->friction << ' ' << b->rest << ' ' << b->side << ' ' << b->maskbit << ' ' << body->GetAngle() << '\n';
				saveUserData(ud, ofs);
				
			} else if(b->name == "circle") {
				if(!ud) continue;
				b2Body* body = (b2Body*)(ud->target);
				//if(body->GetPosition().y < -2000) continue;
				ofs << "circle " << body->GetPosition().x << ' ' << body->GetPosition().y << ' ' << b->radius << ' ' << b->density << ' '
					<< b->friction << ' ' << b->rest << ' ' << b->side << ' ' << b->maskbit << endl;
				saveUserData(ud, ofs);
			} else if(b->name == "polygon") {
				ofs << "polygon " << b->density << ' ' << b->friction << ' ' << b->rest 
					<< ' ' << b->side << ' ' << b->maskbit << ' ' << b->vertex.size() << ' ';
				for(unsigned int j = 0;j < b->vertex.size();j ++) ofs << b->vertex[j].x << ' ' << b->vertex[j].y << ' ';
				saveUserData(ud, ofs);
			}
			if (PVP) {
				if (b->name == "disjoint") {
					continue;
				}
				if (!b->body) continue;
				BreakableUserData *ud = (BreakableUserData*)b->body->GetUserData();
				if (!ud) continue;
				ofs << ud->lastPosition.x << ' ' << ud->lastPosition.y << endl;
				ofs << b->body->GetPosition().x << ' ' << b->body->GetPosition().y << endl;

				ud->lastPosition = b->body->GetPosition();

				ofs << b->body->GetAngle() << endl;
				ofs << b->body->GetFixtureList()->GetFilterData().categoryBits << ' ' <<
					b->body->GetFixtureList()->GetFilterData().maskBits << endl;
			}

		}

		for (auto i = curTest->m_particleSystem->GetParticleGroupList(); i; i = i->GetNext()) {
			ofs << "wa " << i->GetPosition().x << ' ' << i->GetPosition().y << endl;
		}

		char* r = client.sendMsgByHello(("send " + _username + "\\" + filename).c_str());
		//cout << "send " + _username + "\\" + filename << endl;
		client.sendFile(filename.c_str());

	}

	//////////////////////////////////////////////////////////////////
	//////////////////////FrameInit每帧更新///////////////////////////
	//////////////////////////////////////////////////////////////////

	static void RafficaInit(Test* t) {
		Raffica::test = t;
		Raffica::Inited = true;
	}

	static void FrameInit() {
		cout << mainUseAttack << endl;
		b2Body* mouseBody = getBodyAtMouse(curTest->mouse, curTest);

		//在屏幕上显示鼠标坐标
		curTest->DrawTitle(330, 40, const_cast<char*>(("x:" + to_string(curTest->mouse.x) + ", y:" + to_string(curTest->mouse.y)).c_str()), 100);

		if (mouseBody) {
			auto userdata = (BasicUserData*)mouseBody->GetUserData();
			userdata->updateTip();
			curTest->DrawTitle(330, 20, const_cast<char*>(userdata->tip.c_str()), 100);
		}
		for (auto i = curTest->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (!i) continue;
			BreakableUserData* bud = (BreakableUserData*)i->GetUserData();
			if (!bud) continue;
			if (bud->feature == "rocket") {
				vector<int>* tex = new vector<int>();
				tex->push_back(LoadTexture("pic/smoke.png"));
				if (rand() > 27000) RaEffect::smoke(i->GetPosition(), 150, tex, 1);
				continue;
			}

			bud->RStrength = 0;
			if (bud->feature == "raffica") {					//电源？
				bud->RStrength = 10;
				continue;
			}
			if (bud->type == "defend") {						//如果是电路元件？
				auto dud = (DefendUserData*)bud;
				if (dud->defendType == "monitor") {				//监视器
					for (float angle = 0.0f; angle < 360.0f; angle += 10.0f) {
						unsigned int _side = PUBLIC_IDX | FRIEND_IDX | ENEMY_IDX;
						auto look = queryLook(i, b2Vec2(20 * cos(angle), 20 * sin(angle)), _side);
						if (!look) continue;
						if (look->GetFixtureList()->GetFilterData().categoryBits & (dud->attack?FRIEND_IDX:ENEMY_IDX)) {
							bud->RStrength = 10;
							break;
						}
					}
				}
			}
		}
		for (auto i = curTest->m_world->GetBodyList(); i; i = i->GetNext()) {
			if (!i) continue;
			BreakableUserData* bud = (BreakableUserData*)i->GetUserData();
			if (!bud) continue;
			if (!bud->RStrength) continue;

			queue<b2Body*> q;								//广度优先搜索，用来让信号通过导线流动
			q.push(i);
			while (!q.empty()) {
				auto x = q.front();
				auto xud = ((BreakableUserData*)x->GetUserData());
				q.pop();
				if (xud->RStrength == 0) continue;
				for (auto j = x->GetJointList(); j; j = j->next) {
					if (!j->joint) continue;
					DistanceJointUserData* ud = (DistanceJointUserData*)j->joint->GetUserData();
					if (!ud) continue;
					if (ud->feature == "R_TRANSPORT") {
						b2Body* target;
						if (j->joint->GetBodyB() != x) target = j->joint->GetBodyB();
						else target = j->joint->GetBodyA();
						
						BreakableUserData* bud = (BreakableUserData*)target->GetUserData();
						bool flag = false;
						if (!bud) continue;
						if (bud->RStrength > xud->RStrength) continue;
						if (bud->type == "defend") {
							auto dud = (DefendUserData*)bud;
							if (dud->defendType == "cap") {
								if (xud->RStrength != 10) continue;
							}
							if (dud->defendType == "repeater") {
								if (xud->RStrength) flag = true;
							}
						}
						bud->RStrength = xud->RStrength - 1;
						if (flag) bud->RStrength = 10;
						q.push(target);
					}
				}
			}
		}
	}
};













/*
			else if(type == "car") {
				float x, y, ftw, fth, bkw, bkh, tlw, tlh, wr;
				unsigned int side;
				int pos;
				scanf("%f%f%f%f%f%f%f%f%f", &x, &y, &ftw, &fth, &bkw, &bkh, &tlw, &tlh, &wr);
				scanf("%u%d", &side, &pos);
				
				RaCar* car = Raffica::createCar(x, y, ftw, fth, bkw, bkh, tlw, tlh, wr, side, pos?true:false);

				for(int i = 0;i < 5;i ++) {
					string part, url;
					cin >> part;
					cin >> userdataType;
					if(userdataType == "none") continue;
					if(userdataType == "breakable") {
						cin >> hp >> defend;
						vector<int>* urls = new vector<int>();
						while(cin >> url) {
							if(url != "end") {
								urls->push_back(LoadTexture(url.c_str()));
							} else break;
						}
						BreakableUserData* bud = new BreakableUserData();
						bud->Init(hp, defend, urls);
							 if(part == "front") Raffica::setUserDataByTexture(car->front, bud, 1, car->frontWidth, car->frontHeight);
						else if(part == "tail" ) Raffica::setUserDataByTexture(car->tail , bud, 1, car->tailWidth , car->tailHeight );
						else if(part == "back" ) Raffica::setUserDataByTexture(car->back , bud, 1, car->backWidth , car->backHeight );
						else if(part == "wheell" ) Raffica::setUserDataByTexture(car->wheelLeft , bud, 1, car->wheelR*2, car->wheelR*2);
						else if(part == "wheelr") Raffica::setUserDataByTexture(car->wheelRight, bud, 1, car->wheelR*2, car->wheelR*2);
						else assert(0);
					}
				}

			}

			else if(type == "human") {
				float x, y, headr, bodyw, bodyh, armw, armh, legw, legh;
				unsigned int side;
				scanf("%f%f%f%f%f%f%f%f%f%u", &x, &y, &headr, &bodyw, &bodyh, &armw, &armh, &legw, &legh, &side);
				RaHuman* human = Raffica::createHuman(x, y, headr, bodyw, bodyh, armw, armh, legw, legh, side);
				for(int i = 0;i < 10;i ++) {
					string part, url;
					cin >> part;
					cin >> userdataType;
					if(userdataType == "none") continue;
					if(userdataType == "breakable") {
						cin >> hp >> defend;
						vector<int>* urls = new vector<int>();
						while(cin >> url) {
							if(url != "end") {
								urls->push_back(LoadTexture(url.c_str()));
							} else break;
						}
						BreakableUserData* bud = new BreakableUserData();
						bud->Init(hp, defend, urls);
							 if(part == "head") Raffica::setUserDataByTexture(human->head, bud, 1, human->headR*2, human->headR*2);
						else if(part == "body") Raffica::setUserDataByTexture(human->body , bud, 1, human->bodyW , human->bodyH);
						else if(part == "arml1") Raffica::setUserDataByTexture(human->l_arm1, bud, 1, human->armW , human->armH/2);
						else if(part == "arml2") Raffica::setUserDataByTexture(human->l_arm2, bud, 1, human->armW , human->armH/2);
						else if(part == "armr1") Raffica::setUserDataByTexture(human->r_arm1, bud, 1, human->armW , human->armH/2);
						else if(part == "armr2") Raffica::setUserDataByTexture(human->r_arm2, bud, 1, human->armW , human->armH/2);
						else if(part == "legl1") Raffica::setUserDataByTexture(human->l_leg1, bud, 1, human->legW , human->legH/2);
						else if(part == "legl2") Raffica::setUserDataByTexture(human->l_leg2, bud, 1, human->legW , human->legH/2);
						else if(part == "legr1") Raffica::setUserDataByTexture(human->r_leg1, bud, 1, human->legW , human->legH/2);
						else if(part == "legr2") Raffica::setUserDataByTexture(human->r_leg2, bud, 1, human->legW , human->legH/2);
						else assert(0);

					}
				}
			}*/