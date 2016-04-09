#ifndef MD5LOAD
#define MD5LOAD

#include <string.h>
#include <math.h>
#include <fstream>
#include <iostream>
#include <assert.h>

#include "glew.h"
#include "glut.h"

//texture loading
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"
#pragma comment(lib, "DevIL.lib")
#pragma comment(lib, "ILU.lib")
#pragma comment(lib, "ILUT.lib")

		/* Vectors */
		typedef float vec2_t[2];
		typedef float vec3_t[3];
		typedef float vec5_t[5];

		/* Quaternion (x, y, z, w) */
		typedef float quat4_t[4];
		enum {
		  X, Y, Z, W
		};

		/* Joint */
		struct md5_joint_t
		{
		  char name[64];
		  int parent;

		  vec3_t pos;
		  quat4_t orient;
		};

		/* Vertex */
		struct md5_vertex_t
		{
		  vec2_t st;

		  int start; /* start weight */
		  int count; /* weight count */
		};

		/* Triangle */
		struct md5_triangle_t
		{
		  int index[3];
		};

		/* Weight */
		struct md5_weight_t
		{
		  int joint;
		  float bias;

		  vec3_t pos;
		};

		/* Bounding box */
		struct md5_bbox_t
		{
		  vec3_t min;
		  vec3_t max;
		};

		/* MD5 mesh */
		struct md5_mesh_t
		{
		  struct md5_vertex_t *vertices;
		  struct md5_triangle_t *triangles;
		  struct md5_weight_t *weights;

		  int num_verts;
		  int num_tris;
		  int num_weights;

		  char shader[256];
		};

		/* MD5 model structure */
		struct md5_model_t
		{
		  struct md5_joint_t *baseSkel;
		  struct md5_mesh_t *meshes;

		  int num_joints;
		  int num_meshes;
		};

		/* Animation data */
		struct md5_anim_t
		{
		  int num_frames;
		  int num_joints;
		  int frameRate;

		  struct md5_joint_t **skelFrames;
		  struct md5_bbox_t *bboxes;
		};

		/* Animation info */
		const struct anim_info_t
		{
		  int curr_frame;
		  int next_frame;

		  double last_time;
		  double max_time;
		};

		/* Joint info */
		struct joint_info_t
		{
		  char name[64];
		  int parent;
		  int flags;
		  int startIndex;
		};


		/* Base frame joint */
		const struct baseframe_joint_t
		{
		  vec3_t pos;
		  quat4_t orient;
		};



using namespace std;

class md5load
{




	public:
        md5load(void);           //constructor
        ~md5load(void);          //destructor
        void Run(ifstream &theCardFile);

		void draw(float x, float y, float z, float scale);
		void init (const char *filename, const char *animfile, char *texturefile);

		void enableSkeleton(bool skeleton);
		void enableTextured(bool textured);
		void enableRotate(bool rotated);
		void cleanup();
		
		float getSkeletonPosition(int joint, int xyz);


     private:
		 //function definitions
		void AllocVertexArrays ();
		void FreeAnim (struct md5_anim_t *anim);
		int ReadMD5Model (const char *filename, struct md5_model_t *mdl);
		int ReadMD5Anim (const char *filename, struct md5_anim_t *anim);
		//used to be static
		void BuildFrameSkeleton (const struct joint_info_t *jointInfos, const baseframe_joint_t *baseFrame, const float *animFrameData, struct md5_joint_t *skelFrame, int num_joints);
		void InterpolateSkeletons (const struct md5_joint_t *skelA, const struct md5_joint_t *skelB, int num_joints, float interp, struct md5_joint_t *out);
		void PrepareMesh (const struct md5_mesh_t *mesh, const struct md5_joint_t *skeleton);
		void Animate (const struct md5_anim_t *anim, struct anim_info_t *animInfo, double dt);
		void DrawSkeleton (const struct md5_joint_t *skeleton, int num_joints);
		void FreeVertexArrays ();
		void FreeModel (struct md5_model_t *mdl);

		//quaternion functions
		void Quat_computeW (quat4_t q);
		void Quat_rotatePoint (const quat4_t q, const vec3_t in, vec3_t out);
		void Quat_multQuat (const quat4_t qa, const quat4_t qb, quat4_t out);
		void Quat_normalize (quat4_t q);
		void Quat_multVec (const quat4_t q, const vec3_t v, quat4_t out);
		void Quat_slerp (const quat4_t qa, const quat4_t qb, float t, quat4_t out);
		float Quat_dotProduct (const quat4_t qa, const quat4_t qb);

		//texture model loading
		GLuint loadTexture(char *fileName);
		GLuint modeltexture;

		//variable definitions
		struct md5_model_t md5file;
		struct md5_anim_t md5anim;

		int animated;// = 0;

		struct md5_joint_t *skeleton;// = NULL;
		struct anim_info_t animInfo;

		/* vertex array related stuff */
		int max_verts;// = 0;
		int max_tris;// = 0;

		vec5_t *vertexArray;// = NULL;
		GLuint *vertexIndices;// = NULL;

		bool drawTexture;
		bool drawSkeleton;
		bool rotate;
		


		
}; 
#endif