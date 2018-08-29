/*
* RaRules.h
*
* Author: Raffica
*
* ��ΪRaRules.cpp��ͷ�ļ����ϻ�����������������Ϸ���õ�����
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
* �����࣬ÿ�����ʶ���Ӧ������ĳ�����������hp��Ħ�������ԡ��ܶȡ�����
* ����֮�� ���ж�Ӧ��ѡ����ʰ�ť
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
* ����Ҫ����ʱ������Ҫ���������������Ҫ����Ķ���
*/

class Building {

public:

	static int count;												//building���ܸ���

	string name;
	int id;															//��ǰbuilding�ı��
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
* Ϊ�˷�ֹ�Ժ��б��Button���������࣬��������ɳ������á�
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
* ��ť��ʹ���˺���ָ��
* һ����ť��Ӧһ������
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
* BasicUserData�����������Userdata�Ļ��࣬Userdataָ�ڸ����ϵ���ͼ�Լ����ڷ�һЩ�¼�������Display��
* ������ �޷�ʵ�廯
*/

class BasicUserData {

public:
	
	static const string NORMAL;
	static const string BREAKABLE;
	static const string STATIC;
	static const string BASIC;						//�ƺ���û��ʲô���ã�
	static const string DISTANCEJOINT;
	static const string DEFEND;
	static const string EXPLOSIVE;

	static int count;								//userdata�м�����

	bool attack;									//�Ƿ���Ϊ������

	int this_count;									//userdata���
	float width, height;
	int textureIndex;								//������;
	string type;
	string name;
	bool isButton;									//�ǲ��ǰ�ť��
	bool breakable;
	float buttonX, buttonY;
	RaButton* button;
	bool enableTile;								//�����Ƿ�ƽ�̣�
	float tileX, tileY;								//ƽ�̵Ļ���texCoord2f�Ĳ����ǣ�
	bool polygon;									//�Ƿ��Զ���α߽���Ⱦ
	bool ball;										//�Ƿ��Ǹ���
	vector<b2Vec2> vertexs;
	void* target;									//ȡ���ڼ̳��ߣ�����BreakableUserData��target�Ǹ�b2Body*

	float RStrength;

	string feature;									//���ԣ��������Ĭ��ΪNONE
	string tip;										//��Ϊ��ʾʹ��()
	string _tip;									//ÿ����Ʒ������

	virtual void Display() = 0;
	virtual void updateTip();

	BasicUserData();
	virtual ~BasicUserData();

};

/*
* DistanceJointUserData
*
* ������ؽڶ�����Userdata��ĿǰΪֹֻ�иֽ�
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
* ��һ���������UserData
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
* ͨ���������ⶫ������Ϊ�����õ�
*/

class StaticUserData: public UserData {

public:

	StaticUserData();
	~StaticUserData();

	float x, y;
	float angle;								//����

	virtual void Init(float _width, float _height, float _x, float _y, float _angle = 0);

	void Display();

};

/*
* BreakableUserData
*
* ���������UserData ��hp��defend��������
* �����ʱ�����ٶ�Ӧ�ĸ����Լ������Ĺؽ�
*/

class BreakableUserData: public UserData {

public:

	int life;
	float maxhp, hp, defend;					//���Ѫ�� ��ǰѪ�� ������
	vector<int>* textureIndexs;
	bool PVPdeleted;							//����PVPģʽʱ��ÿ�θ���ʱ�Ƿ�ɾ����
	
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
* �����������UserData�ĸ��壬�����BasicUserData::attack=true�ĸ��巢��������
*/

class DefendUserData: public BreakableUserData {

public:

	RaDefender defenderFunc;
	RaDefender destroyDefenderFunc;
	int skillCD;
	string defendType;

	DefendUserData();
	~DefendUserData();

	virtual void Display();												//��������


};

/* 
* ExplosiveUserData
*
* ���ƻ���ᱬը
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
* ��ը��Ч�������userdata����һ��ʱ�����������
*/

class ExplosiveEffectUserData : public BreakableUserData {

public:

	bool widthReduce;

	ExplosiveEffectUserData(float hp);

	virtual void Display();							//��֡��Ѫ

};

////////////////////////////////////////////////////////////////////////
////////////////////////Ŀǰ���������趨////////////////////////////////
////////////////////////////////////////////////////////////////////////



class RaCar {

public:

	unsigned int side;
	bool position;									//����

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