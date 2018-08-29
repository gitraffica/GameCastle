#pragma once
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;
#define WindowWidth   600
#define WindowHeight 600
#define WindowTitle  "OpenGL纹理测试"
#include "freeglut/freeglut.h"
//#include <gl/glut.h>

/* 函数grab2
* 抓取窗口中的像素
* 假设窗口宽度为WindowWidth，高度为WindowHeight
*/
#define BMP_Header_Length 54
void grab2(void)
{
     FILE*     pDummyFile;
     FILE*     pWritingFile;
     GLubyte* pPixelData;
     GLubyte   BMP_Header[BMP_Header_Length];
     GLint     i, j;
     GLint     PixelDataLength;

     // 计算像素数据的实际长度
     i = WindowWidth * 3;    // 得到每一行的像素数据长度
    while( i%4 != 0 )       // 补充数据，直到i是的倍数
         ++i;                // 本来还有更快的算法，
                            // 但这里仅追求直观，对速度没有太高要求
     PixelDataLength = i * WindowHeight;

     // 分配内存和打开文件
     pPixelData = (GLubyte*)malloc(PixelDataLength);
    if( pPixelData == 0 )
        exit(0);

     pDummyFile = fopen("rj.bmp", "rb");
    if( pDummyFile == 0 )
        exit(0);

     pWritingFile = fopen("grab2.bmp", "wb");
    if( pWritingFile == 0 )
        exit(0);

     // 读取像素
     glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
     glReadPixels(0, 0, WindowWidth, WindowHeight,
         GL_BGR_EXT, GL_UNSIGNED_BYTE, pPixelData);

     // 把dummy.bmp的文件头复制为新文件的文件头
    fread(BMP_Header, sizeof(BMP_Header), 1, pDummyFile);
    fwrite(BMP_Header, sizeof(BMP_Header), 1, pWritingFile);
    fseek(pWritingFile, 0x0012, SEEK_SET);
     i = WindowWidth;
     j = WindowHeight;
    fwrite(&i, sizeof(i), 1, pWritingFile);
    fwrite(&j, sizeof(j), 1, pWritingFile);

     // 写入像素数据
    fseek(pWritingFile, 0, SEEK_END);
    fwrite(pPixelData, PixelDataLength, 1, pWritingFile);

     // 释放内存和关闭文件
    fclose(pDummyFile);
    fclose(pWritingFile);
    free(pPixelData);
}
/* 函数power_of_two2
* 检查一个整数是否为2的整数次方，如果是，返回1，否则返回0
* 实际上只要查看其二进制位中有多少个，如果正好有1个，返回1，否则返回0
* 在“查看其二进制位中有多少个”时使用了一个小技巧
* 使用n &= (n-1)可以使得n中的减少一个（具体原理大家可以自己思考）
*/
int power_of_two2(int n)
{
    if( n <= 0 )
        return 0;
    return (n & (n-1)) == 0;
}

