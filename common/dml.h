//
// Doofy's Math library
//

#pragma once

typedef struct {
    float x;
    float y;
} Vector;

float lerp(float v0, float v1, float t);

Vector lerp_2d(Vector start, Vector end, float t);

Vector quadratic_2d(Vector start, Vector control, Vector end, float t);

