#pragma once
#include "unity.h"
#include "config.h"
#include "utils.h"
#include <vector>

namespace Aimbot {
    Unity::PlayerEntity* FindTarget(const std::vector<Unity::PlayerEntity>& entities,
                                     const Vector3& localPos,
                                     const Vector2& screenCenter);
    void AimAt(Unity::PlayerEntity* target, const Vector2& screenCenter);
    void Update(const std::vector<Unity::PlayerEntity>& entities, const Vector3& localPos);
    bool IsInFov(const Vector2& targetScreen, const Vector2& screenCenter, float fovDegrees);
}