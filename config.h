#ifndef RT_CONFIG_H
#define RT_CONFIG_H

#include "precision.h"


#define min(A,B) (((A) < (B)) ? (A) : (B))
#define max(A,B) (((A) > (B)) ? (A) : (B))
#define clamp(X,A,B) (min(max((A),(X)),(B)))

//distances
#define RT_INFINITY real(10000)
#define BACKGROUND real(9000)

//index of refraction
#define REFRACTION real(1.1)

//number of objects and maximum tracing depth
#define NUM_OBJECTS 12
#define MAX_DEPTH 6
#define NUM_NODES (1 << (MAX_DEPTH + 1)) - 1


//epsilon for intersections
#define EPSILON real(0.001)


#endif
