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

* copyright...co你个头，我照样用XD
* 改造：Raffica

*/

#ifdef __APPLE__
	#include <GLUT/glut.h>
#else
	#include "freeglut\freeglut.h"
#endif
#pragma once

// We need these 4 from glext.h, and define them here rather than relying on
// the header, which is not universally available.
#ifndef GL_POINT_SPRITE
#define GL_POINT_SPRITE                   0x8861
#endif
#ifndef GL_COORD_REPLACE
#define GL_COORD_REPLACE                  0x8862
#endif
#ifndef GL_GENERATE_MIPMAP
#define GL_GENERATE_MIPMAP                0x8191
#endif
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE                  0x812F
#endif

#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "Test.h"
#include "RaRules.h"
#include "RaEasy.h"
#include <atlimage.h>

using namespace std;

/**********************************************/

map<string, int> fileIndex;
map<int, string> indexFile;
GLuint texture_ID = 0;

float currentscale = 1;	// amount of pixels that corresponds to one world unit, needed to use glPointSize correctly

int DebugDraw::lists;
float DebugDraw::textB = 0.8f, DebugDraw::textR = 0.7f, DebugDraw::textG = 0.7f;

/**********************************************/

void DebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
}

void DebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glTexCoord2f(vertices[i].x, vertices[i].y); glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
	glDisable(GL_BLEND);

	glColor4f(color.r, color.g, color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
}

void DebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color)
{
	float32 k_segments = 16.0f;
	if (radius < 3.0f) k_segments = 4;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertex2f(v.x, v.y);
		theta += k_increment;
	}
	glEnd();
}

void DebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color)
{
	const float32 k_segments = 16.0f;
	const float32 k_increment = 2.0f * b2_pi / k_segments;
	float32 theta = 0.0f;
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4f(0.5f * color.r, 0.5f * color.g, 0.5f * color.b, 0.5f);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertex2f(v.x, v.y);
		theta += k_increment;
	}
	glEnd();
	glDisable(GL_BLEND);

	theta = 0.0f;
	glColor4f(color.r, color.g, color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < k_segments; ++i)
	{
		b2Vec2 v = center + radius * b2Vec2(cosf(theta), sinf(theta));
		glVertex2f(v.x, v.y);
		theta += k_increment;
	}
	glEnd();

	b2Vec2 p = center + radius * axis;
	glBegin(GL_LINES);
	glVertex2f(center.x, center.y);
	glVertex2f(p.x, p.y);
	glEnd();
}

void DebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
{
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINES);
	glVertex2f(p1.x, p1.y);
	glVertex2f(p2.x, p2.y);
	glEnd();
}

void DebugDraw::DrawTransform(const b2Transform& xf)
{
	b2Vec2 p1 = xf.p, p2;
	const float32 k_axisScale = 0.4f;
	glBegin(GL_LINES);
	
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.q.GetXAxis();
	glVertex2f(p2.x, p2.y);

	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex2f(p1.x, p1.y);
	p2 = p1 + k_axisScale * xf.q.GetYAxis();
	glVertex2f(p2.x, p2.y);

	glEnd();
}

void DebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color)
{
	glPointSize(size);
	glBegin(GL_POINTS);
	glColor3f(color.r, color.g, color.b);
	glVertex2f(p.x, p.y);
	glEnd();
	glPointSize(1.0f);
}

void DebugDraw::selectFont(int size, int charset, const char* face) {  
    HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,  
        charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,  
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);  
    HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);  
    DeleteObject(hOldFont);
	static int isFirstCall = 1;

	if (isFirstCall) {
		isFirstCall = 0;
		// 申请MAX_CHAR个连续的显示列表编号  
		lists = glGenLists(128);
		// 把每个字符的绘制命令都装到对应的显示列表中  
		wglUseFontBitmaps(wglGetCurrentDC(), 0, 1386, lists);
	}
}

