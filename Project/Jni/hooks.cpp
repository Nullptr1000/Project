#include "hooks.h"
#include "draw.h"
#include "esp.h"
#include "aimbot.h"
#include "unity.h"
#include "memory.h"
#include <cstdio>
#include <dlfcn.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>

namespace Hooks {
    static EGLBoolean (*original_eglSwapBuffers)(EGLDisplay dpy, EGLSurface surface) = nullptr;
    static Unity::EntityManager* g_entityManager = nullptr;

    static EGLBoolean hook_eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        Screen::g_nWidth = viewport[2];
        Screen::g_nHeight = viewport[3];
        Draw::BeginFrame();
        if (g_entityManager) {
            g_entityManager->Update();
            const auto& entities = g_entityManager->GetEntities();
            Vector3 localPos = g_entityManager->GetLocalPosition();
            ESP::Render(entities, localPos);
            Aimbot::Update(entities, localPos);
        }
        Draw::EndFrame();
        return original_eglSwapBuffers(dpy, surface);
    }

    bool Initialize() {
        g_entityManager = new Unity::EntityManager();
        if (!g_entityManager) return false;
        if (!IL2CPP::Initialize()) {
            fprintf(stderr, "[Hooks] IL2CPP init failed\n");
        } else {
            Unity::ResolveEngineAPI();
        }
        HookEGL();
        return true;
    }

    void HookEGL() {
        void* eglHandle = dlopen("libEGL.so", RTLD_NOLOAD | RTLD_NOW);
        if (!eglHandle) {
            eglHandle = dlopen("libEGL.so", RTLD_NOW | RTLD_GLOBAL);
            if (!eglHandle) {
                fprintf(stderr, "[Hooks] Failed to load libEGL.so\n");
                return;
            }
        }
        void* eglSwapBuffersAddr = dlsym(eglHandle, "eglSwapBuffers");
        if (!eglSwapBuffersAddr) {
            fprintf(stderr, "[Hooks] Failed to find eglSwapBuffers\n");
            dlclose(eglHandle);
            return;
        }
        original_eglSwapBuffers = reinterpret_cast<decltype(original_eglSwapBuffers)>(eglSwapBuffersAddr);
        uintptr_t hookFunc = reinterpret_cast<uintptr_t>(hook_eglSwapBuffers);
        if (Memory::HookFunction(reinterpret_cast<uintptr_t>(eglSwapBuffersAddr), hookFunc)) {
            fprintf(stderr, "[Hooks] eglSwapBuffers hooked successfully\n");
        } else {
            fprintf(stderr, "[Hooks] Inline hook failed\n");
        }
        dlclose(eglHandle);
    }

    void Shutdown() {
        delete g_entityManager;
        g_entityManager = nullptr;
        Draw::Shutdown();
    }
}