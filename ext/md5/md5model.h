/*
 * md5model.h -- md5mesh model loader + animation
 * last modification: aug. 14, 2007
 *
 * Copyright (c) 2005-2007 David HENRY
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * gcc -Wall -ansi -lGL -lGLU -lglut md5anim.c md5anim.c -o md5model
 */

#ifndef __MD5MODEL_H__
#define __MD5MODEL_H__

#include "md5load.h"



		/**
		 * Quaternion prototypes
		 */
		void Quat_computeW (quat4_t q);
		void Quat_normalize (quat4_t q);
		void Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out);
		void Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out);
		void Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out);
		float Quat_dotProduct (const quat4_t qa, const quat4_t qb);
		void Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out);

		
		/**
		 * md5mesh prototypes
		 */
		int ReadMD5Model (const char *filename, struct md5_model_t *mdl);
		void PrepareMesh (const struct md5_mesh_t *mesh,const struct md5_joint_t *skeleton);
		void AllocVertexArrays ();
		void FreeVertexArrays ();
		void DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints);

		//textures
		//GLuint loadTexture(char *fileName);

		/**
		 * md5anim prototypes
		 */
		int CheckAnimValidity (const struct md5_model_t *mdl,
					   const struct md5_anim_t *anim);
		int ReadMD5Anim (const char *filename, struct md5_anim_t *anim);
		void FreeAnim (struct md5_anim_t *anim);
		void InterpolateSkeletons (const struct md5_joint_t *skelA,
					   const struct md5_joint_t *skelB,
					   int num_joints, float interp,
					   struct md5_joint_t *out);
		void Animate (const struct md5_anim_t *anim,
				  struct anim_info_t *animInfo, double dt);




#endif /* __MD5MODEL_H__ */

