#pragma once
#include <cstdint>

namespace Hooks {
    bool Initialize();
    void HookEGL();
    void HookInput();
    void Shutdown();
}