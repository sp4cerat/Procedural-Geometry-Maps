
#include "md5load.h"


////////////////////////////////////////////////////////////
// Construction/Destruction
////////////////////////////////////////////////////////////
md5load::md5load()
{
		//variable initialisations
		animated = 0;

		skeleton = NULL;

		/* vertex array related stuff */
		max_verts = 0;
		max_tris = 0;

		vertexArray = NULL;
		vertexIndices = NULL;

		//set up texture loading
		ilInit(); // initialize IL

		drawTexture = true;
		drawSkeleton = false;
		rotate = false;
} // end constructor

void md5load::cleanup()
{
  FreeModel (&md5file);
  FreeAnim (&md5anim);

  if (animated && skeleton)
    {
      free (skeleton);
      skeleton = NULL;
    }

  FreeVertexArrays ();
}

md5load::~md5load()  
{

} // end destructor

void md5load::enableSkeleton(bool skeleton)
{
	drawSkeleton = skeleton;
}

void md5load::enableTextured(bool textured)
{
	drawTexture = textured;
}

void md5load::enableRotate(bool rotated)
{
	rotate = rotated;
}

void md5load::init (const char *filename, const char *animfile, char *texturefile)
{

  modeltexture = loadTexture(texturefile);

  /* Initialize OpenGL context */
  glClearColor (0.5f, 0.5f, 0.5f, 1.0f);
  glShadeModel (GL_SMOOTH);

  glEnable (GL_DEPTH_TEST);

  /* Load MD5 model file */
  if (!ReadMD5Model (filename, &md5file))
    exit (EXIT_FAILURE);

  cout << "Model loaded" << endl;

  AllocVertexArrays ();

  /* Load MD5 animation file */
  if (animfile)
    {
      if (!ReadMD5Anim (animfile, &md5anim))
	{
	  FreeAnim (&md5anim);
	  cout << "Animation not loaded" << endl;
	}
      else
	{
	  animInfo.curr_frame = 0;
	  animInfo.next_frame = 1;

	  animInfo.last_time = 0;
	  animInfo.max_time = 1.0 / md5anim.frameRate;

	  /* Allocate memory for animated skeleton */
	  skeleton = (struct md5_joint_t *)
	    malloc (sizeof (struct md5_joint_t) * md5anim.num_joints);

	  animated = 1;
	  cout << "Animation loaded" << endl;
	}
    }

  if (!animated)
    printf ("init: no animation loaded.\n");
}

void md5load::AllocVertexArrays ()
{
  vertexArray = (vec5_t *)malloc (sizeof (vec5_t) * max_verts);
  vertexIndices = (GLuint *)malloc (sizeof (GLuint) * max_tris * 3);
}

/**
 * Free resources allocated for the animation.
 */
void md5load::FreeAnim (struct md5_anim_t *anim)
{
  int i;

  if (anim->skelFrames)
    {
      for (i = 0; i < anim->num_frames; ++i)
	{
	  if (anim->skelFrames[i])
	    {
	      free (anim->skelFrames[i]);
	      anim->skelFrames[i] = NULL;
	    }
	}

      free (anim->skelFrames);
      anim->skelFrames = NULL;
    }

  if (anim->bboxes)
    {
      free (anim->bboxes);
      anim->bboxes = NULL;
    }
}

/**
 * Load an MD5 model from file.
 */
