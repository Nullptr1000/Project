#include "draw.h"
#include <cstdio>
#include <cstring>
#include <cmath>
#include <vector>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

static const char* g_vertexShaderSource = 
    "attribute vec2 aPos;\n"
    "uniform vec2 uScreenSize;\n"
    "void main() {\n"
    "    vec2 clip = (aPos / uScreenSize) * 2.0 - 1.0;\n"
    "    clip.y = -clip.y;\n"
    "    gl_Position = vec4(clip, 0.0, 1.0);\n"
    "}\n";

static const char* g_fragmentShaderSource =
    "precision mediump float;\n"
    "uniform vec4 uColor;\n"
    "void main() {\n"
    "    gl_FragColor = uColor;\n"
    "}\n";

static GLuint g_shaderProgram = 0;
static GLuint g_vbo = 0;
static GLint g_aPosLoc = -1;
static GLint g_uScreenSizeLoc = -1;
static GLint g_uColorLoc = -1;

namespace Draw {
    struct DrawCommand {
        enum Type { LINE, FILLED_RECT, RECT, CIRCLE, TEXT };
        Type type;
        Vector2 p1, p2;
        Color color;
        float thickness;
        float radius;
        int segments;
        char text[128];
        float textSize;
    };

    static std::vector<DrawCommand> s_commands;
    static bool s_initialized = false;

