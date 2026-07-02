#include "aimbot.h"
#include <cmath>
#include <algorithm>
#include <cstdio>

namespace Aimbot {

    static Unity::PlayerEntity* g_currentTarget = nullptr;

    bool IsInFov(const Vector2& targetScreen, const Vector2& screenCenter, float fovDegrees) {
        float dx = targetScreen.x - screenCenter.x;
        float dy = targetScreen.y - screenCenter.y;
        float distance = std::sqrt(dx*dx + dy*dy);
        float maxDist = (fovDegrees / 90.0f) * (screenCenter.x);
        return distance <= maxDist;
    }

    Unity::PlayerEntity* FindTarget(const std::vector<Unity::PlayerEntity>& entities,
                                     const Vector3& localPos,
                                     const Vector2& screenCenter) {
        Unity::PlayerEntity* bestTarget = nullptr;
        float bestDistance = 999999.0f;
        for (const auto& player : entities) {
            if (player.isLocal) continue;
            if (!player.isAlive) continue;
            if (player.distance > Config::g_fMaxRenderDist) continue;
            if (Config::g_bAimbotVisibleOnly && !player.isVisible) continue;
            Vector2 targetCenter(player.screenPos.x, player.screenPos.y);
            if (!IsInFov(targetCenter, screenCenter, Config::g_fAimbotFov)) continue;
            float dx = targetCenter.x - screenCenter.x;
            float dy = targetCenter.y - screenCenter.y;
            float dist = std::sqrt(dx*dx + dy*dy);
            if (dist < bestDistance) {
                bestDistance = dist;
                bestTarget = const_cast<Unity::PlayerEntity*>(&player);
            }
        }
        return bestTarget;
    }

    void AimAt(Unity::PlayerEntity* target, const Vector2& screenCenter) {
        if (!target) return;
        Vector2 targetPos(target->screenPos.x, target->screenPos.y);
        float dx = targetPos.x - screenCenter.x;
        float dy = targetPos.y - screenCenter.y;
        float smoothFactor = std::max(1.0f, Config::g_fAimbotSmooth);
        float moveX = dx / smoothFactor;
        float moveY = dy / smoothFactor;
        if (std::abs(dx) > 5.0f || std::abs(dy) > 5.0f) {
            // Touch input simulation via JNI or /dev/uinput goes here
        }
    }

    void Update(const std::vector<Unity::PlayerEntity>& entities, const Vector3& localPos) {
        if (!Config::g_bAimbotEnabled) {
            g_currentTarget = nullptr;
            return;
        }
        Vector2 screenCenter(Screen::g_nWidth / 2.0f, Screen::g_nHeight / 2.0f);
        Unity::PlayerEntity* target = FindTarget(entities, localPos, screenCenter);
        g_currentTarget = target;
        if (target) {
            AimAt(target, screenCenter);
            if (Config::g_bAimbotAutoFire) {
                // Auto-fire touch event goes here
            }
        }
    }
}