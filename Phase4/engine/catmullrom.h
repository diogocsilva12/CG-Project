#pragma once
#include <vector>
#include "engine.h"

void getGlobalCatmullRomPoint(float gt, const std::vector<Point>& points, float* pos, float* deriv);
void cross(const float* a, const float* b, float* res);
void normalize(float* v);
void buildRotMatrix(const float* x, const float* y, const float* z, float* m);