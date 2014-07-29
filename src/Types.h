#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <math.h>

#define INLINE static inline

typedef float Vec1[1];
typedef float Vec2[2];
typedef float Vec3[3];
typedef float Vec4[4];

typedef Vec1 Color1;
typedef Vec2 Color2;
typedef Vec3 Color3;
typedef Vec4 Color4;

typedef float Mat44[4*4]; // Column-major

INLINE void toVecN(unsigned n,float* out,
                   unsigned sizeIn,const float* in) {
    unsigned len = sizeIn < n ? sizeIn : n;
    unsigned i;
    for(i=0;i<len;++i) {
        out[i] = in[i];
    }
    for(i=len;i<n;++i) {
        out[i] = 0;
    }
}

INLINE void vecNZero(unsigned n,float* out) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = 0;
    }
}
INLINE void vec3Zero(float* out) {
    out[0] = out[1] = out[2] = 0;
}
INLINE void vec4Zero(float* out) {
    out[0] = out[1] = out[2] = out[3] = 0;
}

INLINE void vecNCopy(unsigned n,float* out,const float* in) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = in[i];
    }
}
INLINE void vec3Copy(float* out,const float* in) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
}
INLINE void vec4Copy(float* out,const float* in) {
    out[0] = in[0];
    out[1] = in[1];
    out[2] = in[2];
    out[3] = in[3];
}

INLINE void vecNSwap(unsigned n,float* a,  float* b) {
    unsigned i;
    float tmp;
    for(i=0;i<n;++i) {
        tmp = a[i];
        a[i] = b[i];
        b[i] = tmp;
    }
}

INLINE void vecNNegate(unsigned n,float* out,const float* in) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = -in[i];
    }
}
INLINE void vec3Negate(float* out,const float* in) {
    out[0] = -in[0];
    out[1] = -in[1];
    out[2] = -in[2];
}

INLINE void vecNAdd(unsigned n,float* out,const float* a,const float* b) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = a[i] + b[i];
    }
}
INLINE void vec3Add(float* out,const float* a,const float* b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
}
INLINE void vec4Add(float* out,const float* a,const float* b) {
    out[0] = a[0] + b[0];
    out[1] = a[1] + b[1];
    out[2] = a[2] + b[2];
    out[3] = a[3] + b[3];
}
INLINE void vecNSub(unsigned n,float* out,const float* a,const float* b) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = a[i] - b[i];
    }
}
INLINE void vecNMult(unsigned n,float* out,const float* v,float s) {
    unsigned i;
    for(i=0;i<n;++i) {
        out[i] = v[i] * s;
    }
}
INLINE void vec3Mult(float* out,const float* v,float s) {
    out[0] = v[0] * s;
    out[1] = v[1] * s;
    out[2] = v[2] * s;
}
INLINE void vec4Mult(float* out,const float* v,float s) {
    out[0] = v[0] * s;
    out[1] = v[1] * s;
    out[2] = v[2] * s;
    out[3] = v[3] * s;
}

INLINE void vecNInterpolate(unsigned n,float* out,float s,
                            const float* a,const float* b) {
    float diff[64];
    vecNSub(n,diff,b,a);
    vecNMult(n,diff,diff,s);
    vecNAdd(n,out,a,diff);
}

INLINE float vecNDotProd(unsigned n,const float* a,const float* b) {
    float ret = 0;
    unsigned i;
    for(i=0;i<n;++i) {
        ret += a[i]*b[i];
    }
    return ret;
}
INLINE float vecNMagSquared(unsigned n,const float* v) {
    return vecNDotProd(n,v,v);
}
INLINE float vecNMag(unsigned n,const float* v) {
    return sqrt(vecNMagSquared(n,v));
}

INLINE void vecNNormalize(unsigned n,float* out,const float* in) {
    vecNMult(n,out,in,1/vecNMag(n,in));
}

#define toVec3(out,sizeIn,in) toVecN(3,(out),(sizeIn),(in))
#define toVec4(out,sizeIn,in) toVecN(4,(out),(sizeIn),(in))

