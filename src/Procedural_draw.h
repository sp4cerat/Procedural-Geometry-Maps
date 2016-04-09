void proc_draw(float importance,std::vector<matrix44> &mvp_matrices,GeometryVBO& vbo, GeometryMap& map,bool cull_invert=false)
{
	glCullFace( cull_invert ? GL_BACK : GL_FRONT); 

	glActiveTextureARB( GL_TEXTURE0 );glBindTexture(GL_TEXTURE_2D, map.texture_vertex );
	glActiveTextureARB( GL_TEXTURE1 );glBindTexture(GL_TEXTURE_2D, map.texture_normal );
	glActiveTextureARB( GL_TEXTURE2 );glBindTexture(GL_TEXTURE_2D, map.texture_color );

	if(vbo.type==3)
	{
		// Triangle
		static Shader tri_shader("../shader/geo_tri");
		tri_shader.begin();
		tri_shader.setUniform1i("geomap",0);
		tri_shader.setUniform1i("normalmap",1);
		tri_shader.setUniform1i("colormap",2);
		tri_shader.setUniform1f("importance",importance);
		tri_shader.setUniform3f("tex_scale",map.tex_scale.x,map.tex_scale.y,map.tex_scale.z);
		//tri_shader.setUniform1f("cull_invert",cull_invert ? -1 : 1);
		//tri_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&m_mvp.m[0][0]);	CHECK_GL_ERROR();

		int i;loop(i,0,mvp_matrices.size()) 
		{
			tri_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&mvp_matrices[i].m[0][0]);
			vbo.draw(i);
		}

		tri_shader.end();
	}

	if(vbo.type==4)
	{
		// Quad
		static Shader quad_shader("../shader/geo_quad");
//		static Shader quad_shader("../shader/geo_quad_simple");
		quad_shader.begin();
		quad_shader.setUniform1i("geomap",0);
		quad_shader.setUniform1i("normalmap",1);
		quad_shader.setUniform1i("colormap",2);
		quad_shader.setUniform1f("importance",importance);
		quad_shader.setUniform3f("tex_scale",map.tex_scale.x,map.tex_scale.y,map.tex_scale.z);
		//quad_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&m_mvp.m[0][0]);	CHECK_GL_ERROR();
		//quad_shader.setUniform1f("cull_invert",cull_invert ? -1 : 1);
		//quad_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&m_mvp.m[0][0]);	CHECK_GL_ERROR();
		
		loopi(0,mvp_matrices.size()) 
		{
			static GLint loc = quad_shader.get_loc("mvp_matrix");
			glUniformMatrix4fv(loc, 1, 0, (float*)&mvp_matrices[i].m[0][0]);
			
			vbo.draw(i);
		}

		quad_shader.end();
	}

	if(vbo.type==1)
	{
		// Triangle Strip
		static Shader quad_shader("../shader/geo_quad_simple");
		quad_shader.begin();
		quad_shader.setUniform1i("geomap",0);
		quad_shader.setUniform1i("normalmap",1);
		quad_shader.setUniform1i("colormap",2);
		quad_shader.setUniform3f("tex_scale",map.tex_scale.x,map.tex_scale.y,map.tex_scale.z);
		
		int i;loop(i,0,mvp_matrices.size()) 
		{
			static GLint loc = quad_shader.get_loc("mvp_matrix");
			glUniformMatrix4fv(loc, 1, 0, (float*)&mvp_matrices[i].m[0][0]);
			vbo.draw(i);
		}

		quad_shader.end();
	}
}

void proc_draw_holes(float importance,std::vector<matrix44> &mvp_matrices,GeometryVBO& vbo, GeometryMap& map_in,GeometryMap& map_out)
{
	if(vbo.type!=4) return;

	glCullFace( GL_BACK ); 

	glActiveTextureARB( GL_TEXTURE0 );glBindTexture(GL_TEXTURE_2D, map_out.texture_vertex );
	glActiveTextureARB( GL_TEXTURE1 );glBindTexture(GL_TEXTURE_2D, map_in.texture_vertex );
	glActiveTextureARB( GL_TEXTURE2 );glBindTexture(GL_TEXTURE_2D, map_out.texture_normal );
	glActiveTextureARB( GL_TEXTURE3 );glBindTexture(GL_TEXTURE_2D, map_in.texture_normal );
	glActiveTextureARB( GL_TEXTURE4 );glBindTexture(GL_TEXTURE_2D, map_out.texture_color  );
	glActiveTextureARB( GL_TEXTURE5 );glBindTexture(GL_TEXTURE_2D, map_in.texture_color  );

	// Hole Quads
	static Shader quad_shader("../shader/geo_quad_holes");
	quad_shader.begin();
	quad_shader.setUniform1i("geomap_out",0);
	quad_shader.setUniform1i("geomap_in",1);
	quad_shader.setUniform1i("normalmap_out",2);
	//quad_shader.setUniform1i("normalmap_in",3);
	quad_shader.setUniform1i("colormap_out",4);
	quad_shader.setUniform1f("importance",importance);
		quad_shader.setUniform3f("tex_scale",map_in.tex_scale.x,map_in.tex_scale.y,map_in.tex_scale.z);
	//quad_shader.setUniform1i("colormap_in",5);
	//quad_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&m_mvp.m[0][0]);	CHECK_GL_ERROR();

	int i;loop(i,0,mvp_matrices.size()) 
	{
		quad_shader.setUniformMatrix4fv("mvp_matrix", 1, 0, (float*)&mvp_matrices[i].m[0][0]);
		vbo.draw(i);
	}

	quad_shader.end();

	glActiveTextureARB( GL_TEXTURE2 );glBindTexture(GL_TEXTURE_2D, 0 );
	glActiveTextureARB( GL_TEXTURE1 );glBindTexture(GL_TEXTURE_2D, 0 );
	glActiveTextureARB( GL_TEXTURE0 );glBindTexture(GL_TEXTURE_2D, 0 );
}