/* 函数load_texture2
* 读取一个BMP文件作为纹理
* 如果失败，返回0，如果成功，返回纹理编号
*/
GLuint load_texture2(const char* file_name, GLint width, GLint height)
{
     GLint total_bytes;
     GLubyte* pixels = 0;
     GLuint texture_ID = 0;
	 GLint last_texture_ID;

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
        if( !power_of_two2(width)
          || !power_of_two2(height)
          || width > max
          || height > max )
         {
            const GLint new_width = 256;
            const GLint new_height = 256; // 规定缩放后新的大小为边长的正方形
             GLint new_line_bytes, new_total_bytes;
             GLubyte* new_pixels = 0;

             // 计算每行需要的字节数和总字节数
             new_line_bytes = new_width * 3;
            while( new_line_bytes % 4 != 0 )
                 ++new_line_bytes;
             new_total_bytes = new_line_bytes * new_height;

             // 分配内存
             new_pixels = (GLubyte*)malloc(new_total_bytes);
            if( new_pixels == 0 )
             {
                free(pixels);
                fclose(pFile);
                return 0;
             }

             // 进行像素缩放
             gluScaleImage(GL_RGB,
                 width, height, GL_UNSIGNED_BYTE, pixels,
                 new_width, new_height, GL_UNSIGNED_BYTE, new_pixels);

             // 释放原来的像素数据，把pixels指向新的像素数据，并重新设置width和height
            free(pixels);
             pixels = new_pixels;
             width = new_width;
             height = new_height;
         }
     }

     // 分配一个新的纹理编号
     glGenTextures(1, &texture_ID);
    if( texture_ID == 0 )
     {
		 //cout<<"aaaaaaaaaaaaaa";
        free(pixels);
        fclose(pFile);
        return 0;
     }

     // 绑定新的纹理，载入纹理并设置纹理参数
     // 在绑定前，先获得原来绑定的纹理编号，以便在最后进行恢复
     glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture_ID);
     glBindTexture(GL_TEXTURE_2D, texture_ID);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
         GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
     glBindTexture(GL_TEXTURE_2D, last_texture_ID);

     // 之前为pixels分配的内存可在使用glTexImage2D以后释放
     // 因为此时像素数据已经被OpenGL另行保存了一份（可能被保存到专门的图形硬件中）
    free(pixels);
    return texture_ID;
}
/* 两个纹理对象的编号
*/
GLuint texGround2;
GLuint texWall2;
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
const int n = 20;
const GLfloat R1 = 10.0f;
const GLfloat R2 = 10.0f;
const GLfloat R3 = 18.0f;
const GLfloat R4 = 18.0f;
const GLfloat R5 = 5.0f;
const GLfloat R6 = 5.0f;
const GLfloat R7 = 3.0f;
const GLfloat R8 = 3.0f;
const GLfloat Pi = 3.1415926536f;
const GLfloat x11 = 80.0f;
const GLfloat y11 = 130.0f;
const GLfloat x22 = 120.0f;
const GLfloat y22 = 130.0f;
const GLfloat x33 = 80.0f;
const GLfloat y33 = 130.0f;
const GLfloat x44 = 120.0f;
const GLfloat y44 = 130.0f;
const GLfloat x77 = 62.0f;
const GLfloat y77 = 107.0f;
const GLfloat x88 = 138.0f;
const GLfloat y88 = 107.0f;
GLfloat spin = 0;

void init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	gluOrtho2D(0.0, 200.0, 0.0, 200.0);
}

