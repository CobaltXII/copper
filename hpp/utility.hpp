#pragma once

#define fmin(a, b) ((a) < (b) ? (a) : (b))
#define fmax(a, b) ((a) > (b) ? (a) : (b))

#define EPSILON 1e-3f

std::default_random_engine seed;

std::uniform_real_distribution<float> rand00(0.0f, 1.0f);