#include "dml.h"

float lerp(float v0, float v1, float t) {
    if (t > 1) return v1;
    return (1 - t) * v0 + t * v1;
}

Vector lerp_2d(Vector start, Vector end, float t) {
    return (Vector) {
            lerp(start.x, end.x, t),
            lerp(start.y, end.y, t),
    };
}

Vector quadratic_2d(Vector start, Vector control, Vector end, float t) {
    return lerp_2d(
            lerp_2d(start, control, t),
            lerp_2d(control, end, t),
            t
    );
}