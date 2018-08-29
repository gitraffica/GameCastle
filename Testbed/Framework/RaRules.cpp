/*
* 规定各种玩具的定义
* AUTHOR:Raffica
*/
#include "freeglut/freeglut.h"
#include "Test.h"
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include "RaRules.h"
#include "RaEasy.h"
#include <vector>
using namespace std;

extern Settings settings;
extern float32 viewZoom;

/*
* 请在声明一个userdata变量后调用Init函数；
* 请使用Raffica::setUserDataByTexture来绑定纹理，若无绑定的刚体，请把它Init后加入Raffica::userdataList中
*/

void originClear(BasicUserData* _this) {
	vector<BasicUserData*>& a = Raffica::userdataList;
	unsigned int i, j;
	for(i = 0;i < a.size();i ++) {
		if(a[i] == _this) {
			for(j = i+1;j < a.size();j ++) {
				a[j-1] = a[j];
				if(a[j]) a[j]->this_count --;
			}
			a.resize(a.size() - 1);
			a.shrink_to_fit();
			--_this->count;
			return;
		}
	}
}

void ClearUserData(UserData* _this) {
	originClear(_this);
	if(_this->target) ((b2Body*)(_this->target))->SetUserData(NULL);
}

void ClearUserData(BasicUserData* _this) {
	originClear(_this);
}