int md5load::ReadMD5Model (const char *filename, struct md5_model_t *mdl)
{
  FILE *fp;
  char buff[512];
  int version;
  int curr_mesh = 0;
  int i;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "Error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buff, sizeof (buff), fp);

      if (sscanf (buff, " MD5Version %d", &version) == 1)
	{
	  if (version != 10)
	    {
	      /* Bad version */
	      fprintf (stderr, "Error: bad model version\n");
	      fclose (fp);
	      return 0;
	    }
	}
      else if (sscanf (buff, " numJoints %d", &mdl->num_joints) == 1)
	{
	  if (mdl->num_joints > 0)
	    {
	      /* Allocate memory for base skeleton joints */
	      mdl->baseSkel = (struct md5_joint_t *)
		calloc (mdl->num_joints, sizeof (struct md5_joint_t));
	    }
	}
      else if (sscanf (buff, " numMeshes %d", &mdl->num_meshes) == 1)
	{
	  if (mdl->num_meshes > 0)
	    {
	      /* Allocate memory for meshes */
	      mdl->meshes = (struct md5_mesh_t *)
		calloc (mdl->num_meshes, sizeof (struct md5_mesh_t));
	    }
	}
      else if (strncmp (buff, "joints {", 8) == 0)
	{
	  /* Read each joint */
	  for (i = 0; i < mdl->num_joints; ++i)
	    {
	      struct md5_joint_t *joint = &mdl->baseSkel[i];

	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      if (sscanf (buff, "%s %d ( %f %f %f ) ( %f %f %f )",
			  joint->name, &joint->parent, &joint->pos[0],
			  &joint->pos[1], &joint->pos[2], &joint->orient[0],
			  &joint->orient[1], &joint->orient[2]) == 8)
		{
		  /* Compute the w component */
		  Quat_computeW (joint->orient);
		}
	    }
	}
      else if (strncmp (buff, "mesh {", 6) == 0)
	{
	  struct md5_mesh_t *mesh = &mdl->meshes[curr_mesh];
	  int vert_index = 0;
	  int tri_index = 0;
	  int weight_index = 0;
	  float fdata[4];
	  int idata[3];

	  while ((buff[0] != '}') && !feof (fp))
	    {
	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      if (strstr (buff, "shader "))
		{
		  int quote = 0, j = 0;

		  /* Copy the shader name whithout the quote marks */
		  for (i = 0; i < sizeof (buff) && (quote < 2); ++i)
		    {
		      if (buff[i] == '\"')
			quote++;

		      if ((quote == 1) && (buff[i] != '\"'))
			{
			  mesh->shader[j] = buff[i];
			  j++;
			}
		    }
		}
	      else if (sscanf (buff, " numverts %d", &mesh->num_verts) == 1)
		{
		  if (mesh->num_verts > 0)
		    {
		      /* Allocate memory for vertices */
		      mesh->vertices = (struct md5_vertex_t *)
			malloc (sizeof (struct md5_vertex_t) * mesh->num_verts);
		    }

		  if (mesh->num_verts > max_verts)
		    max_verts = mesh->num_verts;
		}
	      else if (sscanf (buff, " numtris %d", &mesh->num_tris) == 1)
		{
		  if (mesh->num_tris > 0)
		    {
		      /* Allocate memory for triangles */
		      mesh->triangles = (struct md5_triangle_t *)
			malloc (sizeof (struct md5_triangle_t) * mesh->num_tris);
		    }

		  if (mesh->num_tris > max_tris)
		    max_tris = mesh->num_tris;
		}
	      else if (sscanf (buff, " numweights %d", &mesh->num_weights) == 1)
		{
		  if (mesh->num_weights > 0)
		    {
		      /* Allocate memory for vertex weights */
		      mesh->weights = (struct md5_weight_t *)
			malloc (sizeof (struct md5_weight_t) * mesh->num_weights);
		    }
		}
	      else if (sscanf (buff, " vert %d ( %f %f ) %d %d", &vert_index,
			       &fdata[0], &fdata[1], &idata[0], &idata[1]) == 5)
		{
		  /* Copy vertex data */
		  mesh->vertices[vert_index].st[0] = fdata[0];
		  mesh->vertices[vert_index].st[1] = fdata[1];
		  mesh->vertices[vert_index].start = idata[0];
		  mesh->vertices[vert_index].count = idata[1];
		  
		  
		 //Texture mapping UV coords are the 2 float values - fdata[0] and fdata[1]
		 //
		  //
		  //
		  //
		  //
		  //
		}
	      else if (sscanf (buff, " tri %d %d %d %d", &tri_index,
			       &idata[0], &idata[1], &idata[2]) == 4)
		{
		  /* Copy triangle data */
		  mesh->triangles[tri_index ].index[0] = idata[0];
		  mesh->triangles[tri_index ].index[1] = idata[1];
		  mesh->triangles[tri_index ].index[2] = idata[2];
		}
	      else if (sscanf (buff, " weight %d %d %f ( %f %f %f )",
			       &weight_index, &idata[0], &fdata[3],
			       &fdata[0], &fdata[1], &fdata[2]) == 6)
		{
		  /* Copy vertex data */
		  mesh->weights[weight_index].joint  = idata[0];
		  mesh->weights[weight_index].bias   = fdata[3];
		  mesh->weights[weight_index].pos[0] = fdata[0];
		  mesh->weights[weight_index].pos[1] = fdata[1];
		  mesh->weights[weight_index].pos[2] = fdata[2];
		}
	    }

	  curr_mesh++;
	}
    }

  fclose (fp);

  return 1;
}

