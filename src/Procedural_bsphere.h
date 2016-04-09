http://www.racer.nl/reference/vfc.htm
void CalcFrustumEquations()
   // From the current OpenGL modelview and projection matrices,
   // calculate the frustum plane equations (Ax+By+Cz+D=0, n=(A,B,C))
   // The equations can then be used to see on which side points are.
   {
     dfloat *m,t;
     DPlane3 *p;
     int i;
   
     // This piece of code taken from:
     // http://www.markmorley.com/opengl/frustumculling.html
     // Modified quite a bit to suit the D3 classes.
   
     // Retrieve matrices from OpenGL
     glGetFloatv(GL_MODELVIEW_MATRIX,matModelView.GetM());
     glGetFloatv(GL_PROJECTION_MATRIX,matProjection.GetM());
   
     // Combine into 1 matrix
     glGetFloatv(GL_PROJECTION_MATRIX,matFrustum.GetM());
     matFrustum.Multiply(&matModelView);
   
     // Get plane parameters
     m=matFrustum.GetM();
   
     p=&frustumPlane[RIGHT];
     p->n.x=m[3]-m[0];
     p->n.y=m[7]-m[4];
     p->n.z=m[11]-m[8];
     p->d=m[15]-m[12];
   
     p=&frustumPlane[LEFT];
     p->n.x=m[3]+m[0];
     p->n.y=m[7]+m[4];
     p->n.z=m[11]+m[8];
     p->d=m[15]+m[12];
   
     p=&frustumPlane[BOTTOM];
     p->n.x=m[3]+m[1];
     p->n.y=m[7]+m[5];
     p->n.z=m[11]+m[9];
     p->d=m[15]+m[13];
   
     p=&frustumPlane[TOP];
     p->n.x=m[3]-m[1];
     p->n.y=m[7]-m[5];
     p->n.z=m[11]-m[9];
     p->d=m[15]-m[13];
   
     p=&frustumPlane[PFAR];
     p->n.x=m[3]-m[2];
     p->n.y=m[7]-m[6];
     p->n.z=m[11]-m[10];
     p->d=m[15]-m[14];
   
     p=&frustumPlane[PNEAR];
     p->n.x=m[3]+m[2];
     p->n.y=m[7]+m[6];
     p->n.z=m[11]+m[10];
     p->d=m[15]+m[14];
   
     // Normalize all plane normals
     for(i=0;i<6;i++)
       frustumPlane[i].Normalize();
   }

   /**************
   * Classifying *
   **************/
   int DCuller::SphereInFrustum(const DVector3 *center,dfloat radius) const
   // Returns classification (INSIDE/INTERSECTING/OUTSIDE)
   {
     int i;
     const DPlane3 *p;
   
     for(i=0;i<6;i++)
     {
       p=&frustumPlane[i];
       if(p->n.x*center->x+p->n.y*center->y+p->n.z*center->z+p->d <= -radius)
         return OUTSIDE;
     }
     // Decide: Inside or intersecting
     return INTERSECTING;
   }