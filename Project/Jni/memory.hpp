#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <vector>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>

namespace Memory {

    template<typename T>
    inline T Read(uintptr_t addr) {
        if (addr < 0x1000) return T{};
        return *reinterpret_cast<T*>(addr);
    }

    template<typename T>
    inline void Write(uintptr_t addr, T value) {
        if (addr < 0x1000) return;
        uintptr_t pageStart = addr & ~(getpagesize() - 1);
        mprotect((void*)pageStart, getpagesize(), PROT_READ | PROT_WRITE | PROT_EXEC);
        *reinterpret_cast<T*>(addr) = value;
    }

    inline uintptr_t ReadPtr(uintptr_t addr, int offset = 0) {
        return Read<uintptr_t>(addr + offset);
    }

    inline uintptr_t PatternScan(uintptr_t start, size_t size, const char* pattern, const char* mask) {
        size_t patternLen = strlen(mask);
        for (size_t i = 0; i < size - patternLen; i++) {
            bool found = true;
            for (size_t j = 0; j < patternLen; j++) {
                if (mask[j] == 'x' && 
                    *reinterpret_cast<uint8_t*>(start + i + j) != 
                    static_cast<uint8_t>(pattern[j])) {
                    found = false;
                    break;
                }
            }
            if (found) return start + i;
        }
        return 0;
    }

    inline uintptr_t FindModuleBase(const char* moduleName) {
        FILE* maps = fopen("/proc/self/maps", "r");
        if (!maps) return 0;
        char line[512];
        uintptr_t base = 0;
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, moduleName) && strstr(line, "r-xp")) {
                char* dash = strchr(line, '-');
                if (dash) {
                    *dash = '\0';
                    base = strtoul(line, nullptr, 16);
                    break;
                }
            }
        }
        fclose(maps);
        return base;
    }

    inline size_t GetModuleSize(const char* moduleName) {
        FILE* maps = fopen("/proc/self/maps", "r");
        if (!maps) return 0;
        char line[512];
        uintptr_t firstAddr = 0, lastAddr = 0;
        while (fgets(line, sizeof(line), maps)) {
            if (strstr(line, moduleName)) {
                char* dash = strchr(line, '-');
                if (!dash) continue;
                *dash = '\0';
                uintptr_t start = strtoul(line, nullptr, 16);
                char* space = strchr(dash + 1, ' ');
                if (!space) continue;
                *space = '\0';
                uintptr_t end = strtoul(dash + 1, nullptr, 16);
                if (firstAddr == 0) firstAddr = start;
                if (end > lastAddr) lastAddr = end;
            }
        }
        fclose(maps);
        return lastAddr - firstAddr;
    }

    inline uintptr_t PatternScanModule(const char* moduleName, const char* pattern, const char* mask) {
        uintptr_t base = FindModuleBase(moduleName);
        if (!base) return 0;
        size_t size = GetModuleSize(moduleName);
        if (!size) return 0;
        return PatternScan(base, size, pattern, mask);
    }

    inline bool HookFunction(uintptr_t target, uintptr_t destination) {
        if (!target || !destination) return false;
        uint32_t branchInstr;
        int32_t offset = (int32_t)(destination - target - 4);
        if (target & 1) {
            target &= ~1;
            int32_t offset2 = (int32_t)(destination - target - 4);
            if (offset2 < -16777216 || offset2 > 16777214) return false;
            return false; // Thumb hooking would go here
        }
        if (offset < -33554432 || offset > 33554431) return false;
        branchInstr = 0xEA000000 | ((offset >> 2) & 0x00FFFFFF);
        uintptr_t page = target & ~(getpagesize() - 1);
        mprotect((void*)page, getpagesize() * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
        *reinterpret_cast<uint32_t*>(target) = branchInstr;
        __builtin___clear_cache((char*)target, (char*)target + 4);
        return true;
    }
}