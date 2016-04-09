
class Terrain : public Procedural { public:

	Terrain(float importance,int width,int height,int res_x,int res_y,int res_z) 
	{
		this->importance=importance;
		//map_in.init(width,height);
		map_out.init(width,height);
		map_binary.init(width,height);

		Bmp color_texture("../data/textures/terrain/up.jpg");

		int i,j;

		loop(i,0,width)
		loop(j,0,height)
		{
			float a=float(i)/float(width-1);
			float b=float(j)/float(height-1);

			vec3f p=trunk_p_out(a,b);
			vec3f n=trunk_n_out(a,b);
			vec3f c(
				float(color_texture.data[(i+j*width)*3+2])/255.0f,
				float(color_texture.data[(i+j*width)*3+1])/255.0f,
				float(color_texture.data[(i+j*width)*3+0])/255.0f
				);			//=trunk_c_out(a,b);

			float hole = 0;/* 
				cos(2*M_PI*b)+
				cos(2*M_PI*a)+
				cos(5*M_PI*b)+
				cos(5*M_PI*a);*/

			map_out.set_pixel(i,j,p,n,c);
			p.y-=10.00;
			n.y=-n.y;
		//	map_in.set_pixel(i,j,p,n,c);
			map_binary.set_pixel(i,j,(hole<-0.7)? 0:1);
		}

		//map_in.gen_textures();
		map_out.gen_textures();
		//map_in.tex_scale.y=1;
		//map_in.tex_scale.z=1;
		map_out.tex_scale.y=1;
		map_out.tex_scale.z=1;

		// generate vbos
		//gen_geometry(res_x,res_y,res_z);
		gen_geometry_borderless(res_x,res_y);
		//gen_geometry_borderless_strip(res_x,res_y);	
		
		//gen_geometry(res_x,res_y,1);
	}

	vec3f trunk_p_out(float a, float b,float scale=1.0f)
	{
		int width=map_out.width;
		float displace=//get_liana(a+b*3,0.1)*15+
			0.05*(
			sin(2*M_PI*b)+
			sin(2*M_PI*a)+
			sin(8*M_PI*b)*0.2+
			sin(8*M_PI*a)*0.2 
			);

		vec3f p;
		p.x= a*140;// cos(2*M_PI*a)*displace*get_radius(b);
		p.z= b*140;// sin(2*M_PI*a)*displace*get_radius(b);
		p.y= displace*140;//
		
		return p;
	}

	vec3f trunk_n_out(float a, float b)
	{
		vec3f p =trunk_p_out(a,b);
		vec3f dx=trunk_p_out(a+0.02,b)-p;
		vec3f dy=trunk_p_out(a,b+0.02)-p;
		vec3f n=dx;
		n.cross(dx,dy);
		n.normalize();

		return n;
	}
};