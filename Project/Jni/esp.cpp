#include "esp.h"
#include <cstdio>
#include <algorithm>

namespace ESP {

    void Render(const std::vector<Unity::PlayerEntity>& entities, const Vector3& localPos) {
        if (!Config::g_bEspEnabled || entities.empty()) return;
        for (const auto& player : entities) {
            if (player.isLocal) continue;
            if (!player.isAlive) continue;
            if (player.distance > Config::g_fMaxRenderDist) continue;
            if (player.screenPos.z < 0.01f) continue;
            DrawPlayer(player, localPos);
        }
    }

    void DrawPlayer(const Unity::PlayerEntity& player, const Vector3& localPos) {
        Vector2 footPos(player.screenPos.x, player.screenPos.y);
        Vector2 headPos(player.headScreenPos.x, player.headScreenPos.y);
        Color espColor = (player.team == 0)
            ? Color(Config::g_fColorEnemy[0], Config::g_fColorEnemy[1],
                    Config::g_fColorEnemy[2], Config::g_fColorEnemy[3])
            : Color(Config::g_fColorTeam[0], Config::g_fColorTeam[1],
                    Config::g_fColorTeam[2], Config::g_fColorTeam[3]);
        if (Config::g_bEspBox)
            Draw::BoxESP(footPos, headPos, espColor, 1.5f);
        if (Config::g_bEspLine)
            Draw::Snapline(footPos, Color(1, 1, 1, 0.7f));
        if (Config::g_bEspHealth)
            Draw::HealthBar(headPos, std::abs(footPos.y - headPos.y), player.health);
        if (Config::g_bEspDistance) {
            char distText[32];
            snprintf(distText, sizeof(distText), "%.0fm", player.distance);
            Draw::Text(distText, Vector2(footPos.x - 15, footPos.y + 5), Color(1, 1, 1, 0.8f), 14.0f);
        }
        if (Config::g_bEspSkeleton)
            Draw::Text("SKEL", Vector2(headPos.x - 10, headPos.y - 20), Color(0, 1, 1, 0.8f), 12.0f);
    }

    void DrawBox(const Unity::PlayerEntity& player) {
        Vector2 foot(player.screenPos.x, player.screenPos.y);
        Vector2 head(player.headScreenPos.x, player.headScreenPos.y);
        Draw::BoxESP(foot, head, Color(1, 0, 0, 1));
    }

    void DrawSnapline(const Unity::PlayerEntity& player) {
        Vector2 foot(player.screenPos.x, player.screenPos.y);
        Draw::Snapline(foot, Color(1, 1, 1, 0.7f));
    }

    void DrawHealth(const Unity::PlayerEntity& player) {
        Vector2 head(player.headScreenPos.x, player.headScreenPos.y);
        Vector2 foot(player.screenPos.x, player.screenPos.y);
        Draw::HealthBar(head, std::abs(foot.y - head.y), player.health);
    }

    void DrawDistance(const Unity::PlayerEntity& player) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.0f", player.distance);
        Draw::Text(buf, Vector2(player.screenPos.x, player.screenPos.y + 10), Color(1, 1, 1, 0.8f));
    }

    void DrawSkeleton(const Unity::PlayerEntity& player) {
        // Placeholder — requires bone transform walking
    }
}