/**
 * Load an MD5 animation from file.
 */
int md5load::ReadMD5Anim (const char *filename, struct md5_anim_t *anim)
{
  FILE *fp = NULL;
  char buff[512];
  struct joint_info_t *jointInfos = NULL;
  struct baseframe_joint_t *baseFrame = NULL;
  float *animFrameData = NULL;
  int version;
  int numAnimatedComponents;
  int frame_index;
  int i;

  fp = fopen (filename, "rb");
  if (!fp)
    {
      fprintf (stderr, "error: couldn't open \"%s\"!\n", filename);
      return 0;
    }

  while (!feof (fp))
    {
      /* Read whole line */
      fgets (buff, sizeof (buff), fp);

      if (sscanf (buff, " MD5Version %d", &version) == 1)
	{
	  if (version != 10)
	    {
	      /* Bad version */
	      fprintf (stderr, "Error: bad animation version\n");
	      fclose (fp);
	      return 0;
	    }
	}
      else if (sscanf (buff, " numFrames %d", &anim->num_frames) == 1)
	{
	  /* Allocate memory for skeleton frames and bounding boxes */
	  if (anim->num_frames > 0)
	    {
	      anim->skelFrames = (struct md5_joint_t **)
		malloc (sizeof (struct md5_joint_t*) * anim->num_frames);
	      anim->bboxes = (struct md5_bbox_t *)
		malloc (sizeof (struct md5_bbox_t) * anim->num_frames);
	    }
	}
      else if (sscanf (buff, " numJoints %d", &anim->num_joints) == 1)
	{
	  if (anim->num_joints > 0)
	    {
	      for (i = 0; i < anim->num_frames; ++i)
		{
		  /* Allocate memory for joints of each frame */
		  anim->skelFrames[i] = (struct md5_joint_t *)
		    malloc (sizeof (struct md5_joint_t) * anim->num_joints);
		}

	      /* Allocate temporary memory for building skeleton frames */
	      jointInfos = (struct joint_info_t *)
		malloc (sizeof (struct joint_info_t) * anim->num_joints);

	      baseFrame = (struct baseframe_joint_t *)
		malloc (sizeof (struct baseframe_joint_t) * anim->num_joints);
	    }
	}
      else if (sscanf (buff, " frameRate %d", &anim->frameRate) == 1)
	{
	  /*
	    printf ("md5anim: animation's frame rate is %d\n", anim->frameRate);
	  */
	}
      else if (sscanf (buff, " numAnimatedComponents %d", &numAnimatedComponents) == 1)
	{
	  if (numAnimatedComponents > 0)
	    {
	      /* Allocate memory for animation frame data */
	      animFrameData = (float *)malloc (sizeof (float) * numAnimatedComponents);
	    }
	}
      else if (strncmp (buff, "hierarchy {", 11) == 0)
	{
	  for (i = 0; i < anim->num_joints; ++i)
	    {
	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      /* Read joint info */
	      sscanf (buff, " %s %d %d %d", jointInfos[i].name, &jointInfos[i].parent,
		      &jointInfos[i].flags, &jointInfos[i].startIndex);
	    }
	}
      else if (strncmp (buff, "bounds {", 8) == 0)
	{
	  for (i = 0; i < anim->num_frames; ++i)
	    {
	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      /* Read bounding box */
	      sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
		      &anim->bboxes[i].min[0], &anim->bboxes[i].min[1],
		      &anim->bboxes[i].min[2], &anim->bboxes[i].max[0],
		      &anim->bboxes[i].max[1], &anim->bboxes[i].max[2]);
	    }
	}
      else if (strncmp (buff, "baseframe {", 10) == 0)
	{
	  for (i = 0; i < anim->num_joints; ++i)
	    {
	      /* Read whole line */
	      fgets (buff, sizeof (buff), fp);

	      /* Read base frame joint */
	      if (sscanf (buff, " ( %f %f %f ) ( %f %f %f )",
			  &baseFrame[i].pos[0], &baseFrame[i].pos[1],
			  &baseFrame[i].pos[2], &baseFrame[i].orient[0],
			  &baseFrame[i].orient[1], &baseFrame[i].orient[2]) == 6)
		{
		  /* Compute the w component */
		  Quat_computeW (baseFrame[i].orient);
		}
	    }
	}
      else if (sscanf (buff, " frame %d", &frame_index) == 1)
	{
	  /* Read frame data */
	  for (i = 0; i < numAnimatedComponents; ++i)
	    fscanf (fp, "%f", &animFrameData[i]);

	  /* Build frame skeleton from the collected data */
	  BuildFrameSkeleton (jointInfos, baseFrame, animFrameData,
			      anim->skelFrames[frame_index], anim->num_joints);
	}
  }

  fclose (fp);

  /* Free temporary data allocated */
  if (animFrameData)
    free (animFrameData);

  if (baseFrame)
    free (baseFrame);

  if (jointInfos)
    free (jointInfos);

  return 1;
}

