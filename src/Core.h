#pragma once
////////////////////////////////////////////////////////////////////////////////
#define _USE_MATH_DEFINES
#include <iostream> 
#include <vector> 
#include <string> 
#include <stdio.h>
#include <glew.h>
#include <wglew.h>
#include <GL/glut.h>// Header File For The GLUT Library 
#include <time.h>
#include <mmsystem.h>
#include <windows.h>
using namespace std;
#include "VecMath.h"
#include "glsl.h"
#include "opengl_FBO.h"
///////////////////////////////////////////
#include "Bmp.h"
#include "Bmp.h"
///////////////////////////////////////////
typedef float vec3_t[3];
#include "md5/md5load.h"
////////////////////////////////////////////////////////////////////////////////
#define SCREEN_SIZE_X 1024
#define SCREEN_SIZE_Y 768
////////////////////////////////////////////////////////////////////////////////
#define _USE_MATH_DEFINES
#define loopi(start_l,end_l,step_l) for ( int i=start_l;i<end_l;i+=step_l )
#define loopi(start_l,end_l) for ( int i=start_l;i<end_l;++i )
#define loopj(start_l,end_l,step_l) for ( int j=start_l;j<end_l;j+=step_l )
#define loopj(start_l,end_l) for ( int j=start_l;j<end_l;++j )
#define loopk(start_l,end_l,step_l) for ( int k=start_l;k<end_l;k+=step_l )
#define loopk(start_l,end_l) for ( int k=start_l;k<end_l;++k )
#define loop(a_l,start_l,end_l) for ( a_l = start_l;a_l<end_l;++a_l )
#define loops(a_l,start_l,end_l,step_l) for ( a_l = start_l;a_l<end_l;a_l+=step_l )

#ifndef byte
#define byte unsigned char
#endif

#ifndef ushort
#define ushort unsigned short
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef uchar
#define uchar unsigned char
#endif

////////////////////////////////////////////////////////////////////////////////
class Keyboard
{
	public:

	bool  key [256]; // actual
	bool  key2[256]; // before

	Keyboard(){ int a; loop(a,0,256) key[a] = key2[a]=0; }

	bool KeyDn(char a)//key down
	{
		return key[a];
	}
	bool KeyPr(char a)//pressed
	{
		return ((!key2[a]) && key[a] );
	}
	bool KeyUp(char a)//released
	{
		return ((!key[a]) && key2[a] );
	}
	void update()
	{
		int a;loop( a,0,256 ) key2[a] = key[a];
	}
};
////////////////////////////////////////////////////////////////////////////////
class Mouse
{
	public:

	bool  button[256];
	bool  button2[256];
	float mouseX,mouseY;
	float mouseX2,mouseY2;
	float mouseDX,mouseDY;

	Mouse()
	{ 
		int a; loop(a,0,256) button[a] = button2[a]=0; 
		mouseX=mouseY=mouseDX=mouseDY= 0;
	}
	void update()
	{
		mouseDX=mouseX-mouseX2;mouseX2=mouseX;
		mouseDY=mouseY-mouseY2;mouseY2=mouseY;
		int a;loop( a,0,256 ) button2[a] = button[a];
	}
};
////////////////////////////////////////////////////////////////////////////////
class ScreenA
{
	public:
	ScreenA()
	{
		pos=vec3f(0,0,-2);
		rot=vec3f(0,0,0);
	};

	int	 window_width;
	int	 window_height;
	bool fullscreen;

	matrix44 camera;

	vec3f pos,rot;
};
////////////////////////////////////////////////////////////////////////////////
void CoreInit(void (GLUTCALLBACK *drawFunc)(void),int argc,char **argv);
GLuint CoreNewFloat16Tex(int width,int height,float* buffer,bool alpha);
GLuint CoreNewChar8Tex(int width,int height,uchar* buffer,bool alpha);
void CoreKeyMouse();
////////////////////////////////////////////////////////////////////////////////
extern Keyboard		keyboard;
extern Mouse		mouse;
extern ScreenA		screen;
////////////////////////////////////////////////////////////////////////////////
