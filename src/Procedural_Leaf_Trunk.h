
class LeafTrunk : public Procedural { public:

	Bmp *bmp_height;
	Leaf*leaf;

	LeafTrunk(float importance,int width,int height,int res_x,int res_y,Leaf* leaf) 
	{
		this->importance=importance;

		map_out.init(width,height);
		this->leaf=leaf;

		int i,j;
		loop(i,0,width)
		loop(j,0,height)
		{
			float a=float(i)/float(width-1);
			float b=float(j)/float(height-1);
			vec3f p1=p_trunk_out(a,b);
			vec3f n1=n_trunk_out(a,b);
			vec3f c1=vec3f(96.0/256.0,204.0/256.0,0);
			map_out.set_pixel(i,j,p1,n1,c1,1);
		}
		map_out.gen_textures();
		gen_geometry_borderless(res_x,res_y);
		//gen_geometry_borderless_strip(res_x,res_y);
	}

	vec3f p_trunk_out(float a, float b)
	{
		a/=2;
		vec3f p;//(0,0,b*5-5);
		p.x=30*a;
		p.z= sin(2*M_PI*(b+0.5))*1.8*(0.5-a);
		p.y= cos(2*M_PI*(b+0.5))*1.8*(0.5-a);
		p.y+=leaf->heightmap(a,0.5)-leaf->heightmap(0,0.5);
		return p;
	}
	vec3f n_trunk_out(float a, float b)
	{
		vec3f p =p_trunk_out(a,b);
		vec3f dx=p_trunk_out(a+0.02,b)-p;
		vec3f dy=p_trunk_out(a,b+0.02)-p;
		vec3f n=dx;
		n.cross(dx,dy);
		n.z= -cos(2*M_PI*(b+0.5));
		n.y=  sin(2*M_PI*(b+0.5));
		n.normalize();
		return n;
	}
};