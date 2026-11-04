#pragma once

#include <Preferences.h>

#include "Core/ConfigStore.h"
#include "Core/ModuleManager.h"
#include "Core/ServiceRegistry.h"

struct BoardSpec;
struct DomainSpec;
struct FirmwareProfile;
struct ProductIdentity;
struct SupervisorRuntimeOptions;

struct AppContext {
    Preferences preferences{};
    ConfigStore registry{};
    ModuleManager moduleManager{};
    ServiceRegistry services{};
    const FirmwareProfile* profile = nullptr;
    const BoardSpec* board = nullptr;
    const DomainSpec* domain = nullptr;
    const ProductIdentity* identity = nullptr;
    const SupervisorRuntimeOptions* supervisorRuntime = nullptr;
    bool bootCompleted = false;
};