void ClearUserData(DistanceJointUserData* _this) {
	originClear(_this);
	if(_this->target) ((b2DistanceJoint*)(_this->target))->SetUserData(NULL);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*RaButton*/
	RaButton::RaButton(b2Body* _body, string _name, vector<string>& _urls) {
		body = _body; name = _name, urls.assign(_urls.begin(), _urls.end());
		_pressMove = _pressDown = _pressUp = 0; pressed = false;
		this->isMaterialButton = false;
	}
	
	RaButton::~RaButton() {
		if(body) if(body->GetUserData()) delete (UserData*)body->GetUserData();
		if(body) body->GetWorld()->DestroyBody(body);
		
		for(vector<RaButton*>::iterator i = Raffica::ButtonList.begin();i != Raffica::ButtonList.end();i ++) {
			if(*i == this) {
				Raffica::ButtonList.erase(i);
				break;
			}
		}
		cout << "Destroy: " << this->name << endl;
	}
		
	void RaButton::pressDown(const b2Vec2& p, string* mode, const RaButton* button) {
		pressed = true;
		BasicUserData* ud = (BasicUserData*)this->body->GetUserData();
		ud->textureIndex = LoadTexture(this->urls[1].c_str());
		if(this->_pressDown) (this->_pressDown)(p, mode, button);
	}

	void RaButton::pressUp(const b2Vec2& p, string* mode, const RaButton* button)   {
		pressed = false;
		BasicUserData* ud = (BasicUserData*)this->body->GetUserData();
		ud->textureIndex = LoadTexture(this->urls[0].c_str());
		if(this->_pressUp) (this->_pressUp)(p, mode, button);
	}

	void RaButton::pressMove(const b2Vec2& p, string* mode, const RaButton* button) {
		if(!pressed) return;
		BasicUserData* ud = (BasicUserData*)this->body->GetUserData();
		ud->textureIndex = LoadTexture(this->urls[1].c_str());
		if(this->_pressMove) (this->_pressMove)(p, mode, button);
	}

	void RaButton::setPressDown(RaPress p) {
		_pressDown = p;
	}

	void RaButton::setPressUp(RaPress p) {
		_pressUp = p;
	}

	void RaButton::setPressMove(RaPress p) {
		_pressMove = p;
	}

/*}*/

/*Building*/
	Building::Building() {
		id = ++count;
	}

	Building::~Building() {
		vector<Building*>& a = Raffica::buildingList;
		for(vector<Building*>::iterator i = a.begin();i != a.end();i ++) if(*i == this) {
			a.erase(i); break;
		}
	}
/*}*/

/*BasicUserData {*/
	int BasicUserData::count = 0;
	
	const string BasicUserData::NORMAL = "normal";
	const string BasicUserData::BREAKABLE = "breakable";
	const string BasicUserData::STATIC = "static";
	const string BasicUserData::DISTANCEJOINT = "distancejoint";
	const string BasicUserData::BASIC = "basic";
	const string BasicUserData::DEFEND = "defend";
	const string BasicUserData::EXPLOSIVE = "explosive";

	void BasicUserData::updateTip() {

	}
	BasicUserData::BasicUserData() {
		polygon = 0; textureIndex = 0; type = BASIC; this_count = ++ count; isButton = false; enableTile = false; 
		ball = 0; tileX = tileY = .3f; attack = false; button = 0;
		feature = "NONE";
		RStrength = 0;
		tip = _tip = "暂时还没有tip";
		breakable = 0;
	}
	BasicUserData::~BasicUserData() {
	}
	 
/*}*/

/*DistanceJointUserData {*/
	DistanceJointUserData::DistanceJointUserData() { target = NULL; textureIndex = 0; type = DISTANCEJOINT; }
	DistanceJointUserData::~DistanceJointUserData() {
		b2Joint* j = (((b2Joint*)this->target));
		float x = (j->GetAnchorA().x + j->GetAnchorB().x) / 2;
		float y = (j->GetAnchorA().y + j->GetAnchorB().y) / 2;
		float vx = (j->GetBodyA()->GetLinearVelocity().x + j->GetBodyB()->GetLinearVelocity().x) / 2;
		float vy = (j->GetBodyA()->GetLinearVelocity().y + j->GetBodyB()->GetLinearVelocity().y) / 2;
		vector<int>* tex = new vector<int>();
		tex->push_back(this->textureIndex);
		RaEffect::blow(b2Vec2(x, y), b2Vec2(vx, vy), 20, tex);
		ClearUserData(this);
		vector<Building*>& a = Raffica::buildingList;
		for(vector<Building*>::iterator i = a.begin();i != a.end();i ++) {
			if((*i)->joint == this->target) {
				a.erase(i);
				break;
			}
		}
	}


	void DistanceJointUserData::Display() {
		if (!this->target) delete this;
		if(!((b2Joint*)this->target)->GetBodyA()->GetUserData() || !((b2Joint*)this->target)->GetBodyB()->GetUserData()) {
			delete this;
			return;
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glLoadIdentity(); 
		b2DistanceJoint* joint = (b2DistanceJoint*)target;
		float x1 = joint->GetAnchorA().x;
		float x2 = joint->GetAnchorB().x;
		float y1 = joint->GetAnchorA().y;
		float y2 = joint->GetAnchorB().y;
		float len = joint->GetLength();
		float angle = atan2((y1-y2), (x1-x2));
		
		glTranslatef((x1+x2) / 2, (y1+y2) / 2, 0);
		glRotatef(angle*180.0f/3.1416f+90.0f, 0, 0, 1.0f);
		glBindTexture(GL_TEXTURE_2D, this->textureIndex);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-width/2, -len/2);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-width/2, len/2);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(width/2, len/2);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(width/2, -len/2);
		glEnd();
		glDisable(GL_BLEND);
	}

/*}*/

