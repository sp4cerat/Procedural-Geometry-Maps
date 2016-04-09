struct GeometryVBO
{
	std::vector<Vertex> v;	// 1 vertices / triangle-strip
							// 3 vertices / triangle
							// 4 vertices / quad
	int handle;
	int type;				// 3=triangles, 4=quads
	int gl_type;			// GL_PATCHES / GL_TRIANGLE_STRIP
	
	bool valid(){return v.size()>0;}

	void gen_vbo(int vbo_type)
	{
		if(!valid())return;

		this->type=vbo_type;

		if (type==1) gl_type=GL_TRIANGLE_STRIP; else gl_type=GL_PATCHES;

		printf("GeometryVBO gen_vbo start\n");
		glGenBuffers(1, (GLuint *)(&handle));
		glBindBuffer(GL_ARRAY_BUFFER, handle);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex)*v.size(),&v[0], GL_DYNAMIC_DRAW_ARB );
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		printf("GeometryVBO gen_vbo done, %d primitives type %d\n",v.size()/type,type);
	}
	void draw(int repeated=0)
	{
		if(!valid())return;
		if(repeated>0)
		{
			glDrawArrays( gl_type, 0, v.size()); 
			return;
		}

		// Enable VBO
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, handle);	//		CHECK_GL_ERROR();
		glVertexPointer  ( 3, GL_FLOAT,0, (char *) 0);	//	CHECK_GL_ERROR();

		glPatchParameteri(GL_PATCH_VERTICES, type);		//		CHECK_GL_ERROR();
		glDrawArrays( gl_type, 0, v.size());			//		CHECK_GL_ERROR();
		//vertexshader: gl_InstanceID (int)
		//glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);	CHECK_GL_ERROR();
	}
	void draw_quads(int repeated=0)
	{
		if(!valid())return;
		if(repeated>0){glDrawArrays( GL_QUADS, 0, v.size()); return;}

		// Enable VBO
		glBindBufferARB(GL_ARRAY_BUFFER_ARB, handle);	//		CHECK_GL_ERROR();
		glVertexPointer  ( 3, GL_FLOAT,0, (char *) 0);	//	CHECK_GL_ERROR();

		glDrawArrays( GL_QUADS, 0, v.size());			//		CHECK_GL_ERROR();
	}
};
