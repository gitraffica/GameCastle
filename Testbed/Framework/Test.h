/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/
#pragma once

#ifndef TEST_H
#define TEST_H

#include <Box2D/Box2D.h>
#include "Render.h"
#include "ParticleParameter.h"

#include <cstdlib>

#define DRAW_STRING_NEW_LINE 25

class Test;
struct Settings;

typedef Test* TestCreateFcn();

#define	RAND_LIMIT	32767

void SetRestartOnParticleParameterChange(bool enable);
uint32 SetParticleParameterValue(uint32 value);
uint32 GetParticleParameterValue();
void SetParticleParameters(
	const ParticleParameter::Definition * const particleParameterDef,
	const uint32 particleParameterDefCount);

/// Random number in range [-1,1]
inline float32 RandomFloat()
{
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = 2.0f * r - 1.0f;
	return r;
}

/// Random floating point number in range [lo, hi]
inline float32 RandomFloat(float32 lo, float32 hi)
{
	float32 r = (float32)(std::rand() & (RAND_LIMIT));
	r /= RAND_LIMIT;
	r = (hi - lo) * r + lo;
	return r;
}

/// Test settings. Some can be controlled in the GUI.
struct Settings
{
	Settings() :
		viewCenter(0.0f, 20.0f),
		hz(60.0f),
		velocityIterations(3),
		positionIterations(3),
		drawShapes(0),
		drawParticles(1),
		drawJoints(1),
		drawAABBs(0),
		drawPairs(0),
		drawContactPoints(0),
		drawContactNormals(0),
		drawContactForces(0),
		drawFrictionForces(0),
		drawCOMs(0),
		drawStats(0),
		drawProfile(0),
		enableWarmStarting(1),
		enableContinuous(1),
		enableSubStepping(0),
		pause(0),
		singleStep(0)
		{}

	b2Vec2 viewCenter;
	float32 hz;
	int32 velocityIterations;
	int32 positionIterations;
	int32 drawShapes;
	int32 drawParticles;
	int32 drawJoints;
	int32 drawAABBs;
	int32 drawPairs;
	int32 drawContactPoints;
	int32 drawContactNormals;
	int32 drawContactForces;
	int32 drawFrictionForces;
	int32 drawCOMs;
	int32 drawStats;
	int32 drawProfile;
	int32 enableWarmStarting;
	int32 enableContinuous;
	int32 enableSubStepping;
	int32 pause;
	int32 singleStep;
	int32 strictContacts;
};

struct TestEntry
{
	char *name;
	TestCreateFcn *createFcn;
};

extern TestEntry g_testEntries[];

// This is called when a joint in the world is implicitly destroyed
// because an attached body is destroyed. This gives us a chance to
// nullify the mouse joint.
class DestructionListener : public b2DestructionListener
{
public:
	void SayGoodbye(b2Fixture* fixture) { B2_NOT_USED(fixture); }
	void SayGoodbye(b2Joint* joint);

	Test* test;
};

const int32 k_maxContactPoints = 2048;

struct ContactPoint
{
	b2Fixture* fixtureA;
	b2Fixture* fixtureB;
	b2Vec2 normal;
	b2Vec2 position;
	b2PointState state;
};

class Test : public b2ContactListener
{
public:

	b2Vec2 mouse;
	float leftBorder, rightBorder;
	std::string type;										//这决定了你的editor的类别
	int mainAlive, attackMainAlive;
	double totalCost;

	Test();
	virtual ~Test();

	void SetTextLine(int32 line) { m_textLine = line; }
	void DrawTitle(int x, int y, char * string, int lineMax = 30);
	virtual void Step(Settings* settings);
	virtual void Step2(Settings* settings);
	virtual void Keyboarding();
	virtual void Keyboard(unsigned char key) { B2_NOT_USED(key); }
	virtual void KeyboardUp(unsigned char key) { B2_NOT_USED(key); }
	void ShiftMouseDown(const b2Vec2& p);
	virtual void MouseDown(const b2Vec2& p);
	virtual void MouseUp(const b2Vec2& p);
	virtual void MouseMove(const b2Vec2& p);
	virtual void MouseMaintain(const b2Vec2& p);
	virtual void EnterFrame();

	void Test::InitializeParticleParameters(const uint32 filterMask);
	void Test::ColorParticleGroup(b2ParticleGroup * const group,
		uint32 particlesPerColor);
	float32 Test::GetDefaultViewZoom() const;
	void Test::ShiftOrigin(const b2Vec2& newOrigin);
	void Test::RestoreParticleParameters();

	void LaunchBomb();
	void LaunchBomb(const b2Vec2& position, const b2Vec2& velocity);
	
	void SpawnBomb(const b2Vec2& worldPt);
	void CompleteBombSpawn(const b2Vec2& p);

	// Let derived tests know that a joint was destroyed.
	virtual void JointDestroyed(b2Joint* joint) { B2_NOT_USED(joint); }

	// Callbacks for derived classes.
	virtual void BeginContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void EndContact(b2Contact* contact) { B2_NOT_USED(contact); }
	virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	virtual void PostSolve(const b2Contact* contact, const b2ContactImpulse* impulse)
	{
		B2_NOT_USED(contact);
		B2_NOT_USED(impulse);
	}

	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;
	friend class Raffica;

	bool inputing;
	b2Body* m_groundBody;
	b2ParticleSystem* m_particleSystem;
	b2AABB m_worldAABB;
	ContactPoint m_points[k_maxContactPoints];
	int32 m_pointCount;
	DestructionListener m_destructionListener;
	DebugDraw m_debugDraw;
	int32 m_textLine;
	b2World* m_world;
	b2Body* m_bomb;
	b2MouseJoint* m_mouseJoint;
	b2Vec2 m_bombSpawnPoint;
	bool m_bombSpawning;
	b2Vec2 m_mouseWorld;
	int32 m_stepCount;


protected:
	friend class DestructionListener;
	friend class BoundaryListener;
	friend class ContactListener;

	bool m_mouseTracing;
	b2Vec2 m_mouseTracerPosition;
	b2Vec2 m_mouseTracerVelocity;


	ParticleParameter::Value* m_particleParameters;
	ParticleParameter::Definition m_particleParameterDef;

	static const b2ParticleColor k_ParticleColors[];
	static const uint32 k_ParticleColorsCount;
	b2Profile m_maxProfile;
	b2Profile m_totalProfile;
};

#endif
