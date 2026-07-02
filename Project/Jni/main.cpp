#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <dlfcn.h>
#include <jni.h>
#include <android/log.h>
#include "hooks.h"
#include "draw.h"
#include "config.h"

#define LOG_TAG "UnityESP"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

void* HackThread(void* arg) {
    LOGI("[ESP] Hack thread started");
    void* lib = nullptr;
    int attempts = 0;
    while (!lib && attempts < 100) {
        lib = dlopen("libil2cpp.so", RTLD_NOLOAD | RTLD_NOW);
        if (!lib) {
            lib = dlopen("libunity.so", RTLD_NOLOAD | RTLD_NOW);
        }
        if (!lib) {
            usleep(100000);
            attempts++;
        }
    }
    if (!lib) {
        LOGE("[ESP] Failed to find Unity/IL2CPP library");
        return nullptr;
    }
    dlclose(lib);
    LOGI("[ESP] Unity library found, initializing...");
    sleep(1);
    if (!Draw::Initialize()) {
        LOGE("[ESP] Draw initialization failed");
        return nullptr;
    }
    if (!Hooks::Initialize()) {
        LOGE("[ESP] Hook initialization failed");
        return nullptr;
    }
    LOGI("[ESP] Hack initialized successfully!");
    LOGI("[ESP] ESP: %s, Aimbot: %s, Box: %s, Line: %s, Health: %s",
         Config::g_bEspEnabled ? "ON" : "OFF",
         Config::g_bAimbotEnabled ? "ON" : "OFF",
         Config::g_bEspBox ? "ON" : "OFF",
         Config::g_bEspLine ? "ON" : "OFF",
         Config::g_bEspHealth ? "ON" : "OFF");
    while (true) {
        sleep(10);
    }
    return nullptr;
}

extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    LOGI("[ESP] JNI_OnLoad called");
    pthread_t hackThread;
    pthread_create(&hackThread, nullptr, HackThread, nullptr);
    pthread_detach(hackThread);
    return JNI_VERSION_1_6;
}

__attribute__((constructor)) void Init() {
    LOGI("[ESP] Constructor called");
    pthread_t hackThread;
    pthread_create(&hackThread, nullptr, HackThread, nullptr);
    pthread_detach(hackThread);
}