    GLuint CompileShader(GLenum type, const char* source) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);
        GLint status;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (!status) {
            char log[512];
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            fprintf(stderr, "[Draw] Shader compile error: %s\n", log);
            glDeleteShader(shader);
            return 0;
        }
        return shader;
    }

    bool Initialize() {
        if (s_initialized) return true;
        GLuint vs = CompileShader(GL_VERTEX_SHADER, g_vertexShaderSource);
        GLuint fs = CompileShader(GL_FRAGMENT_SHADER, g_fragmentShaderSource);
        if (!vs || !fs) return false;
        g_shaderProgram = glCreateProgram();
        glAttachShader(g_shaderProgram, vs);
        glAttachShader(g_shaderProgram, fs);
        glLinkProgram(g_shaderProgram);
        GLint status;
        glGetProgramiv(g_shaderProgram, GL_LINK_STATUS, &status);
        if (!status) {
            char log[512];
            glGetProgramInfoLog(g_shaderProgram, sizeof(log), nullptr, log);
            fprintf(stderr, "[Draw] Program link error: %s\n", log);
            return false;
        }
        glDeleteShader(vs);
        glDeleteShader(fs);
        g_aPosLoc = glGetAttribLocation(g_shaderProgram, "aPos");
        g_uScreenSizeLoc = glGetUniformLocation(g_shaderProgram, "uScreenSize");
        g_uColorLoc = glGetUniformLocation(g_shaderProgram, "uColor");
        glGenBuffers(1, &g_vbo);
        s_initialized = true;
        return true;
    }

    void BeginFrame() {
        if (!s_initialized) Initialize();
        s_commands.clear();
    }

    void Line(Vector2 from, Vector2 to, Color color, float thickness) {
        DrawCommand cmd;
        cmd.type = DrawCommand::LINE;
        cmd.p1 = from;
        cmd.p2 = to;
        cmd.color = color;
        cmd.thickness = thickness;
        s_commands.push_back(cmd);
    }

    void FilledRect(Vector2 pos, float width, float height, Color color) {
        DrawCommand cmd;
        cmd.type = DrawCommand::FILLED_RECT;
        cmd.p1 = pos;
        cmd.p2 = Vector2(pos.x + width, pos.y + height);
        cmd.color = color;
        s_commands.push_back(cmd);
    }

    void Rect(Vector2 pos, float width, float height, Color color, float thickness) {
        Line(pos, Vector2(pos.x + width, pos.y), color, thickness);
        Line(Vector2(pos.x, pos.y + height), Vector2(pos.x + width, pos.y + height), color, thickness);
        Line(pos, Vector2(pos.x, pos.y + height), color, thickness);
        Line(Vector2(pos.x + width, pos.y), Vector2(pos.x + width, pos.y + height), color, thickness);
    }

    void BoxESP(Vector2 foot, Vector2 head, Color color, float thickness) {
        float boxHeight = std::abs(foot.y - head.y);
        float boxWidth = boxHeight * 0.6f;
        Vector2 topLeft(head.x - boxWidth / 2, head.y);
        Rect(topLeft, boxWidth, boxHeight, color, thickness);
    }

    void HealthBar(Vector2 pos, float height, int health, int maxHealth) {
        float barWidth = 4.0f;
        float barX = pos.x - 20.0f;
        float healthPercent = static_cast<float>(health) / maxHealth;
        FilledRect(Vector2(barX, pos.y), barWidth, height, Color(0.2f, 0.2f, 0.2f, 0.8f));
        Color healthColor;
        if (healthPercent > 0.5f) healthColor = Color(0.0f, 1.0f, 0.0f, 0.9f);
        else if (healthPercent > 0.25f) healthColor = Color(1.0f, 1.0f, 0.0f, 0.9f);
        else healthColor = Color(1.0f, 0.0f, 0.0f, 0.9f);
        float fillHeight = height * healthPercent;
        FilledRect(Vector2(barX, pos.y + height - fillHeight), barWidth, fillHeight, healthColor);
    }

    void Snapline(Vector2 footPos, Color color) {
        Vector2 screenCenter(Screen::g_nWidth / 2.0f, Screen::g_nHeight);
        Line(footPos, screenCenter, color, 1.0f);
    }

    void Text(const char* text, Vector2 pos, Color color, float size) {
        DrawCommand cmd;
        cmd.type = DrawCommand::TEXT;
        cmd.p1 = pos;
        cmd.color = color;
        cmd.textSize = size;
        strncpy(cmd.text, text, sizeof(cmd.text) - 1);
        s_commands.push_back(cmd);
    }

    void EndFrame() {
        if (!s_initialized || s_commands.empty()) return;
        glUseProgram(g_shaderProgram);
        glUniform2f(g_uScreenSizeLoc, Screen::g_nWidth, Screen::g_nHeight);
        GLboolean blendEnabled = glIsEnabled(GL_BLEND);
        GLboolean depthTestEnabled = glIsEnabled(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        glBindBuffer(GL_ARRAY_BUFFER, g_vbo);
        glEnableVertexAttribArray(g_aPosLoc);
        glVertexAttribPointer(g_aPosLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        for (const auto& cmd : s_commands) {
            glUniform4f(g_uColorLoc, cmd.color.r, cmd.color.g, cmd.color.b, cmd.color.a);
            switch (cmd.type) {
                case DrawCommand::LINE: {
                    float verts[] = { cmd.p1.x, cmd.p1.y, cmd.p2.x, cmd.p2.y };
                    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
                    glLineWidth(cmd.thickness);
                    glDrawArrays(GL_LINES, 0, 2);
                    break;
                }
                case DrawCommand::FILLED_RECT: {
                    float verts[] = {
                        cmd.p1.x, cmd.p1.y, cmd.p2.x, cmd.p1.y,
                        cmd.p1.x, cmd.p2.y, cmd.p2.x, cmd.p2.y
                    };
                    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_DYNAMIC_DRAW);
                    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                    break;
                }
                default: break;
            }
        }
        glDisableVertexAttribArray(g_aPosLoc);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        if (!blendEnabled) glDisable(GL_BLEND);
        if (depthTestEnabled) glEnable(GL_DEPTH_TEST);
        glUseProgram(0);
    }

    void Shutdown() {
        if (g_vbo) glDeleteBuffers(1, &g_vbo);
        if (g_shaderProgram) glDeleteProgram(g_shaderProgram);
        g_vbo = 0;
        g_shaderProgram = 0;
        s_initialized = false;
    }
}