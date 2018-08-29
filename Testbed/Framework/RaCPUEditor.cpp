#include "RaCPUEditor.h"
#include "Test.h"
#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include "RaEasy.h"
using namespace std;

void RaCPUEditor::SetTree(float x, float sz) {
	float width = sz;
	float height = sz * 3;
	float stemWidth = sz / 4;
	float stemHeight = sz * 1.5f;

	Raffica::createBoxByMaterial(b2Vec2(x - stemWidth / 2, 0), b2Vec2(x + stemWidth / 2, stemHeight), "wood");
}

void RaCPUEditor::BuildCityGate(Test* test) {

	freopen("info.txt", "w", stdout);
	cout << ' ';
	Raffica::loadWorld(test, "info.txt");
	freopen("CON", "w", stdout);

	float gateLeft, gateRight, gateHeight;
	float gateWidth, gateTopWidth;

	gateLeft = -25 + rand() * 14.0f / 32767 - 7;
	gateRight = 25 + rand() * 14.0f / 32767 - 7;
	gateHeight = 45 + rand() * 14.0f / 32767 - 7;
	gateWidth = 15 + rand() * 1.0f / 32767 - 0.5f;
	gateTopWidth = 15 + rand() * 1.0f / 32767 - 0.5f;

	Raffica::createBoxByMaterial(b2Vec2(gateLeft - gateWidth / 2, 0), b2Vec2(gateLeft + gateWidth / 2, gateHeight), "iron");
	Raffica::createBoxByMaterial(b2Vec2(gateRight - gateWidth / 2, 0), b2Vec2(gateRight + gateWidth / 2, gateHeight), "iron");
	b2Body* b = Raffica::createBoxByMaterial(b2Vec2(gateLeft + gateWidth / 2, gateHeight-gateTopWidth), b2Vec2(gateRight - gateWidth / 2, gateHeight), "iron");
	vector<int> tex;
	tex.push_back(LoadTexture("pic/HDD.bmp"));
	((BreakableUserData*)b->GetUserData())->textureIndexs = &tex;
	((BreakableUserData*)b->GetUserData())->tileX = 0;
	Raffica::createDisJointByAnc(b2Vec2(gateLeft + gateWidth / 2 - 3, gateHeight - 3), b2Vec2(gateLeft + gateWidth / 2 + 3, gateHeight - 3), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateLeft + gateWidth / 2 - 3, gateHeight - gateTopWidth + 3), b2Vec2(gateLeft + gateWidth / 2 + 3, gateHeight - gateTopWidth + 3), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateRight - gateWidth / 2 - 3, gateHeight - 3), b2Vec2(gateRight - gateWidth / 2 + 3, gateHeight - 3), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateRight - gateWidth / 2 - 3, gateHeight - gateTopWidth + 3), b2Vec2(gateRight - gateWidth / 2 + 3, gateHeight - gateTopWidth + 3), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateLeft - gateWidth / 2 + 3, 2), b2Vec2(gateLeft - gateWidth / 2 + 3, -2), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateLeft + gateWidth / 2 - 3, 2), b2Vec2(gateLeft + gateWidth / 2 - 3, -2), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateRight - gateWidth / 2 + 3, 2), b2Vec2(gateRight - gateWidth / 2 + 3, -2), 0.5, "pic/material/steelstick.png");
	Raffica::createDisJointByAnc(b2Vec2(gateRight + gateWidth / 2 - 3, 2), b2Vec2(gateRight + gateWidth / 2 - 3, -2), 0.5, "pic/material/steelstick.png");

	SetTree(40, 10);

	Raffica::savenowWorld("info.txt");

}