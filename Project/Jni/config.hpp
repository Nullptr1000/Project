#pragma once
#include <cstdint>
#include <cstring>

namespace Config {
    // Feature toggles
    inline bool g_bEspEnabled       = true;
    inline bool g_bEspBox           = true;
    inline bool g_bEspLine          = true;
    inline bool g_bEspHealth        = true;
    inline bool g_bEspDistance      = true;
    inline bool g_bEspSkeleton      = false;
    inline bool g_bAimbotEnabled    = true;
    inline bool g_bAimbotAutoFire   = false;
    inline bool g_bAimbotVisibleOnly = false;

    // Aimbot settings
    inline float g_fAimbotFov       = 15.0f;
    inline float g_fAimbotSmooth    = 5.0f;
    inline float g_fMaxRenderDist   = 300.0f;

    // ESP colors (RGBA)
    inline float g_fColorBox[4]     = { 1.0f, 0.0f, 0.0f, 1.0f };
    inline float g_fColorLine[4]    = { 1.0f, 1.0f, 1.0f, 1.0f };
    inline float g_fColorEnemy[4]   = { 1.0f, 0.0f, 0.0f, 1.0f };
    inline float g_fColorTeam[4]    = { 0.0f, 1.0f, 0.0f, 1.0f };

    // Game-dependent
    inline int g_iMaxPlayers = 64;
}