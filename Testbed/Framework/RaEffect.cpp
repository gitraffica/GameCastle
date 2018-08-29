#include <Box2D/Box2D.h>
#include <vector>
#include <iostream>
#include "RaEffects.h"
#include "RaEasy.h"

using namespace std;

/*RaEffect*/

void RaEffect::explosive(b2Vec2 pos, float strength, vector<int>* textureIDXs, int32 particleCNT, float sharp) {

	b2Vec2 velocity;

	for (int i = 0; i < particleCNT; i++) {
		velocity.x = rand()*(rand()*strength * 2 / 32767 - strength)/32767;
		velocity.y = rand()*(rand()*strength * 2 / 32767 - strength) / 32767;

		b2Body* par = Raffica::createBall(pos.x, pos.y, log(strength)/3, 3, .3f, .2f, 0, 1u, 65535u, false);
		ExplosiveEffectUserData* ud = new ExplosiveEffectUserData(log(strength)*3);
		ud->target = par;
		ud->sharp = sharp;
		Raffica::setUserDataByTexture(par, ud, 1, log(strength)/3*2, log(strength) / 3*2);
		
		ud->textureIndexs = textureIDXs;
		par->SetLinearVelocity(velocity);
	}

}

void RaEffect::smoke(b2Vec2 pos, float strength, vector<int>* textureIDXs, int32 particleCNT) {

	b2Vec2 velocity;

	for (int i = 0; i < particleCNT; i++) {
		velocity.x = rand()*(rand()*strength / 32767 - strength/2) / 32767;
		velocity.y = rand()*(rand()*strength / 32767) / 32767;

		b2Body* par = Raffica::createBall(pos.x, pos.y, log(strength), .3f, .2f, 0, 0, 0, 0);
		ExplosiveEffectUserData* ud = new ExplosiveEffectUserData(sqrt(strength)*3);
		Raffica::setUserDataByTexture(par, ud, 1, sqrt(strength)/2 , sqrt(strength) /2);
		ud->textureIndexs = textureIDXs;
		par->SetLinearVelocity(velocity);
	}

}

void RaEffect::blow(b2Vec2 pos, b2Vec2 ve, float strength, vector<int>* textureIDXs, int32 particleCNT, float sharp) {

	b2Vec2 velocity;

	for (int i = 0; i < particleCNT; i++) {
		velocity.x = rand()*(rand()*strength / 32767 - strength / 2) / 32767 + ve.x;
		velocity.y = rand()*(rand()*strength / 32767 - strength / 2) / 32767 + ve.y;

		b2Body* par = Raffica::createBall(pos.x, pos.y, sqrt(strength)/4, .3f, .2f, 0, 0, 65535u, 65535u);
		ExplosiveEffectUserData* ud = new ExplosiveEffectUserData(sqrt(strength));
		Raffica::setUserDataByTexture(par, ud, 1, sqrt(strength) / 2, sqrt(strength) / 2);
		ud->sharp = sharp;
		if (sharp < 0.01f) {
			b2Filter aa;
			aa.categoryBits = 0;
			aa.maskBits = 0;
			par->GetFixtureList()->SetFilterData(aa);
		}
		ud->textureIndexs = textureIDXs;
		par->SetLinearVelocity(velocity);
	}

}