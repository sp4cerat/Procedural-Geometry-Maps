### Procedural-Geometry-Maps

#### License : MIT
#### http://opensource.org/licenses/MIT

The Maps are generated procedurally. They contain X,Y,Z coordinates, which are used for the visualization with the Tessellation Shader. To be able to have holes too, they can also be generated with and optional 2D marching quads method.

![Screenshot1](https://raw.githubusercontent.com/sp4cerat/Procedural-Geometry-Maps/master/screenshots/wire.png)

![Screenshot1](https://raw.githubusercontent.com/sp4cerat/Procedural-Geometry-Maps/master/screenshots/solid.png)

Here the code of how the bean trunk is generated:

    
    class Trunk : public Procedural { public:
    
        Trunk(float importance,int width,int height,int res_x,int res_y,int res_z) 
        {
            this->importance=importance;
            map_out.init(width,height);
    
            int i,j;
    
            loop(i,0,width)
            loop(j,0,height)
            {
                float a=float(i)/float(width-1);
                float b=float(j)/float(height-1);
    
                vec3f p=trunk_p_out(a,b);
                vec3f n=trunk_n_out(a,b);
                vec3f c(0,0,0);
                map_out.set_pixel(i,j,p,n,c);
            }
    
            map_out.save_vertex_map_to_bmp("../test.bmp");
    
            Bmp test("../data/textures/trunk/up.jpg");
            loopi(0,width)
            loopj(0,height)
            {
                map_out.color_map[(i+j*width)*4+0]=test.data[(i+j*width)*3+0];
                map_out.color_map[(i+j*width)*4+1]=test.data[(i+j*width)*3+1];
                map_out.color_map[(i+j*width)*4+2]=test.data[(i+j*width)*3+2];
            }
            map_out.gen_textures();
            map_out.tex_scale.y=4;
            map_out.tex_scale.z=8;
    
            // generate vbos
            //gen_geometry(res_x,res_y,res_z);
            gen_geometry_borderless(res_x,res_y);
            //gen_geometry_borderless_strip(res_x,res_y);    
        }
    
        vec3f get_displace_xyz(float b)
        {
            vec3f d;
            d.x=15*sin(2*M_PI*b*2);
            d.y=b*300.0;
            d.z=15*cos(2*M_PI*b*2);
            return d;
        }
    
        float get_angle(float b)
        {
            return b*5;
        }
    
        float get_radius(float b)
        {
            return 0.3*(1-b*b)*15 ;
        }
    
        float get_liana(float a,float scale=0.1f)
        {
            a=a-float(int(a));
            if(a>scale*2)return 0;
            float d=fabs(a-scale);
            return sqrt(scale*scale-d*d);
        }
    
        float get_liana2(float a,float scale=0.1f)
        {
            a=a-float(int(a));
            if(a>scale)return 0;
            if(a<0)return 0;
            return a/scale;
        }
    
        float get_max(float a,float b){ return (a>b) ? a:b;}
    
        vec3f trunk_p_out(float a, float b,float scale=1.0f)
        {
            int width=map_out.width;
    
            static bool init=true;
            static std::vector<float> radius;
            if(init)
            {
                radius.resize(width*3);
    
                int i,j,j_max=width;
                loop(i,0,3)
                {
                    vec3f p1(1,0,0),p2(1,0,0);
                    float a1=0+i*120;    p1.rot_z(a1*2*M_PI/360.0f);
                    float a2=(i+1)*120;    p2.rot_z(a2*2*M_PI/360.0f);
                    vec3f center=(p1+p2)*0.5;                
                    a1=-30+i*120;        a1 = a1*2*M_PI/360.0f;
                    a2=-30+i*120+180;    a2 = a2*2*M_PI/360.0f;
    
                    loop(j,0,j_max)
                    {
                        vec3f p(1,0,0);
                        p.rot_z(a1+(a2-a1)*float(j)/float(j_max));
                        p=p+center;
                        radius[i*j_max+j]=p.length()*p.length();
                    }
                }
                init=false;
            }
            float float_ofs=a+get_angle(b);
            int radius_ofs = int(float(float_ofs*float(width*3)))%(width*3);
    
            float displace=0;
    
            displace+=radius[radius_ofs];
            displace+=get_liana(a+b*3,0.1)*15+
                0.1+0.2*(
                sin(2*3*2*M_PI*b)+
                cos(2*M_PI*(a+2*b))*1.2+
                cos(2*M_PI*(2*3*b+2*a))*1.2+
                sin(4*M_PI*(a-6*b))*0.2+
                cos(3*M_PI*(a*4))*0.2+
                sin(1+32*M_PI*(2*a-b))*0.1+
                cos(2+32*M_PI*(2*a+b))*0.1
                );
    
            vec3f p;
            p.x=  cos(2*M_PI*a)*displace*get_radius(b);
            p.z=  sin(2*M_PI*a)*displace*get_radius(b);
            p.y= 0;//
            
            p=p+get_displace_xyz(b);
    
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
