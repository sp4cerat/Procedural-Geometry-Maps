struct GeometryMap
{
	std::vector<float> vextex_map;	// 4 float / pixel , xyz1
	std::vector<float> normal_map;	// 4 float / pixel , nxnynz1
	std::vector<uchar>	color_map;	// 4 float / pixel , nxnynz1
	int texture_vertex;
	int texture_normal;
	int texture_color;
	int width,height;
	vec3f tex_scale;

	bool valid(){ return (vextex_map.size()>0); };

	void init(int width,int height)
	{
		this->width=width;
		this->height=height;
		vextex_map.resize(width*height*4);
		normal_map.resize(width*height*4);
		color_map.resize(width*height*4);
		tex_scale.x=1;
		tex_scale.y=1;
		tex_scale.z=1;
	}

	void save_vertex_map_to_bmp(char* name)
	{
		Bmp bmp(width,height,24,0);

		vec3f pmin;
		pmin.x=vextex_map[0];
		pmin.y=vextex_map[1];
		pmin.z=vextex_map[2];
		vec3f pmax=pmin;

		loopi(0,width)
		loopj(0,height)
		{
			int o = (i+j*width)*4;
			float x=vextex_map[o+0];
			float y=vextex_map[o+1];
			float z=vextex_map[o+2];
			if(x<pmin.x)pmin.x=x;
			if(y<pmin.y)pmin.y=y;
			if(z<pmin.z)pmin.z=z;
			if(x>pmax.x)pmax.x=x;
			if(y>pmax.y)pmax.y=y;
			if(z>pmax.z)pmax.z=z;
		}
		loopi(0,width)
		loopj(0,height)
		{
			int o = (i+j*width);
			bmp.data[o*3+0]=(vextex_map[o*4+0]-pmin.x)*255/(pmax.x-pmin.x);
			bmp.data[o*3+1]=(vextex_map[o*4+1]-pmin.x)*255/(pmax.x-pmin.x);
			bmp.data[o*3+2]=(vextex_map[o*4+2]-pmin.x)*255/(pmax.x-pmin.x);
		}
		bmp.save(name);
	}

	void set_pixel(int i,int j,vec3f p,vec3f n,vec3f c,float alpha=1)
	{
		if(!valid())return;
		int o = (i+j*width)*4;
		vextex_map[o+0]=p.x*1 ;
		vextex_map[o+1]=p.y*1 ;
		vextex_map[o+2]=p.z*1 ;
		vextex_map[o+3]=1   ;

		normal_map[o+0]=n.x*1 ;
		normal_map[o+1]=n.y*1 ;
		normal_map[o+2]=n.z*1 ;
		normal_map[o+3]=1   ;

		color_map[o+0]=c.z*255 ;
		color_map[o+1]=c.y*255 ;
		color_map[o+2]=c.x*255 ;
		color_map[o+3]=alpha*255   ;
	}
	void gen_textures()
	{
		if(!valid())return;
		
		texture_vertex = CoreNewFloat16Tex(width,height,&vextex_map[0],true);
		texture_normal = CoreNewFloat16Tex(width,height,&normal_map[0],true);
		texture_color  = CoreNewChar8Tex(width,height,&color_map[0],true);
	}
};

struct BinaryMap
{
	std::vector<uchar>	map_binary;
	int width,height;

	bool valid(){ return (map_binary.size()>0); };

	void init(int width,int height)
	{
		this->width=width;
		this->height=height;
		map_binary.resize(width*height,1);
	}
	uchar get_pixel(float a,float b,float am,float bm)
	{
		if(!valid())return 1;
		int x= float(a/am)*float(width);
		int y= float(b/bm)*float(height);
		x=x%width;
		y=y%height;
		return map_binary[y*width+x];
	}
	uchar get_region_max(float a1,float b1,float a2,float b2)
	{
		if(!valid())return 1;

		int x1= float(a1)*float(width);	
		int y1= float(b1)*float(height);	
		int x2= float(a2)*float(width);	
		int y2= float(b2)*float(height);	

		int i,j;
		
		loop(i,x1,x2)
		loop(j,y1,y2)
		{
			int x=i%width;
			int y=j%height;
			int m=map_binary[y*width+x];
			if (m>0)return 1;
		}
		return 0;
	}
	void set_pixel(int x, int y,uchar v)
	{
		if(!valid())return;
		map_binary[y*width+x]=v;
	}
};