void SpongeBob() {
	int i;
	//glClear(GL_COLOR_BUFFER_BIT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	//eye
	glColor3f(0.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	for (i = 0; i<n; i++)
		glVertex2f(x11 + R1*cos(2 * Pi / n*i), y11 + R1*sin(2 * Pi / n*i));
	glEnd();

	glBegin(GL_POLYGON);//
	for (i = 0; i<n; i++)
		glVertex2f(x22 + R2*cos(2 * Pi / n*i), y22 + R2*sin(2 * Pi / n*i));
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);//
	for (i = 0; i<n; i++)
		glVertex2f(x11 + R5*cos(2 * Pi / n*i), y11 + R5*sin(2 * Pi / n*i));
	glEnd();

	glBegin(GL_POLYGON);//
	for (i = 0; i<n; i++)
		glVertex2f(x22 + R6*cos(2 * Pi / n*i), y22 + R6*sin(2 * Pi / n*i));
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= n; i++)
		glVertex2f(x33 + R3*cos(2 * Pi / n*i), y33 + R3*sin(2 * Pi / n*i));
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (i = 0; i <= n; i++)
		glVertex2f(x44 + R4*cos(2 * Pi / n*i), y44 + R4*sin(2 * Pi / n*i));
	glEnd();

	//body
	glColor3f(0.935, 0.768, 0.533);
	glBegin(GL_LINE_STRIP);
	glVertex2i(.50, 160);
	glVertex2i(.50, 60);
	glVertex2i(.150, 60);
	glVertex2i(.150, 160);
	glVertex2i(.50, 160);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(50, 85);
	glVertex2i(150, 85);
	glEnd();

	//tie
	glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex2i(50, 85);
	glVertex2i(75, 80);
	glVertex2i(100, 85);
	glVertex2i(90, 75);
	glVertex2i(100, 65);
	glVertex2i(110, 75);
	glVertex2i(100, 85);
	glVertex2i(125, 80);
	glVertex2i(150, 85);
	glEnd();

	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_STRIP);
	glVertex2i(50, 70);
	glVertex2i(55, 70);
	glVertex2i(55, 65);
	glVertex2i(50, 65);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2i(75, 70);
	glVertex2i(75, 65);
	glVertex2i(90, 65);
	glVertex2i(90, 70);
	glVertex2i(75, 70);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2i(110, 70);
	glVertex2i(110, 65);
	glVertex2i(125, 65);
	glVertex2i(125, 70);
	glVertex2i(110, 70);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex2i(150, 70);
	glVertex2i(145, 70);
	glVertex2i(145, 65);
	glVertex2i(150, 65);
	glEnd();

		glColor3f(0.935, 0.768, 0.533);
	glBegin(GL_LINE_STRIP);
	glVertex2i(100, 112);
	glVertex2i(105, 112);
	glVertex2i(107, 113);
	glVertex2i(110, 114);
	glVertex2i(112, 115);
	glVertex2i(114, 116);
	glVertex2i(116, 117);
	glVertex2i(118, 115);
	glVertex2i(120, 113);
	glVertex2i(118, 115);
	glVertex2i(120, 113);
	glVertex2i(120, 113);
	glVertex2i(118, 110);
	glVertex2i(116, 109);
	glVertex2i(114, 108);
	glVertex2i(112, 107);
	glVertex2i(108, 107);
	glVertex2i(105, 106);
	glVertex2i(104, 106);
	glVertex2i(100, 105);
	glEnd();

	//mouth
	glColor3f(0.6, 0.0, 0.0);
	glBegin(GL_LINE_STRIP);
	glVertex2i(62, 107);
	glVertex2i(95, 106);
	glVertex2i(100, 105);
	glVertex2i(110, 106);
	glVertex2i(138, 107);
	glEnd();

	glBegin(GL_LINE_STRIP);
	glVertex2i(62, 107);
	glVertex2i(75, 102);
	glVertex2i(90, 92);
	glVertex2i(100, 90);
	glVertex2i(110, 92);
	glVertex2i(125, 102);
	glVertex2i(138, 107);
	glEnd();

		glColor3f(0.961, 0.828, 0.851);
	glBegin(GL_POLYGON);
	for (i = 0; i<n; i++)
		glVertex2f(x77 + R7*cos(2 * Pi / n*i), y77 + R7*sin(2 * Pi / n*i));
	glEnd();

	glBegin(GL_POLYGON);//
	for (i = 0; i<n; i++)
		glVertex2f(x88 + R8*cos(2 * Pi / n*i), y88 + R8*sin(2 * Pi / n*i));
	glEnd();

	//arm_left
	glColor3f(0.935, 0.768, 0.533);
	glBegin(GL_LINE_STRIP);
	glVertex2i(50, 85);
	glVertex2i(40, 85);
	glVertex2i(45, 77);
	glVertex2i(50, 85);
	glEnd();

	//arm_right
	glBegin(GL_LINE_STRIP);
	glVertex2i(150, 85);
	glVertex2i(160, 85);
	glVertex2i(155, 77);
	glVertex2i(150, 85);
	glEnd();

	//leg_left
	glColor3f(0.935, 0.768, 0.533);
	glBegin(GL_LINE_STRIP);
	glVertex2i(80, 60);
	glVertex2i(75, 53);
	glVertex2i(85, 53);
	glVertex2i(80, 60);
	glEnd();

	//leg_right
	glBegin(GL_LINE_STRIP);
	glVertex2i(120, 60);
	glVertex2i(115, 53);
	glVertex2i(125, 53);
	glVertex2i(120, 60);
	glEnd();

	glFlush();
}
void display2(void)
{
	cout<<"aaa";
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glBindTexture(GL_TEXTURE_2D, texGround2);
     /*glBegin(GL_QUADS); 
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-5.0f, -5.0f);
		glTexCoord2f(0.0f, 5.0f); glVertex2f(-5.0f, 5.0f);
		glTexCoord2f(5.0f, 5.0f); glVertex2f(5.0f, 5.0f);
		glTexCoord2f(5.0f, 0.0f); glVertex2f(5.0f, -5.0f);
     glEnd();*/
	 glutSwapBuffers();
}

int main2(int argc, char* argv[])
{
     // GLUT初始化
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
     glutInitWindowPosition(100, 100);
     glutInitWindowSize(WindowWidth, WindowHeight);
     glutCreateWindow(WindowTitle);
     glutDisplayFunc(&display2);

     // 在这里做一些初始化
    // glEnable(GL_TEXTURE_2D);
     //texGround2 = load_texture2("rj.bmp", 10, 10);
     //texWall2 = load_texture2("rj.bmp", 10, 10);

	 //cout << texWall2;
     // 开始显示
     glutMainLoop();

    return 0;
}

void raf(int argc, char* argv[]) {
	main2(argc, argv);
}