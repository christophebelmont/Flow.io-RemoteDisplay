#pragma once

#include <stdint.h>

struct AppContext;
struct BoardSpec;
struct DomainSpec;

struct ProductIdentity {
    const char* productName = nullptr;
    const char* mdnsName = nullptr;
    const char* firmwareVersion = nullptr;
    const char* runtimeTopicRoot = nullptr;
};

struct SupervisorRuntimeOptions {
    uint32_t pirTimeoutMs = 60000U;
    uint32_t factoryResetHoldMs = 5000U;
};

struct FirmwareProfile {
    const char* name = nullptr;
    const BoardSpec* board = nullptr;
    const DomainSpec* domain = nullptr;
    ProductIdentity identity{};
    const SupervisorRuntimeOptions* supervisorRuntime = nullptr;
    void (*setup)(AppContext&) = nullptr;
    void (*loop)(AppContext&) = nullptr;
};