/*UserData {*/
	UserData::UserData() { target = NULL, textureIndex = 0; type = NORMAL; sharp = 1.0f; button = NULL; lastPosition = b2Vec2(-1000, -1000); }
	UserData::~UserData() {
		ClearUserData(this);
		if (this->target) ((b2Body*)this->target)->SetUserData(NULL);
		if (this->button) this->button->body = NULL;
	}

	void UserData::setTexture(int url) {
		this->textureIndex = url;
	}

	void UserData::Init(float _width, float _height) {
		width = _width, height = _height;
	}

	void UserData::Display() {

		//if(!this || !this->target) return;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		/*if (this->RStrength != 0) {
			vector<int>* texs = new vector<int>();
			texs->push_back(LoadTexture("pic/rj.png"));
			if(rand() > 32000) RaEffect::smoke(((b2Body*)this->target)->GetPosition(), this->RStrength * 10, texs, 1);
		}*/

		if(isButton) {
			//buttonX += Raffica::button_Xoffset;
			b2Body* body = (b2Body*)this->target;
			body->SetTransform(b2Vec2(Raffica::button_Xoffset + (settings.viewCenter.x+viewZoom*buttonX), (settings.viewCenter.y+viewZoom*buttonY)), 0);
			((b2PolygonShape*)body->GetFixtureList()->GetShape())->SetAsBox(((UserData*)body->GetUserData())->width/2, ((UserData*)body->GetUserData())->height/2);
			((UserData*)body->GetUserData())->width = ((UserData*)body->GetUserData())->button->originWidth*viewZoom*2;
			((UserData*)body->GetUserData())->height= ((UserData*)body->GetUserData())->button->originHeight*viewZoom*2;
		}

		glLoadIdentity();
		b2Body* body = (b2Body*)this->target;
		if(!isButton) glTranslatef(body->GetPosition().x, body->GetPosition().y, 0);
		else {
			glTranslatef(body->GetPosition().x - Raffica::button_Xoffset, body->GetPosition().y, 0);
		}
		glRotatef(body->GetAngle()*180.0f/3.1416f, 0, 0, 1.0f);
		glBindTexture(GL_TEXTURE_2D, this->textureIndex);
		glBegin(GL_POLYGON);
		if(this->ball) {
			if (enableTile) {
				int iter = 40;
				if (this->width < 3) iter = 5;
				for (int i = 0; i < iter; i++) {
					glVertex2f(width*cos(2 * 3.14f / iter * i) / 2, width*sin(2 * 3.14f / iter * i) / 2);
					glTexCoord2f(width*cos(2 * 3.14f / iter * i) / 2 / tileX, width*sin(2 * 3.14f / iter * i) / 2 / tileY);
				}
			}
			if (!enableTile) {
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-width / 2, -height / 2);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-width / 2, height / 2);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(width / 2, height / 2);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(width / 2, -height / 2);
			}
		}
		else if(!this->polygon) {
			if(!enableTile) {
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-width/2, -height/2);
				glTexCoord2f(0.0f, 1.0f); glVertex2f(-width/2, height/2);
				glTexCoord2f(1.0f, 1.0f); glVertex2f(width/2, height/2);
				glTexCoord2f(1.0f, 0.0f); glVertex2f(width/2, -height/2);
			} else {
				glTexCoord2f(0.0f, 0.0f); glVertex2f(-width/2, -height/2);
				glTexCoord2f(0.0f, height/tileY); glVertex2f(-width/2, height/2);
				glTexCoord2f(width/tileX, height/tileY); glVertex2f(width/2, height/2);
				glTexCoord2f(width/tileX, 0.0f); glVertex2f(width/2, -height/2);
			}
		} else {
			for(unsigned int i = 0;i < this->vertexs.size();i ++) {
				glTexCoord2f(this->vertexs[i].x, this->vertexs[i].y);
				glVertex2f(this->vertexs[i].x, this->vertexs[i].y);
			}
		}
		glEnd();
		glDisable(GL_BLEND);
	}

/*}*/

/*StaticUserData {*/

	StaticUserData::StaticUserData() { type = STATIC; }
	StaticUserData::~StaticUserData() {}

	void StaticUserData::Init(float _width, float _height, float _x, float _y, float _angle) {
		x = _x, y = _y, width = _width, height = _height; angle = _angle;
		Raffica::userdataList.push_back(this);
	}

	void StaticUserData::Display() {
		glLoadIdentity();
		glTranslatef(this->x, this->y, 0);
		glRotatef(this->angle*180.0f/3.1416f, 0, 0, 1.0f);
		glBindTexture(GL_TEXTURE_2D, this->textureIndex);
		glBegin(GL_POLYGON);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-width/2, -height/2);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-width/2, height/2);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(width/2, height/2);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(width/2, -height/2);
		glEnd();
	}
/*}*/

/*
* hp表示生命值，defend表示每次碰撞时用冲量减去defend作为伤害
* textureIndexs表示不同的hp对应不同的纹理，越低的textureIndexs的下标对应的hp越低
*/

