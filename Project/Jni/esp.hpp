#pragma once
#include "unity.h"
#include "draw.h"
#include "config.h"
#include <vector>

namespace ESP {
    void Render(const std::vector<Unity::PlayerEntity>& entities, const Vector3& localPos);
    void DrawPlayer(const Unity::PlayerEntity& player, const Vector3& localPos);
    void DrawBox(const Unity::PlayerEntity& player);
    void DrawSnapline(const Unity::PlayerEntity& player);
    void DrawHealth(const Unity::PlayerEntity& player);
    void DrawDistance(const Unity::PlayerEntity& player);
    void DrawSkeleton(const Unity::PlayerEntity& player);
}