void md5load::Quat_computeW (quat4_t q)
{
  float t = 1.0f - (q[X] * q[X]) - (q[Y] * q[Y]) - (q[Z] * q[Z]);

  if (t < 0.0f)
    q[W] = 0.0f;
  else
    q[W] = -sqrt (t);
}

/**
 * Build skeleton for a given frame data.
 */
//used to be static
void md5load::BuildFrameSkeleton (const struct joint_info_t *jointInfos, const baseframe_joint_t *baseFrame, const float *animFrameData, struct md5_joint_t *skelFrame, int num_joints)
{
  int i;

  for (i = 0; i < num_joints; ++i)
    {
      const struct baseframe_joint_t *baseJoint = &baseFrame[i];
      vec3_t animatedPos;
      quat4_t animatedOrient;
      int j = 0;

      memcpy (animatedPos, baseJoint->pos, sizeof (vec3_t));
      memcpy (animatedOrient, baseJoint->orient, sizeof (quat4_t));

      if (jointInfos[i].flags & 1) /* Tx */
	{
	  animatedPos[0] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      if (jointInfos[i].flags & 2) /* Ty */
	{
	  animatedPos[1] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      if (jointInfos[i].flags & 4) /* Tz */
	{
	  animatedPos[2] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      if (jointInfos[i].flags & 8) /* Qx */
	{
	  animatedOrient[0] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      if (jointInfos[i].flags & 16) /* Qy */
	{
	  animatedOrient[1] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      if (jointInfos[i].flags & 32) /* Qz */
	{
	  animatedOrient[2] = animFrameData[jointInfos[i].startIndex + j];
	  ++j;
	}

      /* Compute orient quaternion's w value */
      Quat_computeW (animatedOrient);

      /* NOTE: we assume that this joint's parent has
	 already been calculated, i.e. joint's ID should
	 never be smaller than its parent ID. */
      struct md5_joint_t *thisJoint = &skelFrame[i];

      int parent = jointInfos[i].parent;
      thisJoint->parent = parent;
      strcpy (thisJoint->name, jointInfos[i].name);

      /* Has parent? */
      if (thisJoint->parent < 0)
	{
	  memcpy (thisJoint->pos, animatedPos, sizeof (vec3_t));
	  memcpy (thisJoint->orient, animatedOrient, sizeof (quat4_t));
	}
      else
	{
	  struct md5_joint_t *parentJoint = &skelFrame[parent];
	  vec3_t rpos; /* Rotated position */

	  /* Add positions */
	  Quat_rotatePoint (parentJoint->orient, animatedPos, rpos);
	  thisJoint->pos[0] = rpos[0] + parentJoint->pos[0];
	  thisJoint->pos[1] = rpos[1] + parentJoint->pos[1];
	  thisJoint->pos[2] = rpos[2] + parentJoint->pos[2];

	  /* Concatenate rotations */
	  Quat_multQuat (parentJoint->orient, animatedOrient, thisJoint->orient);
	  Quat_normalize (thisJoint->orient);
	}
    }
}

void md5load::Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out)
{
  quat4_t tmp, inv, final;

  inv[X] = -q[X]; inv[Y] = -q[Y];
  inv[Z] = -q[Z]; inv[W] =  q[W];

  Quat_normalize (inv);

  Quat_multVec (q, in, tmp);
  Quat_multQuat (tmp, inv, final);

  out[X] = final[X];
  out[Y] = final[Y];
  out[Z] = final[Z];
}

void md5load::Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out)
{
  out[W] = (qa[W] * qb[W]) - (qa[X] * qb[X]) - (qa[Y] * qb[Y]) - (qa[Z] * qb[Z]);
  out[X] = (qa[X] * qb[W]) + (qa[W] * qb[X]) + (qa[Y] * qb[Z]) - (qa[Z] * qb[Y]);
  out[Y] = (qa[Y] * qb[W]) + (qa[W] * qb[Y]) + (qa[Z] * qb[X]) - (qa[X] * qb[Z]);
  out[Z] = (qa[Z] * qb[W]) + (qa[W] * qb[Z]) + (qa[X] * qb[Y]) - (qa[Y] * qb[X]);
}

void md5load::Quat_normalize (quat4_t q)
{
  /* compute magnitude of the quaternion */
  float mag = sqrt ((q[X] * q[X]) + (q[Y] * q[Y])
		    + (q[Z] * q[Z]) + (q[W] * q[W]));

  /* check for bogus length, to protect against divide by zero */
  if (mag > 0.0f)
    {
      /* normalize it */
      float oneOverMag = 1.0f / mag;

      q[X] *= oneOverMag;
      q[Y] *= oneOverMag;
      q[Z] *= oneOverMag;
      q[W] *= oneOverMag;
    }
}

void md5load::Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out)
{
  out[W] = - (q[X] * v[X]) - (q[Y] * v[Y]) - (q[Z] * v[Z]);
  out[X] =   (q[W] * v[X]) + (q[Y] * v[Z]) - (q[Z] * v[Y]);
  out[Y] =   (q[W] * v[Y]) + (q[Z] * v[X]) - (q[X] * v[Z]);
  out[Z] =   (q[W] * v[Z]) + (q[X] * v[Y]) - (q[Y] * v[X]);
}

void md5load::draw (float x, float y, float z, float scale)
//void md5load::draw ()
{
  int i;
  static float angle = 0;
  static double curent_time = 0;
  static double last_time = 0;

  last_time = curent_time;
  curent_time = (double)glutGet (GLUT_ELAPSED_TIME) / 1000.0;

  glLoadIdentity ();

  if (drawTexture == true)
  {
	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);
  }
  else
  {
	glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
  }

  glTranslatef (x, y, z);
  //glTranslatef(0.0f, -35.0f, -150.0f);

  glRotatef (-90.0f, 1.0, 0.0, 0.0);

  glScalef(scale, scale, scale);

  if (rotate == true)
  {
	glRotatef (angle, 0.0, 0.0, 1.0);
  }

  angle += 25 * (curent_time - last_time);

  if (angle > 360.0f)
    angle -= 360.0f;

  if (animated)
    {
 //     /* Calculate current and next frames */
      Animate (&md5anim, &animInfo, curent_time - last_time);

 //     /* Interpolate skeletons between two frames */
      InterpolateSkeletons (md5anim.skelFrames[animInfo.curr_frame],
			    md5anim.skelFrames[animInfo.next_frame],
			    md5anim.num_joints,
			    animInfo.last_time * md5anim.frameRate,
			    skeleton);
    }
  else
    {
      /* No animation, use bind-pose skeleton */
      skeleton = md5file.baseSkel;
    }


 // /* Draw skeleton */
  if (drawSkeleton == true)
  {
	DrawSkeleton (skeleton, md5file.num_joints);
  }


 //// /* Draw each mesh of the model */
  for (i = 0; i < md5file.num_meshes; ++i)
    {

	  glBindTexture(GL_TEXTURE_2D, modeltexture);

      PrepareMesh (&md5file.meshes[i], skeleton);

      glVertexPointer (3,GL_FLOAT,sizeof(GL_FLOAT)*5,vertexArray);

	  char *evilPointer = (char *)vertexArray;
	  evilPointer+=sizeof(GL_FLOAT)*3;
	  glTexCoordPointer(2,GL_FLOAT,sizeof(GL_FLOAT)*5,evilPointer);

      glDrawElements (GL_TRIANGLES, md5file.meshes[i].num_tris * 3, GL_UNSIGNED_INT, vertexIndices);
    }

}

/**
 * Prepare a mesh for drawing.  Compute mesh's final vertex positions
 * given a skeleton.  Put the vertices in vertex arrays.
 */
void md5load::PrepareMesh (const struct md5_mesh_t *mesh, const struct md5_joint_t *skeleton)

{
  int i, j, k;

  /* Setup vertex indices */
  for (k = 0, i = 0; i < mesh->num_tris; ++i)
    {
      for (j = 0; j < 3; ++j, ++k)
	vertexIndices[k] = mesh->triangles[i].index[j];
    }

  /* Setup vertices */
  for (i = 0; i < mesh->num_verts; ++i)
    {
      vec3_t finalVertex = { 0.0f, 0.0f, 0.0f };

      /* Calculate final vertex to draw with weights */
      for (j = 0; j < mesh->vertices[i].count; ++j)
	{
	  const struct md5_weight_t *weight
	    = &mesh->weights[mesh->vertices[i].start + j];
	  const struct md5_joint_t *joint
	    = &skeleton[weight->joint];

	  /* Calculate transformed vertex for this weight */
	  vec3_t wv;
	  Quat_rotatePoint (joint->orient, weight->pos, wv);

	  /* The sum of all weight->bias should be 1.0 */
	  finalVertex[0] += (joint->pos[0] + wv[0]) * weight->bias;
	  finalVertex[1] += (joint->pos[1] + wv[1]) * weight->bias;
	  finalVertex[2] += (joint->pos[2] + wv[2]) * weight->bias;
	}

      vertexArray[i][0] = finalVertex[0];
      vertexArray[i][1] = finalVertex[1];
      vertexArray[i][2] = finalVertex[2];
	  vertexArray[i][3] = mesh->vertices[i].st[0];
	  vertexArray[i][4] = 1.0f - mesh->vertices[i].st[1];
    }

  
}

/**
 * Perform animation related computations.  Calculate the current and
 * next frames, given a delta time.
 */
void md5load::Animate (const struct md5_anim_t *anim, struct anim_info_t *animInfo, double dt)
{
  int maxFrames = anim->num_frames - 1;

  animInfo->last_time += dt;

  /* move to next frame */
  if (animInfo->last_time >= animInfo->max_time)
    {
      animInfo->curr_frame++;
      animInfo->next_frame++;
      animInfo->last_time = 0.0;

      if (animInfo->curr_frame > maxFrames)
	animInfo->curr_frame = 0;

      if (animInfo->next_frame > maxFrames)
	animInfo->next_frame = 0;
    }
}

/**
 * Smoothly interpolate two skeletons
 */
void md5load::InterpolateSkeletons (const struct md5_joint_t *skelA, const struct md5_joint_t *skelB, int num_joints, float interp, struct md5_joint_t *out)
{
  int i;

  for (i = 0; i < num_joints; ++i)
    {
      /* Copy parent index */
      out[i].parent = skelA[i].parent;

      /* Linear interpolation for position */
      out[i].pos[0] = skelA[i].pos[0] + interp * (skelB[i].pos[0] - skelA[i].pos[0]);
      out[i].pos[1] = skelA[i].pos[1] + interp * (skelB[i].pos[1] - skelA[i].pos[1]);
      out[i].pos[2] = skelA[i].pos[2] + interp * (skelB[i].pos[2] - skelA[i].pos[2]);

      /* Spherical linear interpolation for orientation */
      Quat_slerp (skelA[i].orient, skelB[i].orient, interp, out[i].orient);
    }
}

void md5load::FreeVertexArrays ()
{
  if (vertexArray)
    {
      free (vertexArray);
      vertexArray = NULL;
    }

  if (vertexIndices)
    {
      free (vertexIndices);
      vertexIndices = NULL;
    }
}

void md5load::Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out)
{
  /* Check for out-of range parameter and return edge points if so */
  if (t <= 0.0)
    {
      memcpy (out, qa, sizeof(quat4_t));
      return;
    }

  if (t >= 1.0)
    {
      memcpy (out, qb, sizeof (quat4_t));
      return;
    }

  /* Compute "cosine of angle between quaternions" using dot product */
  float cosOmega = Quat_dotProduct (qa, qb);

  /* If negative dot, use -q1.  Two quaternions q and -q
     represent the same rotation, but may produce
     different slerp.  We chose q or -q to rotate using
     the acute angle. */
  float q1w = qb[W];
  float q1x = qb[X];
  float q1y = qb[Y];
  float q1z = qb[Z];

  if (cosOmega < 0.0f)
    {
      q1w = -q1w;
      q1x = -q1x;
      q1y = -q1y;
      q1z = -q1z;
      cosOmega = -cosOmega;
    }

  /* We should have two unit quaternions, so dot should be <= 1.0 */
  assert (cosOmega < 1.1f);

  /* Compute interpolation fraction, checking for quaternions
     almost exactly the same */
  float k0, k1;

  if (cosOmega > 0.9999f)
    {
      /* Very close - just use linear interpolation,
	 which will protect againt a divide by zero */

      k0 = 1.0f - t;
      k1 = t;
    }
  else
    {
      /* Compute the sin of the angle using the
	 trig identity sin^2(omega) + cos^2(omega) = 1 */
      float sinOmega = sqrt (1.0f - (cosOmega * cosOmega));

      /* Compute the angle from its sin and cosine */
      float omega = atan2 (sinOmega, cosOmega);

      /* Compute inverse of denominator, so we only have
	 to divide once */
      float oneOverSinOmega = 1.0f / sinOmega;

      /* Compute interpolation parameters */
      k0 = sin ((1.0f - t) * omega) * oneOverSinOmega;
      k1 = sin (t * omega) * oneOverSinOmega;
    }

  /* Interpolate and return new quaternion */
  out[W] = (k0 * qa[3]) + (k1 * q1w);
  out[X] = (k0 * qa[0]) + (k1 * q1x);
  out[Y] = (k0 * qa[1]) + (k1 * q1y);
  out[Z] = (k0 * qa[2]) + (k1 * q1z);
}

