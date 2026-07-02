#pragma once
#include <cstdint>
#include "utils.h"

namespace Draw {
    bool Initialize();
    void BeginFrame();
    void Line(Vector2 from, Vector2 to, Color color, float thickness = 1.0f);
    void Rect(Vector2 pos, float width, float height, Color color, float thickness = 1.0f);
    void FilledRect(Vector2 pos, float width, float height, Color color);
    void Circle(Vector2 center, float radius, Color color, int segments = 24);
    void Text(const char* text, Vector2 pos, Color color, float size = 16.0f);
    void BoxESP(Vector2 foot, Vector2 head, Color color, float thickness = 1.5f);
    void HealthBar(Vector2 pos, float height, int health, int maxHealth = 100);
    void Snapline(Vector2 footPos, Color color);
    void EndFrame();
    void Shutdown();
}