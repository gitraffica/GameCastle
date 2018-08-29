/*
* RaRules.h
*
* Author: Raffica
*
* 作为RaRules.cpp的头文件（废话），声明了所有游戏里用到的类
*
*/


#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "Box2D\Box2D.h"

#define INF 1000000000

using namespace std;

/*
* Material
* 材质类，每个材质都对应着纹理的长宽、防御力、hp、摩擦、弹性、密度、名字
* 除此之外 还有对应的选择材质按钮
*/

struct Material {
	float width, height, defend, hp, density, friction, rest;
	string name;
	vector<int>* textures;
	vector<string> urls;
	bool enableTile;
	string buttonDownURL, buttonUpURL;
};

class BasicUserData;

/*
* Building
*
* 当你要保存时，你需要用这个类来描述你要保存的东西
*/

class Building {

public:

	static int count;												//building的总个数

	string name;
	int id;															//当前building的编号
	float x, y, density, friction, rest, width, height, radius;
	unsigned int side, maskbit;
	bool collide;
	vector<b2Vec2> vertex;

	b2Body *body;
	b2Body *body1, *body2;
	b2Joint *joint;
	b2Vec2 anc1, anc2;

	BasicUserData* userdata;

	Building();
	~Building();

};

class RaButton;
class DefendUserData;

typedef void (*RaPress)		    (const b2Vec2& p, string* mode, const RaButton* button);
typedef void (*RaMaterialPress)	(const b2Vec2& p, string* material, const RaButton* button);
typedef void (*RaDefender)		(DefendUserData* ud);

/*
* RaBasicButton
* 为了防止以后还有别的Button类而定义的类，请把它当成抽象类用【
*/

class RaBasicButton {

public:

	b2Body* body;
	string name;
	vector<string> urls;
	bool pressed;
	float originWidth, originHeight;
	
};

/*
* RaButton
* 
* 按钮，使用了函数指针
* 一个按钮对应一个刚体
*
*/

class RaButton: public RaBasicButton {

public:

	bool isMaterialButton;
	Material* material;

	RaButton(b2Body* body, string name, vector<string>& urls);
	~RaButton();

	void setPressDown(RaPress p);
	void setPressUp	 (RaPress p);
	void setPressMove(RaPress p);

	void pressDown(const b2Vec2& p, string* mode, const RaButton* button);
	void pressUp  (const b2Vec2& p, string* mode, const RaButton* button);
	void pressMove(const b2Vec2& p, string* mode, const RaButton* button);
	RaPress _pressDown;
	RaPress _pressUp;
	RaPress _pressMove;

};

/*
* BasicUserData
* BasicUserData是所有种类的Userdata的基类，Userdata指在刚体上的贴图以及用于放一些事件（比如Display）
* 抽象类 无法实体化
*/

class BasicUserData {

public:
	
	static const string NORMAL;
	static const string BREAKABLE;
	static const string STATIC;
	static const string BASIC;						//似乎并没有什么卵用？
	static const string DISTANCEJOINT;
	static const string DEFEND;
	static const string EXPLOSIVE;

	static int count;								//userdata有几个？

	bool attack;									//是否作为进攻方

	int this_count;									//userdata编号
	float width, height;
	int textureIndex;								//纹理编号;
	string type;
	string name;
	bool isButton;									//是不是按钮？
	bool breakable;
	float buttonX, buttonY;
	RaButton* button;
	bool enableTile;								//纹理是否平铺？
	float tileX, tileY;								//平铺的话，texCoord2f的参数是？
	bool polygon;									//是否以多边形边界渲染
	bool ball;										//是否是个球
	vector<b2Vec2> vertexs;
	void* target;									//取决于继承者，例如BreakableUserData的target是个b2Body*

	float RStrength;

	string feature;									//特性，比如尖锐，默认为NONE
	string tip;										//作为提示使用()
	string _tip;									//每个物品的特性

	virtual void Display() = 0;
	virtual void updateTip();

	BasicUserData();
	virtual ~BasicUserData();

};

