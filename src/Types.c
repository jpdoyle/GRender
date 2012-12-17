#include "Types.h"

static inline unsigned umin(unsigned a,unsigned b) {
    return a < b ? a : b;
}

void toVec4(Vec4* out,unsigned sizeIn,const float* vecIn) {
    unsigned len = umin(sizeIn,4);
    unsigned i;
    for(i=0;i<len;++i) {
        (*out)[i] = vecIn[i];
    }
    for(i=len;i<4;++i) {
        (*out)[i] = i < 3 ? 0 : 1;
    }
}

void vec4Zero(Vec4* out) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = 0;
    }
}

void vec4Ident(Vec4* out) {
    (*out)[0] = (*out)[1] = (*out)[2] = 0;
    (*out)[3] = 1;
}

void vec4Copy(Vec4* out,const Vec4 in) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = in[i];
    }
}

void vec4Swap(Vec4* a,Vec4* b) {
    unsigned i;
    float tmp;
    for(i=0;i<4;++i) {
        tmp = (*a)[i];
        (*a)[i] = (*b)[i];
        (*b)[i] = tmp;
    }
}

void vec4Negate(Vec4* out,const Vec4 in) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = -in[i];
    }
}

void vec4Add(Vec4* out,const Vec4 a,const Vec4 b) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = a[i] + b[i];
    }
}

void vec4Sub(Vec4* out,const Vec4 a,const Vec4 b) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = a[i] - b[i];
    }
}

void vec4Mult(Vec4* out,const Vec4 v,float s) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i] = v[i] * s;
    }
}

void mat44Zero(Mat44* out) {
    unsigned i,j;
    for(i=0;i<4;++i) {
        for(j=0;j<4;++j) {
            (*out)[i][j] = 0;
        }
    }
}

void mat44Ident(Mat44* out) {
    unsigned i;
    for(i=0;i<4;++i) {
        (*out)[i][i] = 1;
    }
}

void mat44Copy(Mat44* out,const Mat44 in) {
    unsigned i,j;
    for(i=0;i<4;++i) {
        for(j=0;j<4;++j) {
            (*out)[i][j] = in[i][j];
        }
    }
}

void mat44Add(Mat44* out,const Mat44 a,const Mat44 b) {
    unsigned i,j;
    for(i=0;i<4;++i) {
        for(j=0;j<4;++j) {
            (*out)[i][j] = a[i][j] + b[i][j];
        }
    }
}

void mat44Sub(Mat44* out,const Mat44 a,const Mat44 b) {
    unsigned i,j;
    for(i=0;i<4;++i) {
        for(j=0;j<4;++j) {
            (*out)[i][j] = a[i][j] - b[i][j];
        }
    }
}

void mat44Scale(Mat44* out,const Mat44 m,float s) {
    unsigned i,j;
    for(i=0;i<4;++i) {
        for(j=0;j<4;++j) {
            (*out)[i][j] = m[i][j] * s;
        }
    }
}

void mat44Mult(Mat44* out,const Mat44 a,const Mat44 b) {
    unsigned r,c,i;
    for(c=0;c<4;++c) {
        for(r=0;r<4;++r) {
            float val = 0;
            for(i=0;i<4;++i) {
                val += a[i][r]*b[c][i];
            }
            (*out)[c][r] = val;
        }
    }
}

void mat44MultVec4(Vec4* out,const Mat44 A,const Vec4 v) {
    unsigned r,i;
    for(r=0;r<4;++r) {
        float val = 0;
        for(i=0;i<4;++i) {
            val += A[i][r]*v[i];
        }
        (*out)[r] = val;
    }
}

