/////////////////////////////////////////////
// This source is licensed under MIT license
/////////////////////////////////////////////
#include "Core.h"
///////////////////////////////////////////
#include "Procedural.h"
#include "Procedural_Terrain.h"
#include "Procedural_Trunk.h"
#include "Procedural_Leaf.h"
#include "Procedural_Leaf_Trunk.h"
///////////////////////////////////////////
void DrawScene()
{
	glViewport(0,0,screen.window_width,screen.window_height);
	glClearDepth(1.0f);
	glClearColor(
		129.0f/255.0f, 
		182.0f/255.0f, 
		255.0f/255.0f, 
		0.0f);

	static Terrain		terrain		(0.5, 1024,1024,16,16 ,1);
	static Trunk		trunk		(1.0, 1024,1024,4,8 ,1);
	static Leaf			leaf		(1.0, 512,512,1,1,1);
	static LeafTrunk	leaftrunk	(1.0, 64,64,1,4,&leaf);

	glClearDepth(1.0f);
	glClearColor(
		129.0f/255.0f, 
		182.0f/255.0f, 
		255.0f/255.0f, 
		0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLfloat)screen.window_width / (GLfloat) screen.window_height, 1, 10000.0);
	glMatrixMode( GL_MODELVIEW);
	glLoadIdentity();
	
	// Position
	glRotatef(screen.rot.z, 0,0,1);
	glRotatef(screen.rot.y, 1,0,0);
	glRotatef(screen.rot.x, 0,1,0);
	glTranslatef(
		screen.pos.x,
		screen.pos.y,
		screen.pos.z
		);
	float Projection[16];
	float Modelview[16];
	glGetFloatv(GL_PROJECTION_MATRIX, Projection);
	glGetFloatv(GL_MODELVIEW_MATRIX, Modelview);

	matrix44 m_model,m_proj;
	m_proj.set(Projection);
	m_model.set(Modelview);

	screen.camera=m_proj*m_model;
	screen.camera.transpose();

	static std::vector<matrix44> trunk_matrices;
	static std::vector<matrix44> leaf_matrices;
	static std::vector<matrix44> terrain_matrices;
	static std::vector<matrix44> trunk_matrices_mvp;
	static std::vector<matrix44> leaf_matrices_mvp;
	static std::vector<matrix44> terrain_matrices_mvp;

	if(trunk_matrices.size()==0)
	{
		terrain_matrices.clear();
		trunk_matrices.clear();
		leaf_matrices.clear();
		int res=5;

		loopi(-res,res)
		loopj(-res,res)
		{
			matrix44 m;
			m.ident();
			m.set_translation(vector3(i*140,0,j*140));
			trunk_matrices.push_back(m);
			terrain_matrices.push_back(m);

			loopk(0,32)
			{
				matrix44 m;
				m.ident();
				vec3f c(i*140,0,j*140);
				float b=float(k)/32.0f;//c.y/(15*20);
				c=c+trunk.get_displace_xyz(b);
				float s=1.5f*trunk.get_radius(b)/trunk.get_radius(0);
				m.scale(vector3(s,s,s));
				m.translate(vector3(trunk.get_radius(b)*0.75,0,0));
				m.rotate_y(33*trunk.get_angle(b)+k*k*10);
				m.set_translation(vector3(c.x,c.y,c.z));
				leaf_matrices.push_back(m);
			}
		}
		terrain_matrices_mvp=terrain_matrices;
		trunk_matrices_mvp=trunk_matrices;
		leaf_matrices_mvp=leaf_matrices;
	}

	loopi(0,terrain_matrices.size()) terrain_matrices_mvp[i]=terrain_matrices[i]*screen.camera;
	loopi(0,trunk_matrices.size()) trunk_matrices_mvp[i]=trunk_matrices[i]*screen.camera;
	loopi(0,leaf_matrices.size()) leaf_matrices_mvp[i]=leaf_matrices[i]*screen.camera;

	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

	terrain.Draw(terrain_matrices_mvp);
	trunk.Draw(trunk_matrices_mvp);
	leaf.Draw(leaf_matrices_mvp);
	leaftrunk.Draw(leaf_matrices_mvp);

	CoreKeyMouse();
	glutSwapBuffers();
}
///////////////////////////////////////////
int main(int argc, char **argv) 
{ 
	CoreInit(DrawScene,argc, argv);
	glutMainLoop(); 
}
///////////////////////////////////////////
