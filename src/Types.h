#ifndef TYPES_H
#define TYPES_H

typedef float Vec1[1];
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];

typedef Vec1 Color1;
typedef Vec2 Color2;
typedef Vec3 Color3;
typedef Vec4 Color4;

typedef float Mat44[4][4]; // Column-major

void toVec4    (Vec4* out,unsigned sizeIn,const float* vecIn);

void vec4Zero  (Vec4* out);
void vec4Ident (Vec4* out);// Sets w to 1
void vec4Copy  (Vec4* out,const Vec4 in);
void vec4Swap  (Vec4* a,  Vec4* b);
void vec4Negate(Vec4* out,const Vec4 in);
void vec4Add   (Vec4* out,const Vec4 a,const Vec4 b);
void vec4Sub   (Vec4* out,const Vec4 a,const Vec4 b);
void vec4Mult  (Vec4* out,const Vec4 v,float s);

void mat44Zero  (Mat44* out);
void mat44Ident (Mat44* out);
void mat44Copy  (Mat44* out,const Mat44 in);
void mat44Negate(Mat44* out,const Mat44 in);
void mat44Add   (Mat44* out,const Mat44 a,const Mat44 b);
void mat44Sub   (Mat44* out,const Mat44 a,const Mat44 b);
void mat44Scale (Mat44* out,const Mat44 m,float s);
void mat44Mult  (Mat44* out,const Mat44 a,const Mat44 b);

void mat44MultVec4(Vec4* out,const Mat44 A,const Vec4 v);

#endif