void DebugDraw::DrawString(int x, int y, char *string, int lineMax, ...) {

	//const int lineMax = 30;

	va_list arg;
	va_start(arg, string);
	va_end(arg);
	///////////
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	int w = glutGet(GLUT_WINDOW_WIDTH);
	int h = glutGet(GLUT_WINDOW_HEIGHT);
	gluOrtho2D(0, w, h, 0);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glColor3f(textR, textG, textB);
	glRasterPos2i(x, y);
	int32 length = (int32)strlen(string);
	
	int len, i;
	vector<wchar_t> wstring;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	// 计算字符的个数
	// 如果是双字节字符的（比如中文字符），两个字节才算一个字符
	// 否则一个字节算一个字符
	len = 0;

	int lines = 0, start = 0;

	for (i = start; string[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(string[i]))
			++i;
		++len;
	}
	
	// 将混合字符转化为宽字符
	wstring.resize(len+1);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, string + start, -1, &wstring[0], len);
	wstring[len] = L'\0';

	int nowline = 0;
	
	for (i = 0; i < len; ++i, ++nowline) {
		if (wstring[i] > 128) ++nowline;
		if (wstring[i] == L'\r' || nowline > lineMax) {
			lines++, nowline = 0;
		}
	}
	nowline = 0;
	for (i = 0; i < wstring.size() && lines > 7; ++i, ++nowline) {
		if (wstring[i] > 128) ++nowline;
		if (wstring[i] == L'\r' || nowline > lineMax) {
			for (int j = i + 1; j < wstring.size(); j++) {
				wstring[j - i - 1] = wstring[j];
			}
			wstring.resize(wstring.size() - i - 1);
			lines--;
			i = 0;
			nowline = 0;
		}
	}
	len = wstring.size();
	
	// 逐个输出字符

	for (i = 0; i<len; ++i) {
		nowline++;
		if (wstring[i] > 128) ++nowline;
		//cout << string[i];
		if (wstring[i] == L'\r' || nowline > lineMax) {
			nowline = 0;
			y += 20;
			glRasterPos2i(x, y);
			if(wstring[i] == L'\r') continue;
		}
		wglUseFontBitmapsW(hDC, wstring[i], 1, list);
		glCallList(list);
	}
	//cout << endl;

	// 回收所有临时资源
	glDeleteLists(list, 1);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}

void DebugDraw::DrawAABB(b2AABB* aabb, const b2Color& c)
{
	glColor3f(c.r, c.g, c.b);
	glBegin(GL_LINE_LOOP);
	glVertex2f(aabb->lowerBound.x, aabb->lowerBound.y);
	glVertex2f(aabb->upperBound.x, aabb->lowerBound.y);
	glVertex2f(aabb->upperBound.x, aabb->upperBound.y);
	glVertex2f(aabb->lowerBound.x, aabb->upperBound.y);
	glEnd();
}


////////////////////

#define BMP_Header_Length 54
/* 函数power_of_two
* 检查一个整数是否为2的整数次方，如果是，返回1，否则返回0
* 实际上只要查看其二进制位中有多少个，如果正好有1个，返回1，否则返回0
* 在“查看其二进制位中有多少个”时使用了一个小技巧
* 使用n &= (n-1)可以使得n中的减少一个（具体原理大家可以自己思考）
*/
int power_of_two(int n)
{
    if( n <= 0 )
        return 0;
    return (n & (n-1)) == 0;
}

/* 函数load_texture
* 读取一个BMP文件作为纹理  
* 如果失败，返回0，如果成功，返回纹理编号
*/
unsigned int LoadTexture(const char* file_name) {
	if(fileIndex.find(string(file_name)) != fileIndex.end()) {
		return fileIndex.find(string(file_name))->second;
	}
	cout << file_name << ' ';
	fileIndex[string(file_name)] = ++ texture_ID;
	indexFile[texture_ID] = string(file_name);
	GLubyte* pixels = 0;

	
	 BITMAP bm;  
    GLuint idTexture = 0;  
    CImage img;             //需要头文件atlimage.h  
    HRESULT hr = img.Load(file_name);  
    if ( !SUCCEEDED(hr) )   //文件加载失败  
    {
        MessageBox(NULL, ("文件" + string(file_name) + "加载失败").c_str(), "ERROR", 0);
		assert(0);
        return NULL;  
    }
    HBITMAP hbmp = img;  
    if(!GetObject(hbmp, sizeof(bm), &bm))  
        return 0;  
  
    //glGenTextures(1, &idTexture);  
     // 分配一个新的纹理编号
     glGenTextures(1, &texture_ID);


    if( texture_ID == 0 )
     {
        free(pixels);
        return 0;
     }
	
	int tmp = (int) texture_ID;

     // 绑定新的纹理，载入纹理并设置纹理参数
     // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
     glGetIntegerv(GL_TEXTURE_BINDING_2D, &tmp);
     glBindTexture(GL_TEXTURE_2D, texture_ID); 
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
	 //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
         //GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
	 string filename = file_name;
	 if(filename.find(".png") != string::npos) {
		 cout << "png";
		 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, bm.bmWidth, bm.bmHeight, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, bm.bmBits); //这里不是GL_RGB  
	 }
	 else {
		 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bm.bmWidth, bm.bmHeight, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, bm.bmBits); //这里不是GL_RGB  
	 }
     // 之前为pixels分配的内存可在使用glTexImage2D以后释放	
     // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
    free(pixels);
	cout << "finished\n";
    return texture_ID;
}

float smoothstep(float x) { return x * x * (3 - 2 * x); }

