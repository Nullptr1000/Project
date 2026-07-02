#pragma once
#include "il2cpp.h"
#include "utils.h"
#include <cstdio>
#include <vector>

namespace Unity {

    inline bool ResolveEngineAPI() {
        if (!IL2CPP::API::ResolveICall) return false;
        const char* icalls[] = {
            "UnityEngine.Camera::get_main",
            "UnityEngine.Camera::WorldToScreenPoint_Vector3",
            "UnityEngine.Camera::get_fieldOfView",
            "UnityEngine.GameObject::Find",
            "UnityEngine.GameObject::get_transform",
            "UnityEngine.Transform::get_position",
            "UnityEngine.Transform::get_rotation",
            "UnityEngine.Component::get_gameObject",
            "UnityEngine.GameObject::get_activeSelf",
            "UnityEngine.Object::get_name",
            "UnityEngine.Object::get_hideFlags",
            nullptr
        };
        for (int i = 0; icalls[i]; i++) {
            uintptr_t addr = IL2CPP::API::ResolveICall(icalls[i]);
            if (addr) {
                // resolved addresses would go into a map here
            }
        }
        return true;
    }

    inline Vector3 WorldToScreen(Vector3 worldPos, Matrix4x4 viewProj) {
        Vector3 screen = viewProj.MultiplyPoint(worldPos);
        screen.x = (screen.x + 1.0f) * 0.5f * Screen::g_nWidth;
        screen.y = (1.0f - screen.y) * 0.5f * Screen::g_nHeight;
        return screen;
    }

    inline bool GetViewMatrix(Matrix4x4& viewMatrix, Matrix4x4& projMatrix) {
        return false;
    }

    struct PlayerEntity {
        Il2CppObject* gameObject;
        Il2CppObject* transform;
        Vector3 position;
        Vector3 screenPos;
        Vector3 headScreenPos;
        float distance;
        bool isVisible;
        bool isAlive;
        bool isLocal;
        int health;
        int team;
        char name[64];
        float height;
    };

    class EntityManager {
    private:
        std::vector<PlayerEntity> m_entities;
        Il2CppObject* m_localPlayer = nullptr;
        Vector3 m_localPos;

        bool FindEntities() {
            m_entities.clear();
            if (IL2CPP::API::ResolveICall) {
                uintptr_t findObjsOfType = IL2CPP::API::ResolveICall(
                    "UnityEngine.Object::FindObjectsOfType");
            }
            return !m_entities.empty();
        }

    public:
        bool Update() {
            return FindEntities();
        }

        const std::vector<PlayerEntity>& GetEntities() const {
            return m_entities;
        }

        Vector3 GetLocalPosition() const { return m_localPos; }
        Il2CppObject* GetLocalPlayer() const { return m_localPlayer; }
    };
}