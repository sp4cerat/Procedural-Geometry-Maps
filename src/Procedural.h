#include "Procedural_tables.h"

struct Vertex{ float u,v,w;} ;

#include "Procedural_map.h"
#include "Procedural_vbo.h"
#include "Procedural_draw.h"

class Procedural
{
public:

	float				importance; // detail level weight
	GeometryMap			map_in;
	GeometryMap			map_out;
	BinaryMap			map_binary;

private:

	GeometryVBO vbo_quads_holes;
	GeometryVBO vbo_quads;
	GeometryVBO vbo_triangles;
	GeometryVBO vbo_strips;

	bool no_border;

public:

	void Draw(std::vector<matrix44> &mvp_matrices)
	{
		glEnableClientState(GL_VERTEX_ARRAY);		CHECK_GL_ERROR();

		// Draw outside
		if(map_out.valid()) 
		{
			if (vbo_triangles.valid()) 
				proc_draw(importance,mvp_matrices,vbo_triangles,map_out,false);

			if (vbo_quads.valid()) 
				proc_draw(importance,mvp_matrices,vbo_quads,map_out,false);

			if (vbo_strips.valid()) 
				proc_draw(importance,mvp_matrices,vbo_strips,map_out,false);
		}

		// Draw inside
		if(map_in.valid() ) 
		{
			if (vbo_triangles.valid()) 
				proc_draw(importance,mvp_matrices,vbo_triangles,map_in,true);

			if (vbo_quads.valid()) 
				proc_draw(importance,mvp_matrices,vbo_quads,map_in,true);

			if (vbo_strips.valid()) 
				proc_draw(importance,mvp_matrices,vbo_strips,map_in,true);
		}

		// Draw border
		if(map_out.valid() && map_in.valid() ) 
		if (vbo_quads_holes.valid()) 
			proc_draw_holes(importance,mvp_matrices,vbo_quads_holes, map_in,map_out);
		
		glActiveTextureARB( GL_TEXTURE2 );glBindTexture(GL_TEXTURE_2D, 0 );
		glActiveTextureARB( GL_TEXTURE1 );glBindTexture(GL_TEXTURE_2D, 0 );
		glActiveTextureARB( GL_TEXTURE0 );glBindTexture(GL_TEXTURE_2D, 0 );

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);	CHECK_GL_ERROR();
		glDisableClientState(GL_VERTEX_ARRAY);		CHECK_GL_ERROR();
	}

	void gen_geometry_borderless(int res_i = 70,int res_j = 70)
	{
		int i,j;
		for (i=0;i<res_i;i++)
		{
			float a1=float(i+0)/float(res_i);
			float a2=float(i+1)/float(res_i);

			for (int j=0;j<res_j;j++)
			{
				float b1=float(j+0)/float(res_j); 
				float b2=float(j+1)/float(res_j); 

				int map = map_binary.get_region_max(a1,b1,a2,b2);

				if (map>0)
				{
					Vertex v;
					
					v.u=a2;v.v=b1;v.w=1;vbo_quads.v.push_back(v);
					v.u=a2;v.v=b2;v.w=1;vbo_quads.v.push_back(v);
					v.u=a1;v.v=b2;v.w=1;vbo_quads.v.push_back(v);
					v.u=a1;v.v=b1;v.w=1;vbo_quads.v.push_back(v);
				}
			}
		}
		vbo_quads.gen_vbo(4);
	}

	void gen_geometry_borderless_strip(int res_i = 70,int res_j = 70)
	{
		int i,j;

		for (i=0;i<res_i;i++)
		{
			float a1=float(i+0)/float(res_i);
			float a2=float(i+1)/float(res_i);

			int map2=0;
			int map1=0;

			for (int j=0;j<=res_j;j++)
			{
				float b1=float(j+0)/float(res_j); 
				float b2=float(j+1)/float(res_j); 

				map2 = map1;
				map1 = map_binary.get_region_max(a1,b1,a2,b2);

				if (map1!=0)
				{
					Vertex v;

					v.u=a1;v.v=b1;v.w=1;vbo_strips.v.push_back(v);

					if (map2!=0)
					{
						vbo_strips.v.push_back(v);
					}

					v.u=a2;v.v=b1;v.w=1;vbo_strips.v.push_back(v);					
				}
				else
				if (map2!=0)
				{
					Vertex v=vbo_strips.v[vbo_strips.v.size()-1];
					vbo_strips.v.push_back(v);
				}
			}
		}
		vbo_strips.gen_vbo(1);
	}

	void gen_geometry(int res_i = 70,int res_j = 70,int res_k = 1)
	{
		if(res_k==0)
		{
			gen_geometry_borderless(res_i,res_j);
		}

		int i,j,k;
		// main
		for (i=0;i<res_i;i++)
		{
			float a1=float(i+0); 
			float a2=float(i+1); 

			// map to radial

			for (int j=0;j<res_j;j++)
			{
				float b1=float(j+0); 
				float b2=float(j+1); 

				int map   = map_binary.get_pixel(a1,b1,res_i,res_j) *1+
							map_binary.get_pixel(a2,b1,res_i,res_j) *2+
							map_binary.get_pixel(a1,b2,res_i,res_j) *4+
							map_binary.get_pixel(a2,b2,res_i,res_j) *8;

				int* quads = ms_idx[map];

				vec3f vlisto[16];
				vec3f vlisti[16];

				for (int t=1;t<=quads[0]*3;t++)
				{
					int index=quads[t];
					vec3f p = ms_p[index];

					if (p.x==0.5)
					{
						// interpolate						
						float x1=a1;int fx1=map_binary.get_pixel(a1,b1+p.y,res_i,res_j);
						float x2=a2;int fx2=map_binary.get_pixel(a2,b1+p.y,res_i,res_j);

						float dx=0.25;
						for (int bs=0;bs<4;bs++,dx/=2)
						{
							int fx=map_binary.get_pixel(p.x+i,b1+p.y,res_i,res_j);
							if (fx==fx1) p.x+=dx; else p.x-=dx;
						}
					}

					if (p.y==0.5)
					{
						// interpolate
						float y1=b1;int fy1=map_binary.get_pixel(a1+p.x,b1,res_i,res_j);
						float y2=b2;int fy2=map_binary.get_pixel(a1+p.x,b2,res_i,res_j);

						float dy=0.25;
						for (int bs=0;bs<4;bs++,dy/=2)
						{
							int fy=map_binary.get_pixel(a1+p.x, p.y+j,res_i,res_j);
							if (fy==fy1) p.y+=dy; else p.y-=dy;
						}
					}

					p.x+=i;
					p.y+=j;

					Vertex v;
					v.u=p.x/float(res_i);
					v.v=p.y/float(res_j);
					v.w=1;
					vbo_triangles.v.push_back(v);

					vlisto[t] = vec3f(v.u,v.v,1);
					vlisti[t] = vec3f(v.u,v.v,0);
				}//t

				int edge_start = quads[0]*3+2;
				int edge_num   = quads[edge_start-1]*2;
				int edge_end   = edge_num+edge_start;

				//poly_count+=edge_num;
				
				for (int t=edge_start;t<edge_end;t+=2)
				{
					vec3f p0=vlisto[quads[t+0]];
					vec3f p1=vlisto[quads[t+1]];
					vec3f p2=vlisti[quads[t+1]];
					vec3f p3=vlisti[quads[t+0]];

					vec3f d0=p3-p0;
					vec3f d1=p2-p1;

					for (int k=0;k<res_k;k++)
					{
						float a0 = float(k  )/float(res_k);
						float a1 = float(k+1)/float(res_k);
						vec3f q0 = p0+d0*a0;
						vec3f q1 = p1+d1*a0;
						vec3f q2 = p1+d1*a1;
						vec3f q3 = p0+d0*a1;

						Vertex v;
						v.u=q0.x;v.v=q0.y;v.w=q0.z; vbo_quads_holes.v.push_back(v);
						v.u=q1.x;v.v=q1.y;v.w=q1.z; vbo_quads_holes.v.push_back(v);
						v.u=q2.x;v.v=q2.y;v.w=q2.z; vbo_quads_holes.v.push_back(v);
						v.u=q3.x;v.v=q3.y;v.w=q3.z; vbo_quads_holes.v.push_back(v);

						//printf("v %f %f %f\n",v.u,v.v,v.w);

					}//k
				}//t
			}//j
		}//i
		
		vbo_triangles.gen_vbo(3);

		if (vbo_quads_holes.valid())
		{
			vbo_quads_holes.gen_vbo(4);
		}
	}
};


