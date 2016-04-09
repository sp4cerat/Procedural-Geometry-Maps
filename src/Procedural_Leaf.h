
class Leaf : public Procedural { public:

	Bmp *bmp_height;

	Leaf(float importance,int width,int height,int res_x,int res_y,int res_z) 
	{
		this->importance=importance;

		int border=50;
		Bmp bmp_out("../data/textures/leaf/up.jpg");
		Bmp bmp_in("../data/textures/leaf/dn.jpg");
		Bmp bmp_height("../data/textures/leaf/stencil.png");
		bmp_height.make_border(border,255,0,255);
		this->bmp_height=&bmp_height;
		
		Bmp bmp_stencil("../data/textures/leaf/stencil.png");

		map_out.init(width,height);
		map_in.init(width,height);
		map_binary.init(width,height);

		int i,j;

		loop(i,0,width)
		loop(j,0,height)
		{
			float a=float(i)/float(width);
			float b=float(j)/float(height);

			vec3f stencil=bmp_stencil.get_pixel(a,b);

			float alpha=1;

			if (stencil.x>=0.99)
			if (stencil.y<=0.01)
			if (stencil.z>=0.99)
			{
				alpha=0;
			}

			vec3f p1,p2,n1,n2,c1,c2;

			p1=p_out(a,b);
			n1=n_out(a,b);
			c1=bmp_out.get_pixel(a,b);

			p2=p_in(a,b);
			n2=n_in(a,b);
			c2=bmp_in.get_pixel(a,b);

			p1.z-=0.27;
			p2.z-=0.27;

			map_out.set_pixel(i,j,p1,n1,c1,alpha);
			map_in.set_pixel(i,j,p2,n2,c2,alpha);
			map_binary.set_pixel(i,j,alpha);
		}
		
		map_out.gen_textures();
		map_in.gen_textures();

		// generate vbos
		//gen_geometry(res_x,res_y,1);
		gen_geometry_borderless(res_x,res_y);
		//gen_geometry_borderless_strip(res_x,res_y);	
	}

	float heightmap(float a,float b)
	{
		float d=2*fabs(b-0.5);
		float 
		h=sin(a*M_PI)+2*sin(b*M_PI);
		h+=d*sin(8*a*M_PI)*0.5;//+sin(b*M_PI);
		h-=2*a*a;
		return h*3;
	}

	float border(float a,float b)
	{
		float bo=bmp_height->get_pixel(a,b).y;
		bo=bo-0.25;//-0.5;
		if(bo<0)bo=0;
		return bo*0.3;
	}

	vec3f p_out(float a, float b)
	{
		vec3f p(a*30,0,(b-0.5)*30);
		p.y+=heightmap(a,b)-heightmap(0,0.5);
		return p;
	}

	vec3f p_in(float a, float b)
	{
		vec3f p=p_out(a,b);
		p.y-=border(a,b);
		return p;
	}

	vec3f n_out(float a, float b)
	{
		vec3f p =p_out(a,b);
		vec3f dx=p_out(a+0.02,b)-p;
		vec3f dy=p_out(a,b+0.02)-p;
		vec3f n=dx;
		n.cross(dx,dy);
		n.normalize();
		return n;
	}

	vec3f n_in(float a, float b)
	{
		vec3f p =p_in(a,b);
		vec3f dx=p_in(a+0.02,b)-p;
		vec3f dy=p_in(a,b+0.02)-p;
		vec3f n=dx;
		n.cross(dx,dy);
		n.normalize();
		return n;
	}
};