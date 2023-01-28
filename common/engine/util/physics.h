#pragma once

#include "common/dml.h"
#include <furi.h>

typedef struct {
    Vector center;
    Vector corners[4];
} RectCollider;

typedef struct {
    Vector center;
    uint8_t radius;
} CircleCollider;

typedef struct {
    void *Collider;
    float bounciness;
    float friction;
    float weight;
    Vector velocity;
    bool use_gravity;
} PhysicsBody;