float md5load::Quat_dotProduct (const quat4_t qa, const quat4_t qb)
{
  return ((qa[X] * qb[X]) + (qa[Y] * qb[Y]) + (qa[Z] * qb[Z]) + (qa[W] * qb[W]));
}

/**
 * Draw the skeleton as lines and points (for joints).
 */
void md5load::DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints)
{
  int i;

  /* Draw each joint */
  glPointSize (5.0f);
  glColor3f (1.0f, 0.0f, 0.0f);
  glBegin (GL_POINTS);
    for (i = 0; i < num_joints; ++i)
      glVertex3fv (skeleton[i].pos);
  glEnd ();
  glPointSize (1.0f);

  /* Draw each bone */
  glColor3f (0.0f, 1.0f, 0.0f);
  glBegin (GL_LINES);
    for (i = 0; i < num_joints; ++i)
      {
		if (skeleton[i].parent != -1)
		{
			glVertex3fv (skeleton[skeleton[i].parent].pos);
			glVertex3fv (skeleton[i].pos);
		}
      }
  glEnd();

  glColor3f(1.0f,1.0f,1.0f);
}



GLuint md5load::loadTexture(char *fileName)
{
	ILuint ILImage;
	GLuint textureHandle;
	ilGenImages(1,&ILImage);
	glGenTextures(1,&textureHandle);

	ilBindImage(ILImage);
	ILboolean success = ilLoadImage(fileName);
	if (!success)
	{
		//MessageBox(NULL, "could not load texture", "problem", 0);
		printf("texture not found: %s\n",fileName);
		while(1);;
	}
	
	glBindTexture(GL_TEXTURE_2D, textureHandle);
	// enable automatic mipmap generation
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		ilGetInteger(IL_IMAGE_BPP), 
		ilGetInteger(IL_IMAGE_WIDTH),
		ilGetInteger(IL_IMAGE_HEIGHT), 
		0, 
		ilGetInteger(IL_IMAGE_FORMAT), 
		GL_UNSIGNED_BYTE,
		ilGetData()
		);

	// release IL image
	ilDeleteImages(1, &ILImage);
	return(textureHandle);
}

/**
 * Free resources allocated for the model.
 */
void md5load::FreeModel (struct md5_model_t *mdl)
{
  int i;

  if (mdl->baseSkel)
    {
      free (mdl->baseSkel);
      mdl->baseSkel = NULL;
    }

  if (mdl->meshes)
    {
      /* Free mesh data */
      for (i = 0; i < mdl->num_meshes; ++i)
	{
	  if (mdl->meshes[i].vertices)
	    {
	      free (mdl->meshes[i].vertices);
	      mdl->meshes[i].vertices = NULL;
	    }

	  if (mdl->meshes[i].triangles)
	    {
	      free (mdl->meshes[i].triangles);
	      mdl->meshes[i].triangles = NULL;
	    }

	  if (mdl->meshes[i].weights)
	    {
	      free (mdl->meshes[i].weights);
	      mdl->meshes[i].weights = NULL;
	    }
	}

      free (mdl->meshes);
      mdl->meshes = NULL;
    }
}

float md5load::getSkeletonPosition(int joint, int xyz)
{
	return skeleton[joint].pos[xyz];
}