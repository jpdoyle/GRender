#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>

typedef float Vec1[1];
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];

typedef Vec1 Color1;
typedef Vec2 Color2;
typedef Vec3 Color3;
typedef Vec4 Color4;

typedef float Mat44[4*4]; // Column-major

void toVecN    (unsigned n,float* out,
                unsigned sizeIn,const float* in);

void vecNZero  (unsigned n,float* out);
void vecNCopy  (unsigned n,float* out,const float* in);
void vecNSwap  (unsigned n,float* a,  float* b);
void vecNNegate(unsigned n,float* out,const float* in);
void vecNAdd   (unsigned n,float* out,const float* a,const float* b);
void vecNSub   (unsigned n,float* out,const float* a,const float* b);
void vecNMult  (unsigned n,float* out,const float* v,float s);

float vecNDotProd   (unsigned n,const float* a,const float* b);
float vecNMagSquared(unsigned n,const float* v);
float vecNMag       (unsigned n,const float* v);

void vecNNormalize(unsigned n,float* out,const float* in);

#define toVec3(out,sizeIn,in) toVecN(3,(out),(sizeIn),(in))
#define toVec4(out,sizeIn,in) toVecN(4,(out),(sizeIn),(in))

#define vec3Zero(out)      vecNZero  (3,(out))
#define vec3Copy(out,in)   vecNCopy  (3,(out),(in))
#define vec3Swap(a,b)      vecNSwap  (3,(a),(b))
#define vec3Negate(out,in) vecNNegate(3,(out),(in))
#define vec3Add(out,a,b)   vecNAdd   (3,(out),(a),(b))
#define vec3Sub(out,a,b)   vecNSub   (3,(out),(a),(b))
#define vec3Mult(out,v,s)  vecNMult  (3,(out),(v),(s))

#define vec3DotProd(a,b)  vecNDotProd   (3,(a),(b))
#define vec3MagSquared(v) vecNMagSquared(3,(v))
#define vec3Mag(v)        vecNMag       (3,(v))

#define vec3Normalize(v)  vecNNormalize (3,(v))

#define vec4Zero(out)      vecNZero  (4,(out))
#define vec4Ident(out)     vecNIndent(4,(out))
#define vec4Copy(out,in)   vecNCopy  (4,(out),(in))
#define vec4Swap(a,b)      vecNSwap  (4,(a),(b))
#define vec4Negate(out,in) vecNNegate(4,(out),(in))
#define vec4Add(out,a,b)   vecNAdd   (4,(out),(a),(b))
#define vec4Sub(out,a,b)   vecNSub   (4,(out),(a),(b))
#define vec4Mult(out,v,s)  vecNMult  (4,(out),(v),(s))

#define vec4DotProd(a,b)  vecNDotProd   (4,(a),(b))
#define vec4MagSquared(v) vecNMagSquared(4,(v))
#define vec4Mag(v)        vecNMag       (4,(v))

#define vec4Normalize(v)  vecNNormalize (4,(v))


#define mat44At(m,c,r) m[c*4+r]

void mat44Zero     (Mat44 out);
void mat44Ident    (Mat44 out);
void mat44Copy     (Mat44 out,const Mat44 in);
void mat44Negate   (Mat44 out,const Mat44 in);
void mat44Transpose(Mat44 out,const Mat44 in);
void mat44Add      (Mat44 out,const Mat44 a,const Mat44 b);
void mat44Sub      (Mat44 out,const Mat44 a,const Mat44 b);
void mat44Scale    (Mat44 out,const Mat44 m,float s);
void mat44Mult     (Mat44 out,const Mat44 a,const Mat44 b);

void mat44MultVec4(Vec4 out,const Mat44 A,const Vec4 v);

void mat44MultVec3(Vec4 out,const Mat44 A,const Vec3 v);

void mat44Translate(Mat44 out,double x,double y,double z);
void mat44Rotate   (Mat44 out,double degAngle,double x,double y, 
                                              double z);
void mat44Ortho    (Mat44 out,double left,double right,
                              double bottom,double top,
                              double near,double far);
void mat44Frustum  (Mat44 out,double left,double right,
                              double bottom,double top,
                              double near,double far);

void mat44Perspective(Mat44 out,double degFovy,double aspect,
                                double near,double far);

void mat44Print(FILE* file,const Mat44 mat);

#endif