/* #define vec3Zero(out)      vecNZero  (3,(out)) */
/* #define vec3Copy(out,in)   vecNCopy  (3,(out),(in)) */
#define vec3Swap(a,b)      vecNSwap  (3,(a),(b))
/* #define vec3Negate(out,in) vecNNegate(3,(out),(in)) */
/* #define vec3Add(out,a,b)   vecNAdd   (3,(out),(a),(b)) */
#define vec3Sub(out,a,b)   vecNSub   (3,(out),(a),(b))
/* #define vec3Mult(out,v,s)  vecNMult  (3,(out),(v),(s)) */

#define vec3Interpolate(out,s,a,b)  vecNInterpolate(3,(out),(s),(a),(b))

#define vec3DotProd(a,b)  vecNDotProd   (3,(a),(b))
#define vec3MagSquared(v) vecNMagSquared(3,(v))
#define vec3Mag(v)        vecNMag       (3,(v))

#define vec3Normalize(v)  vecNNormalize (3,(v))

/* #define vec4Zero(out)      vecNZero  (4,(out)) */
#define vec4Ident(out)     vecNIndent(4,(out))
/* #define vec4Copy(out,in)   vecNCopy  (4,(out),(in)) */
#define vec4Swap(a,b)      vecNSwap  (4,(a),(b))
/* #define vec4Negate(out,in) vecNNegate(4,(out),(in)) */
/* #define vec4Add(out,a,b)   vecNAdd   (4,(out),(a),(b)) */
#define vec4Sub(out,a,b)   vecNSub   (4,(out),(a),(b))
/* #define vec4Mult(out,v,s)  vecNMult  (4,(out),(v),(s)) */

#define vec4Interpolate(out,s,a,b)  vecNInterpolate(4,(out),(s),(a),(b))

#define vec4DotProd(a,b)  vecNDotProd   (4,(a),(b))
#define vec4MagSquared(v) vecNMagSquared(4,(v))
#define vec4Mag(v)        vecNMag       (4,(v))

#define vec4Normalize(v)  vecNNormalize (4,(v))


#define mat44At(m,c,r) m[c*4+r]

INLINE void mat44Zero(Mat44 out) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = 0;
    }
}
INLINE void mat44Ident(Mat44 out) {
    unsigned r,c;
    for(c=0;c<4;++c) {
        for(r=0;r<4;++r) {
            out[c*4+r] = r == c ? 1 : 0;
        }
    }
}
INLINE void mat44Copy(Mat44 out,const Mat44 in) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = in[i];
    }
}
INLINE void mat44Negate(Mat44 out,const Mat44 in) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = -in[i];
    }
}
INLINE void mat44Transpose(Mat44 out,const Mat44 in) {
    unsigned c,r;
    for(c=0;c<4;++c) {
        for(r=0;r<4;++r) {
            out[c*4+r] = in[r*4+c];
        }
    }
}
INLINE void mat44Add(Mat44 out,const Mat44 a,const Mat44 b) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = a[i] + b[i];
    }
}
INLINE void mat44Sub(Mat44 out,const Mat44 a,const Mat44 b) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = a[i] - b[i];
    }
}
INLINE void mat44Scale(Mat44 out,const Mat44 m,float s) {
    unsigned i;
    for(i=0;i<4*4;++i) {
        out[i] = m[i] * s;
    }
}
INLINE void mat44Mult(Mat44 out,const Mat44 a,const Mat44 b) {
    Mat44 final;
    unsigned r,c,i;
    for(c=0;c<4;++c) {
        for(r=0;r<4;++r) {
            float val = 0;
            for(i=0;i<4;++i) {
                val += a[i*4+r]*b[c*4+i];
            }
            final[c*4+r] = val;
        }
    }
    mat44Copy(out,final);
}

