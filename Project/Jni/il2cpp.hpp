#pragma once
#include <cstdint>
#include <cstdio>
#include <dlfcn.h>
#include <vector>
#include <string>
#include "memory.h"

typedef uintptr_t Il2CppObject;
typedef uintptr_t Il2CppClass;
typedef uintptr_t Il2CppMethod;
typedef uintptr_t Il2CppField;
typedef uintptr_t Il2CppType;
typedef uintptr_t Il2CppArray;
typedef uintptr_t Il2CppString;
typedef uintptr_t Il2CppImage;
typedef uintptr_t Il2CppAssembly;

namespace IL2CPP {

    namespace API {
        inline Il2CppClass* (*Class_FromName)(Il2CppImage* image, const char* namespaze, const char* name) = nullptr;
        inline Il2CppClass* (*Class_FromSystemType)(Il2CppType* type) = nullptr;
        inline const char*  (*Class_GetName)(Il2CppClass* klass) = nullptr;
        inline const char*  (*Class_GetNamespace)(Il2CppClass* klass) = nullptr;
        inline uintptr_t    (*Class_GetFieldFromName)(Il2CppClass* klass, const char* name) = nullptr;
        inline uintptr_t    (*Class_GetMethodFromName)(Il2CppClass* klass, const char* name, int argsCount) = nullptr;
        inline uintptr_t    (*Class_GetPropertiesFromName)(Il2CppClass* klass, const char* name) = nullptr;
        inline uintptr_t    (*Field_GetValue)(Il2CppObject* obj, uintptr_t field, void* value) = nullptr;
        inline uintptr_t    (*Field_GetStaticValue)(uintptr_t field, void* value) = nullptr;
        inline const char*  (*Field_GetName)(uintptr_t field) = nullptr;
        inline uintptr_t    (*Field_GetType)(uintptr_t field) = nullptr;
        inline Il2CppObject* (*Object_New)(Il2CppClass* klass) = nullptr;
        inline Il2CppString* (*String_New)(const char* str) = nullptr;
        inline Il2CppArray*  (*Array_New)(Il2CppClass* elementClass, uintptr_t length) = nullptr;
        inline Il2CppImage*  (*Assembly_GetImage)(Il2CppAssembly* assembly) = nullptr;
        inline Il2CppAssembly** (*GetAssemblies)(void* domain, size_t* count) = nullptr;
        inline void*         (*Domain_Get)() = nullptr;
        inline Il2CppObject* (*RuntimeInvoke)(uintptr_t method, void* obj, void** params, Il2CppObject** exc) = nullptr;
        inline uintptr_t     (*Method_GetParamCount)(uintptr_t method) = nullptr;
        inline const char*   (*Method_GetName)(uintptr_t method) = nullptr;
        inline uintptr_t (*ResolveICall)(const char* name) = nullptr;
    }

    inline bool Initialize() {
        void* handle = dlopen("libil2cpp.so", RTLD_NOLOAD | RTLD_NOW);
        if (!handle) {
            handle = dlopen("libil2cpp.so", RTLD_NOW | RTLD_GLOBAL);
            if (!handle) {
                fprintf(stderr, "[ESP] Failed to load libil2cpp.so\n");
                return false;
            }
        }

        #define RESOLVE(name) \
            API::name = (decltype(API::name))dlsym(handle, "il2cpp_" #name); \
            if (!API::name) { fprintf(stderr, "[ESP] Failed to resolve il2cpp_" #name "\n"); }

        RESOLVE(Class_FromName);
        RESOLVE(Class_GetName);
        RESOLVE(Class_GetNamespace);
        RESOLVE(Class_GetFieldFromName);
        RESOLVE(Class_GetMethodFromName);
        RESOLVE(Field_GetValue);
        RESOLVE(Field_GetStaticValue);
        RESOLVE(Field_GetName);
        RESOLVE(Field_GetType);
        RESOLVE(Object_New);
        RESOLVE(String_New);
        RESOLVE(Array_New);
        RESOLVE(Assembly_GetImage);
        RESOLVE(Domain_Get);
        RESOLVE(RuntimeInvoke);
        RESOLVE(Method_GetParamCount);
        RESOLVE(Method_GetName);

        API::ResolveICall = (decltype(API::ResolveICall))dlsym(handle, "il2cpp_resolve_icall");
        API::GetAssemblies = (decltype(API::GetAssemblies))dlsym(handle, "il2cpp_get_assemblies");
        if (!API::GetAssemblies)
            API::GetAssemblies = (decltype(API::GetAssemblies))dlsym(handle, "il2cpp_domain_get_assemblies");

        dlclose(handle);
        return true;
    }

    inline Il2CppAssembly* FindAssembly(const char* name) {
        if (!API::Domain_Get || !API::GetAssemblies) return nullptr;
        size_t count = 0;
        Il2CppAssembly** assemblies = API::GetAssemblies(API::Domain_Get(), &count);
        if (!assemblies) return nullptr;
        for (size_t i = 0; i < count; i++) {
            Il2CppImage* image = API::Assembly_GetImage(assemblies[i]);
            if (!image) continue;
            const char* imgName = reinterpret_cast<const char*>(image + 0x10);
            if (imgName && strstr(imgName, name)) {
                return assemblies[i];
            }
        }
        return nullptr;
    }

    inline Il2CppClass* FindClass(const char* namespaze, const char* name) {
        Il2CppAssembly* assembly = FindAssembly("Assembly-CSharp");
        if (!assembly) return nullptr;
        Il2CppImage* image = API::Assembly_GetImage(assembly);
        if (!image) return nullptr;
        return API::Class_FromName(image, namespaze, name);
    }

    template<typename... Args>
    inline Il2CppObject* InvokeStatic(const char* namespaze, const char* className, 
                                       const char* methodName, Args... args) {
        Il2CppClass* klass = FindClass(namespaze, className);
        if (!klass) return nullptr;
        uintptr_t method = API::Class_GetMethodFromName(klass, methodName, sizeof...(args));
        if (!method) return nullptr;
        void* params[] = { (void*)(uintptr_t)args... };
        Il2CppObject* exc = nullptr;
        return API::RuntimeInvoke(method, nullptr, params, &exc);
    }

    template<typename T>
    inline T GetFieldValue(Il2CppObject* obj, const char* fieldName) {
        if (!obj || !API::Class_GetFieldFromName) return T{};
        uintptr_t klassPtr = Memory::Read<uintptr_t>(obj);
        uintptr_t field = API::Class_GetFieldFromName(klassPtr, fieldName);
        if (!field) return T{};
        T value;
        API::Field_GetValue(obj, field, &value);
        return value;
    }
}