/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
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

/*
* Author: Box2D的testbed
* 
* 请自由编辑，只要不出现bug
*/

#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <Box2D/Box2D.h>
#include <iostream>

struct b2AABB;

// This class implements debug drawing callbacks that are invoked
// inside b2World::Step.
class DebugDraw : public b2Draw
{
public:
	static int lists;
	static float textR, textG, textB;						//文本颜色的RGB

	void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color);

	void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color);

	void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color);

	void DrawTransform(const b2Transform& xf);

    void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color);

    void DrawAABB(b2AABB* aabb, const b2Color& color);

	void DrawParticles(const b2Vec2 *centers, float32 radius, const b2ParticleColor *colors, int32 count);

	void DrawFlatPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);

	static void selectFont(int size, int charset, const char* face);

	void DrawString(int x, int y, char * string, int lineMax = 30, ...);

};

//Author: raffica
unsigned int LoadTexture(const char* file_name);
void Display();
void postDisplay();



#endif