INLINE void mat44MultVec4(Vec4 out,const Mat44 A,const Vec4 v) {
    Vec4 final;
    unsigned r,i;
    for(r=0;r<4;++r) {
        float val = 0;
        for(i=0;i<4;++i) {
            val += A[i*4+r]*v[i];
        }
        final[r] = val;
    }
    vec4Copy(out,final);
}

INLINE void mat44MultVec3(Vec4 out,const Mat44 A,const Vec3 v) {
    Vec4 v4 = { v[0],v[1],v[2],1 };
    mat44MultVec4(out,A,v4);
}

INLINE void mat44Translate(Mat44 out,double x,double y,double z) {
    out[0]  = 1;
    out[1]  = 0;
    out[2]  = 0;
    out[3]  = 0;

    out[4]  = 0;
    out[5]  = 1;
    out[6]  = 0;
    out[7]  = 0;

    out[8]  = 0;
    out[9]  = 0;
    out[10] = 1;
    out[11] = 0;

    out[12] = x;
    out[13] = y;
    out[14] = z;
    out[15] = 1;
}
INLINE void mat44Rotate(Mat44 out,double degAngle,
                        double x,double y,double z) {
    double radAngle = degAngle*M_PI/180;
    double s = sin(radAngle),c = cos(radAngle);

    out[0]  = x*x*(1-c)+c;
    out[1]  = x*y*(1-c)+z*s;
    out[2]  = x*z*(1-c)-y*s;
    out[3]  = 0;

    out[4]  = x*y*(1-c)-z*s;
    out[5]  = y*y*(1-c)+c;
    out[6]  = y*z*(1-c)+x*s;
    out[7]  = 0;

    out[8]  = x*z*(1-c)+y*s;
    out[9]  = y*z*(1-c)-x*s;
    out[10] = z*z*(1-c)+c;
    out[11] = 0;

    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}
INLINE void mat44Ortho(Mat44 out,double left,  double right,
                                 double bottom,double top,
                                 double near,  double far) {
    out[0]  = 2/(right-left);
    out[1]  = 0;
    out[2]  = 0;
    out[3]  = 0;

    out[4]  = 0;
    out[5]  = 2/(top-bottom);
    out[6]  = 0;
    out[7]  = 0;

    out[8]  = 0;
    out[9]  = 0;
    out[10] = -2/(far-near);
    out[11] = 0;

    out[12] = -(right+left)/(right-left);
    out[13] = -(top+bottom)/(top-bottom);
    out[14] = -(near+far)/(near-far);
    out[15] = 1;
}
INLINE void mat44Frustum(Mat44 out,double left,  double right,
                                   double bottom,double top,
                                   double near,  double far) {
    out[0]  = 2*near/(right-left);
    out[1]  = 0;
    out[2]  = 0;
    out[3]  = 0;

    out[4]  = 0;
    out[5]  = 2*near/(top-bottom);
    out[6]  = 0;
    out[7]  = 0;

    out[8]  = (right+left)/(right-left);
    out[9]  = (top+bottom)/(top-bottom);
    out[10] = (near+far)/(near-far);
    out[11] = -1;

    out[12] = 0;
    out[13] = 0;
    out[14] = 2*far*near/(near-far);
    out[15] = 0;
}

INLINE void mat44Perspective(Mat44 out,double degFovy,double aspect,
                                       double near,   double far) {
    double radFovy = degFovy*M_PI/180;
    double halfVert = near*tan(radFovy/2);
    double halfHoriz = halfVert*aspect;
    mat44Frustum(out,-halfHoriz,halfHoriz,
                     -halfVert,halfVert,
                     near,far);
}

INLINE void mat44Print(FILE* file,const Mat44 mat) {
    fprintf(file,"%f %f %f %f\n"\
                 "%f %f %f %f\n"\
                 "%f %f %f %f\n"\
                 "%f %f %f %f\n",
                 mat[0],mat[4],mat[8], mat[12],
                 mat[1],mat[5],mat[9], mat[13],
                 mat[2],mat[6],mat[10],mat[14],
                 mat[3],mat[7],mat[11],mat[15]);

}

#endif
