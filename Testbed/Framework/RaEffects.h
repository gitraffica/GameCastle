#pragma once
using namespace std;

class RaEffect {
	
public:

	static void explosive(b2Vec2 pos, float strength, vector<int>* textureIDXs, int32 particleCNT, float sharp = 1);

	static void smoke(b2Vec2 pos, float strength, vector<int>* tex, int32 particleCNT = 5);

	static void blow(b2Vec2 pos, b2Vec2 velocity, float strength, vector<int>* tex, int32 particleCNT = 5, float sharp = 1);

};