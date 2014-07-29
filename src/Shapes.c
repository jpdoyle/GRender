#include "Shapes.h"

#include <stdlib.h>
#include <string.h>

Solid* createSolid(unsigned nVerts,unsigned nInds) {
    Solid* ret = malloc(sizeof(Solid));
    ret->verts = malloc(nVerts*sizeof(CustomVert));
    ret->indices = malloc(nInds*sizeof(unsigned));
    ret->nVerts = nVerts;
    ret->nInds = nInds;
    return ret;
}

void freeSolid(Solid* s) {
    free(s->verts);
    free(s->indices);
    free(s);
}

void copySolid(Solid* out,const Solid* in) {
    memcpy(out->verts,in->verts,in->nVerts*sizeof(CustomVert));
    memcpy(out->indices,in->indices,in->nInds*sizeof(CustomVert));
}

Solid* dividedTetra(unsigned divisions,Color4* colors) {
    unsigned i,j;
    unsigned factor = 1;
    unsigned n = divisions;
    while(n--) {
        factor *= 3;
    }
    unsigned nInds = factor*12;
    Solid* ret = createSolid(nInds,nInds);
    for(i=0;i<nInds;++i) {
        ret->indices[i] = i;
    }
    CustomVert verts[] = { /* face 1 */
                           { { -1.73,-1.5,-1 }, { 1, 1, 1 }, { 0,-1,0 } },
                           { {  1.73,-1.5,-1 }, { 1, 1, 1 }, { 0,-1,0 } },
                           { {  0,   -1.5, 2 }, { 1, 1, 1 }, { 0,-1,0 } },
                           /* face 2 */
                           { {  1.73,-1.5,-1 }, { 1, 1, 1 }, { 0,0.56,-0.83 } },
                           { { -1.73,-1.5,-1 }, { 1, 1, 1 }, { 0,0.56,-0.83 } },
                           { {  0,    1.5, 0 }, { 1, 1, 1 }, { 0,0.56,-0.83 } },
                           /* face 3 */
                           { { -1.73,-1.5,-1 }, { 1, 1, 1 }, { -0.72,0.55,0.42 } },
                           { {  0,   -1.5, 2 }, { 1, 1, 1 }, { -0.72,0.55,0.42 } },
                           { {  0,    1.5, 0 }, { 1, 1, 1 }, { -0.72,0.55,0.42 } },
                           /* face 4 */
                           { {  0,   -1.5, 2 }, { 1, 1, 1 }, { 0.72,0.55,0.42 } },
                           { {  1.73,-1.5,-1 }, { 1, 1, 1 }, { 0.72,0.55,0.42 } },
                           { {  0,    1.5, 0 }, { 1, 1, 1 }, { 0.72,0.55,0.42 } } };
    unsigned blockSize = nInds/4;
    for(i=0;i<4;++i) {
        unsigned step = blockSize/4;
        for(j=0;j<3;++j) {
            /* use memcpy if = doesn't work for structs */
            ret->verts[i*blockSize+j*step] = verts[i*3+j];
        }
    }
    /* recursively subdivide triangles:
     *      c
     *     / \
     *    /   \
     *   /     \
     *  a-------b
     *  or, as an array:
     *  a _ _ b _ _ c _ _ _ _ _
     *  becomes
     *      c
     *     / \
     *    e---f
     *   / \ / \
     *  a---d---b
     *  a d e b f d c e f e d f
     */
    while(blockSize >= 12) {
        for(i=0;i<nInds/blockSize;++i) {
            unsigned index = i*blockSize;
            unsigned step = blockSize/12;
            CustomVert* a = ret->verts+index,
                      * b = a + 3*step,
                      * c = a + 6*step;
            CustomVert* d = a + 1*step,
                      * e = a + 2*step,
                      * f = a + 4*step;

            cvertInterpolate(d,0.5,a,b);
            ret->verts[index+5*step]  = *d;
            ret->verts[index+10*step] = *d;

            cvertInterpolate(e,0.5,c,a);
            ret->verts[index+7*step] = *e;
            ret->verts[index+9*step] = *e;

            cvertInterpolate(f,0.5,b,c);
            ret->verts[index+8*step]  = *f;
            ret->verts[index+11*step] = *f;
        }
        blockSize /= 4;
    }

    return ret;
}

void spherify(Solid* s) {
    unsigned n = s->nVerts;
    Vec3 centroid = { 0,0,0 };
    unsigned i;
    for(i=0;i<n;++i) {
        vec3Add(centroid,centroid,s->verts[i].loc);
    }
    vec3Mult(centroid,centroid,1.0/n);
    Vec3* diffs = malloc(n*sizeof(Vec3));
    float* dists = malloc(n*sizeof(float));
    float maxR = 0;
    for(i=0;i<n;++i) {
        vec3Sub(diffs[i],s->verts[i].loc,centroid);
        float r = vec3Mag(diffs[i]);
        dists[i] = r;
        if(r > maxR) {
            maxR = r;
        }
    }
    for(i=0;i<n;++i) {
        vec3Mult(diffs[i],diffs[i],1.0/dists[i]);
        vec3Copy(s->verts[i].normal,diffs[i]);
        vec3Mult(diffs[i],diffs[i],maxR);
        vec3Add(s->verts[i].loc,centroid,diffs[i]);
    }
    free(dists);
    free(diffs);
}

