namespace Unity {

    // ============================================================
    // OFFSETS — YOU MUST UPDATE THESE FOR EACH GAME
    // Change the hex values below based on your reverse engineering
    // ============================================================
    namespace Offsets {
        // --- Player class offsets (from Player object) ---
        inline uint32_t Health         = 0x68;   // <-- CHANGE THIS
        inline uint32_t MaxHealth      = 0x6C;   // <-- CHANGE THIS
        inline uint32_t Team           = 0x70;   // <-- CHANGE THIS
        inline uint32_t IsAlive        = 0x74;   // <-- CHANGE THIS
        inline uint32_t IsVisible      = 0x75;   // <-- CHANGE THIS
        inline uint32_t PlayerName     = 0x78;   // <-- CHANGE THIS
        inline uint32_t HeadPos        = 0x88;   // <-- CHANGE THIS

        // --- GameObject/Transform offsets ---
        inline uint32_t GameObjectToTransform = 0x10;  // <-- usually stays 0x10
        inline uint32_t TransformPosition     = 0x38;  // <-- usually stays 0x38
        inline uint32_t TransformRotation     = 0x44;  // <-- usually stays 0x44

        // --- Camera offsets ---
        inline uint32_t CameraViewMatrix  = 0x2C0;  // <-- CHANGE THIS
        inline uint32_t CameraProjMatrix  = 0x300;  // <-- CHANGE THIS

        // --- Tag / Layer filtering ---
        inline int32_t PlayerLayer  = 8;    // <-- CHANGE THIS
        inline int32_t PlayerTag    = 0;    // <-- CHANGE THIS
    }