/*BreakableUserData:UserData {*/
	BreakableUserData::BreakableUserData() {
		hp = 0, defend = 0; type = BREAKABLE; textureIndexs = new vector<int>(); breakable = true;
		_tip = ""; life = 0;
		PVPdeleted = false;
		lastPosition = b2Vec2(-1000, -1000);
	}
	BreakableUserData::BreakableUserData(const BreakableUserData& ud) {
		BreakableUserData();
		type = BREAKABLE;
		this->Init(ud.hp, ud.defend, ud.textureIndexs);
		this->enableTile = ud.enableTile;
		this->tileX = ud.tileX, this->tileY = ud.tileY;
		this->width = ud.width, this->height = ud.height;
		this->textureIndexs = ud.textureIndexs;
		this->ball = ud.ball;
		breakable = true;
		life = 0;
	}
	BreakableUserData::~BreakableUserData() {				//摧毁这个刚体所有的关节userdata

		//封装的不是很好，这地方的判断本来应该和explosiveUserData一起做的
		
		ClearUserData(this);

		b2Body* b = ((b2Body*)this->target);

		if (life) {
			if (feature == "tnt") Raffica::explosive(b->GetPosition(), b->GetMass() * 10);
			if (type == "explosiveeffect") Raffica::effectCNT--;
		}

		vector<Building*>& a = Raffica::buildingList;
		for (vector<Building*>::iterator i = a.begin(); i != a.end(); i++) {
			if ((*i)->body == this->target) {
				//(*i)->userdata = NULL;
				a.erase(i);
				break;
			}
		}

		if (hp <= 0) {
			if (!b) return;

			b2Vec2 m1(b->GetPosition().x - width / 2, b->GetPosition().y - height / 2);
			b2Vec2 m2(b->GetPosition().x + width / 2, b->GetPosition().y + height / 2);
			//Raffica::createParticle(m1, m2);
			for (float i = m1.x; i < m2.x; i += 3) {
				for (float j = m1.y; j < m2.y; j += 3) {
					float _x = i, _y = j;
					i = (m1.x + m2.x) / 2 + (i - (m1.x + m2.x) / 2) * cos(b->GetAngle());
					j = (m1.y + m2.y) / 2 + (j - (m1.y + m2.y) / 2) * sin(b->GetAngle());
					Raffica::createParticle(b2Vec2(i, j), b2Vec2(i + .5f, j + .5f), 6 + rand()*5.0f / 32767, 200, 200, 200);
					i = _x, j = _y;
					//cout << i << ' ' << j << endl;
				}
			}
		}

		if (this->target == NULL) return;
		b2Body* body = (b2Body*)this->target;
		this->target = NULL;
		body->SetUserData(NULL);
		body->GetWorld()->DestroyBody(body);

		/////unfinished(?)
	}

	void BreakableUserData::Init(float _width, float _height, float _hp, float _defend, vector<int>* textures) {
		if(_hp <= 0) throw("breakableUserData的hp值必须大于0");
		if(_defend <= 0) throw("breakableUserData的defend值必须大于0");
		width = _width;
		height = _height;
		maxhp = hp = _hp, defend = _defend;
		textureIndexs = textures;
		RStrength = 0;
	}

	void BreakableUserData::Init(float _hp, float _defend, vector<int>* textures) {
		if (_hp <= 0) {
			//throw("breakableUserData的hp值必须大于0");
		}
		if(_defend < 0) throw("breakableUserData的defend值必须大于0");
		maxhp = hp = _hp, defend = _defend;
		RStrength = 0;
		textureIndexs = textures;
	}

	int BreakableUserData::UpdateTexture() {
		float rate = hp / maxhp;
		float sz = (float)(textureIndexs->size());
		float res = rate * sz;
		res -= 0.00001f;
		if(floor(res) < 0) {
			return -1;
		}
		b2Body* b = ((b2Body*)this->target);
		if ((int)floor(res) != sz-1 && this->textureIndex != (*textureIndexs)[(int)floor(res)] &&
			this->type != "explosiveeffect") {			//被打坏
			//RaEffect::blow(((b2Body*)this->target)->GetPosition(), ((b2Body*)this->target)->GetLinearVelocity(), this->width * this->height/5, textureIndexs, 5, this->feature == "spine" ? 30.0f : 1.0f);
			b2Vec2 m1(b->GetPosition().x - width / 2, b->GetPosition().y - height / 2);
			b2Vec2 m2(b->GetPosition().x + width / 2, b->GetPosition().y + height / 2);
			//Raffica::createParticle(m1, m2);
			for (float i = m1.x; i < m2.x; i += 5) {
				for (float j = m1.y; j < m2.y; j += 5) {
					float _x = i, _y = j;
					i = (m1.x + m2.x) / 2 + (i - (m1.x + m2.x) / 2) * cos(b->GetAngle());
					j = (m1.y + m2.y) / 2 + (j - (m1.y + m2.y) / 2) * sin(b->GetAngle());
					Raffica::createParticle(b2Vec2(i, j), b2Vec2(i+.5f, j+.5f), 6 + rand()*5.0f/32767);
					i = _x, j = _y;
				}
			}
		}
		this->textureIndex = (*textureIndexs)[(int)floor(res)];
		return (int)floor(res);
	}

	void BreakableUserData::updateTip() {
		if (this->feature == "spine") {
			_tip = "尖刺";
		}
		if (this->feature == "explosive" || this->feature == "rocket" || this->feature == "tnt") {
			_tip = "爆炸物";
		}
		tip = _tip + (_tip == "" ? "" : ", ") + "Breakable, HP=" + 
			to_string(this->hp) + ", maxHP=" + to_string(this->maxhp) + ", defend=" + to_string(this->defend);
	}

	void BreakableUserData::Display() {
		life++;
		if(UpdateTexture() == -1) {
			if(this->type != "explosiveeffect")
				RaEffect::blow(((b2Body*)this->target)->GetPosition(), ((b2Body*)this->target)->GetLinearVelocity(), this->width * this->height, textureIndexs, 5, this->feature == "spine" ? 30.0f : 1.0f);
			delete this;
			return;
		}
		if (lastPosition.x == -10000) {
			lastPosition = ((b2Body*)target)->GetPosition();
		}
		UserData::Display();
	}