/*
* DistanceJointUserData
*
* 给距离关节定做的Userdata，目前为止只有钢筋
*
*/

class DistanceJointUserData: public BasicUserData {

public:

	DistanceJointUserData();
	~DistanceJointUserData();
	
	virtual void Display();

};

/*
* UserData
*
* 不一定会碎掉的UserData
*/

class UserData: public BasicUserData {

public:

	float sharp;
	b2Vec2 lastPosition;

	UserData();
	virtual ~UserData();

	virtual void Init(float _width, float _height);

	virtual void setTexture(int url);

	virtual void Display();

};

/*
* StaticUserData
*
* 通常来讲，这东西是作为背景用的
*/

class StaticUserData: public UserData {

public:

	StaticUserData();
	~StaticUserData();

	float x, y;
	float angle;								//弧度

	virtual void Init(float _width, float _height, float _x, float _y, float _angle = 0);

	void Display();

};

/*
* BreakableUserData
*
* 可以碎掉的UserData 有hp和defend两个属性
* 当碎掉时，销毁对应的刚体以及附带的关节
*/

class BreakableUserData: public UserData {

public:

	int life;
	float maxhp, hp, defend;					//最大血量 当前血量 防御力
	vector<int>* textureIndexs;
	bool PVPdeleted;							//当在PVP模式时，每次更新时是否被删除？
	
	BreakableUserData();
	BreakableUserData(const BreakableUserData& ud);
	~BreakableUserData();

	virtual void Init(float _width, float _height, float _hp, float _defend, vector<int>* textures);
	virtual void Init(float _hp, float _defend, vector<int>* textures);
	virtual void updateTip();

	virtual int UpdateTexture();

	virtual void Display();

};

/*
* DefendUserData
*
* 被赋予了这个UserData的刚体，将会对BasicUserData::attack=true的刚体发动攻击。
*/

class DefendUserData: public BreakableUserData {

public:

	RaDefender defenderFunc;
	RaDefender destroyDefenderFunc;
	int skillCD;
	string defendType;

	DefendUserData();
	~DefendUserData();

	virtual void Display();												//用作防御


};

/* 
* ExplosiveUserData
*
* 被破坏后会爆炸
*/

class ExplosiveUserData : public BreakableUserData {

	float power;

public:

	ExplosiveUserData(float p);
	~ExplosiveUserData();

};

/*
* ExplosiveEffectUserData
*
* 爆炸特效后产生的userdata，过一段时间后自行销毁
*/

class ExplosiveEffectUserData : public BreakableUserData {

public:

	bool widthReduce;

	ExplosiveEffectUserData(float hp);

	virtual void Display();							//按帧掉血

};

////////////////////////////////////////////////////////////////////////
////////////////////////目前废弃掉的设定////////////////////////////////
////////////////////////////////////////////////////////////////////////



class RaCar {

public:

	unsigned int side;
	bool position;									//朝向

	float frontWidth, frontHeight;
	float backWidth, backHeight;
	float wheelR;
	float tailWidth, tailHeight;

	b2Body *wheelLeft, *wheelRight;
	b2Body *wheelHoldLeft, *wheelHoldRight;
	b2Body *front, *tail, *back;

	void Init(float frW, float frH, float baW, float baH, float whR, float taW, float taH, unsigned int si, bool pos);
};

class RaHuman {

public:
	b2Body *head, *body, *l_arm1, *l_arm2;
	b2Body *r_arm1, *r_arm2;
	b2Body *l_leg1, *l_leg2;
	b2Body *r_leg1, *r_leg2;
	b2Joint *l_arm_joint1, *r_arm_joint1;
	b2Joint *l_arm_joint2, *r_arm_joint2;
	b2Joint *weapon_joint;
		
	float headR;
	float bodyW;
	float bodyH;
	float armW;
	float armH;
	float legW;
	float legH;
	unsigned int side;

	void Init(float _headR, float _bodyW, float _bodyH, float _armW, float _armH, float _legW, float _legH, unsigned int _side);
};