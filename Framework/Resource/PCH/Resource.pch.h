#pragma once

// Utils
#include <stdio.h>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <map>
#include <cassert>
// Utils Continued
// {
#define ZERO_MEM(a) memset(a, 0, sizeof(a))
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
// }


// Math
#include "MathHeaders.h"

// fbxsdk
#include "fbxsdk.h"

#include "GraphicsFrameworkHeader.h"

void FbxAMatrixToMat4(const FbxAMatrix* _in, glm::mat4& _out);
int iClamp(int value, int min, int max);

typedef glm::simdVec4 fcvec4;
typedef glm::simdMat4 fcmat4;