/*}*/


/*DefendUserData {*/

	DefendUserData::DefendUserData() {
		type = "defend";
		skillCD = 0;
		breakable = true;
		destroyDefenderFunc = NULL;
	}

	DefendUserData::~DefendUserData() {
		ClearUserData(this);
		if(destroyDefenderFunc && life) destroyDefenderFunc(this);
	}

	void DefendUserData::Display() {
		if (life) defenderFunc(this);
		BreakableUserData::Display();
	}

/*}*/

/*ExplosiveUserData {*/

	ExplosiveUserData::ExplosiveUserData(float p) {
		breakable = true;
		type = "explosive";
		power = p;
	}

	ExplosiveUserData::~ExplosiveUserData() {
		Raffica::explosive(((b2Body*)this->target)->GetPosition(), power);
	}

/*}*/

/*ExplosiveEffectUserData*/

	ExplosiveEffectUserData::ExplosiveEffectUserData(float _hp) {
		breakable = false;
		type = "explosiveeffect";
		Init(_hp, 0, 0);
		widthReduce = true;
		Raffica::effectCNT++;
	}

	void ExplosiveEffectUserData::Display() {
		if (Raffica::effectCNT > Raffica::effectUpper) {
			delete this;
			return;
		}
		hp--;
		if(widthReduce == true) width *= (float)hp / (hp + 1);
		height*= (float)hp / (hp + 1);
		/*
		b2Body* b = (b2Body*)this->target;
		BasicUserData* ud = (BasicUserData*)b->GetUserData();
		b2Vec2 p = b->GetPosition();

		Raffica::test->m_world->DestroyBody(b);
		
		b = Raffica::createBall(p.x, p.y, width / 2, 3.0f, .3f, .2f, this);
		*/
		BreakableUserData::Display();
	}

/*}*/






/////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////已废弃//////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////

/*RaCar {*/
void RaCar::Init(float frW, float frH, float baW, float baH, float whR, float taW, float taH, unsigned int si, bool pos) {
		frontWidth = frW, frontHeight = frH, backHeight = baH, backWidth = baW, wheelR = whR, tailWidth = taW, tailHeight = taH;
		position = pos, side = si;
	}

/*};*/

/*RaHuman {*/
	void RaHuman::Init(float _headR, float _bodyW, float _bodyH, 
								 float _armW, float _armH, 
								 float _legW, float _legH, 
								 unsigned int _side) {
		headR = _headR, bodyW = _bodyW, bodyH = _bodyH, armW = _armW, armH = _armH, legW = _legW, legH = _legH, side = _side;
	}

/*}*/
