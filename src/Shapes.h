#ifndef SHAPES_H
#define SHAPES_H

#include "Types.h"
#include "CustomVert.h"

typedef struct {
    CustomVert* verts;
    unsigned* indices;
    unsigned nVerts,nInds;
} Solid;

Solid* createSolid(unsigned nVerts,unsigned nInds);
void   freeSolid(Solid* s);

void copySolid(Solid* out,const Solid* in);

/* Returns a tetrahedron, divided by successive iterations of triangle
 * division.divisions == 0 gives a normal tetra. colors should have 4 
 * elements, in the order: top, near left, near right, back
 */
Solid* dividedTetra(unsigned divisions,Color4* colors);

void sphereify(Solid* s);

#endif