void DebugDraw::DrawParticles(const b2Vec2 *centers, float32 radius, const b2ParticleColor *colors, int32 count)
{
	radius *= 4;

	static unsigned int particle_texture = 0;

	if (!particle_texture ||
		!glIsTexture(particle_texture)) // Android deletes textures upon sleep etc.
	{
		// generate a "gaussian blob" texture procedurally
		glGenTextures(1, &particle_texture);
		b2Assert(particle_texture);
		const int TSIZE = 64;
		unsigned char tex[TSIZE][TSIZE][4];
		for (int y = 0; y < TSIZE; y++)
		{
			for (int x = 0; x < TSIZE; x++)
			{
				float fx = (x + 0.5f) / TSIZE * 2 - 1;
				float fy = (y + 0.5f) / TSIZE * 2 - 1;
				float dist = sqrtf(fx * fx + fy * fy);
				unsigned char intensity = (unsigned char)(dist <= 1 ? smoothstep(1 - dist) * 255 : 0);
				tex[y][x][0] = tex[y][x][1] = tex[y][x][2] = 128;
				tex[y][x][3] = intensity;
			}
		}
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, particle_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, TSIZE, TSIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex);
		glDisable(GL_TEXTURE_2D);

		glEnable(GL_POINT_SMOOTH);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, particle_texture);

#ifdef __ANDROID__
	glEnable(GL_POINT_SPRITE_OES);
	glTexEnvf(GL_POINT_SPRITE_OES, GL_COORD_REPLACE_OES, GL_TRUE);
#else
	glEnable(GL_POINT_SPRITE);
	glTexEnvi(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);
#endif

	const float particle_size_multiplier = 3;  // because of falloff
	glPointSize(radius * currentscale * particle_size_multiplier);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, &centers[0].x);
	if (colors)
	{
		glEnableClientState(GL_COLOR_ARRAY);
		glColorPointer(4, GL_UNSIGNED_BYTE, 0, &colors[0].r);
	} else
	{
		glColor4f(1, 1, 1, 1);
	}

	glDrawArrays(GL_POINTS, 0, count);

	glDisableClientState(GL_VERTEX_ARRAY);
	if (colors) glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
#ifdef __ANDROID__
	glDisable(GL_POINT_SPRITE_OES);
#endif
}


void DebugDraw::DrawFlatPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	glColor4f(color.r, color.g, color.b, 1);
	glBegin(GL_TRIANGLE_FAN);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
}

void Display() {
	unsigned int i;
	glEnable(GL_TEXTURE_2D);
	for(i = 0;i < Raffica::userdataList.size();i ++) {
		vector<BasicUserData*>& tmp = Raffica::userdataList;
		if(!tmp[i]) continue;
		tmp[i]->Display();
	}
	glDisable(GL_TEXTURE_2D);
}

void postDisplay() {
	unsigned int i;
	glEnable(GL_TEXTURE_2D);
	for (i = 0; i < Raffica::userdataList.size(); i++) {
		vector<BasicUserData*>& tmp = Raffica::userdataList;
		if (!tmp[i]) continue;
		if (!tmp[i]->isButton) continue;
		tmp[i]->Display();
	}
	glDisable(GL_TEXTURE_2D);
}

////////////////////







//GLint width, height;
//GLint total_bytes;
/*
// 打开文件，如果失败，返回
FILE* pFile = fopen(file_name, "rb");
if( pFile == 0 )
return 0;

// 读取文件中图象的宽度和高度
fseek(pFile, 0x0012, SEEK_SET);
fread(&width, 4, 1, pFile);
fread(&height, 4, 1, pFile);
fseek(pFile, BMP_Header_Length, SEEK_SET);

// 计算每行像素所占字节数，并根据此数据计算总像素字节数
{
GLint line_bytes = width * 3;
while( line_bytes % 4 != 0 )
++line_bytes;
total_bytes = line_bytes * height;
}

// 根据总像素字节数分配内存
pixels = (GLubyte*)malloc(total_bytes);
if( pixels == 0 )
{
fclose(pFile);
return 0;
}

// 读取像素数据
if( fread(pixels, total_bytes, 1, pFile) <= 0 )
{
free(pixels);
fclose(pFile);
return 0;
}

// 在旧版本的OpenGL中
// 如果图象的宽度和高度不是的整数次方，则需要进行缩放
// 这里并没有检查OpenGL版本，出于对版本兼容性的考虑，按旧版本处理
// 另外，无论是旧版本还是新版本，
// 当图象的宽度和高度超过当前OpenGL实现所支持的最大值时，也要进行缩放
{
GLint max;
glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max);
if( !power_of_two(width)
|| !power_of_two(height)
|| width > max
|| height > max )
{
GLint new_width = 256;
GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
GLint new_line_bytes, new_total_bytes;
GLubyte*& new_pixels = pixels;

// 计算每行需要的字节数和总字节数
new_line_bytes = new_width * 3;
while( new_line_bytes % 4 != 0 )
++new_line_bytes;
new_total_bytes = new_line_bytes * new_height;

// 分配内存
//new_pixels = pixels;
new_width = width;
new_height = height;
// 进行像素缩放

// 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
//free(pixels);
//pixels = new_pixels;
width = new_width;
height = new_height;
}

}*/