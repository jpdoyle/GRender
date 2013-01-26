#include "Vertex.h"
#include <stdlib.h>

VertexArray* createVertArray(unsigned numAttributes,
                             const VertexAttribute* attributes) {
   VertexArray* ret   = malloc(sizeof(VertexArray));
   ret->numAttributes = numAttributes;
   ret->attributes    = attributes;
   if(numAttributes) {
       ret->attributePtrs = malloc(numAttributes*sizeof(float*));
   } else {
       ret->attributePtrs = NULL;
   }

   return ret;
}

void freeVertArray(VertexArray* arr) {
    if(!arr) {
        return;
    }
    if(arr->attributePtrs) {
        free(arr->attributePtrs);
    }
    free(arr);
}

Vertex* createVertex(VertexArray* arr) {
    Vertex* ret = malloc(sizeof(Vertex));
    ret->numAttributes = arr->numAttributes;
    ret->attributes    = arr->attributes;
    if(ret->numAttributes) {
        ret->attributePtrs = malloc(ret->numAttributes
                                    *sizeof(float*));
    } else {
        ret->attributePtrs = NULL;
    }
    return ret;
}

void freeVertex(Vertex* vert) {
    if(!vert) {
        return;
    }
    if(vert->attributePtrs) {
        free(vert->attributePtrs);
    }
    free(vert);
}

void vertAt(Vertex* out,VertexArray* arr,unsigned i) {
    unsigned locDiff   = arr->locStep*i,
             colorDiff = arr->colorStep*i;
    if(arr->locs) {
        out->loc = (Vec3*)(((void*)arr->locs)+locDiff);
    }
    if(arr->colors) {
        out->color = (Color3*)(((void*)arr->colors)+colorDiff);
    }
    unsigned j;
    for(j=0;j<arr->numAttributes;++j) {
        unsigned diff = arr->attributes[j].step;
        out->attributePtrs[j] = (float*)(((void*)arr->attributePtrs[j]+diff));
    }
}

