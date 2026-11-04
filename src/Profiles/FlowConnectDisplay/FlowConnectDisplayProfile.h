#pragma once

#include "App/FirmwareProfile.h"
#include "Modules/FlowConnectDisplay/FlowConnectDisplayUdpClientModule/FlowConnectDisplayUdpClientModule.h"
#include "Modules/EventBusModule/EventBusModule.h"
#include "Modules/Logs/LogDispatcherModule/LogDispatcherModule.h"
#include "Modules/Logs/LogHubModule/LogHubModule.h"
#include "Modules/Logs/LogSerialSinkModule/LogSerialSinkModule.h"
#include "Modules/Network/WifiModule/WifiModule.h"
#include "Modules/Network/WifiProvisioningModule/WifiProvisioningModule.h"
#include "Modules/Stores/ConfigStoreModule/ConfigStoreModule.h"
#include "Modules/Stores/DataStoreModule/DataStoreModule.h"

namespace Profiles {
namespace FlowConnectDisplay {

struct ModuleInstances {
    ModuleInstances() = default;

    LogHubModule logHubModule{};
    LogDispatcherModule logDispatcherModule{};
    LogSerialSinkModule logSerialSinkModule{};
    EventBusModule eventBusModule{};
    ConfigStoreModule configStoreModule{};
    DataStoreModule dataStoreModule{};
    WifiModule wifiModule{};
    WifiProvisioningModule wifiProvisioningModule{};
    FlowConnectDisplayUdpClientModule flowConnectDisplayUdpClientModule{};
};

ModuleInstances& moduleInstances();
const FirmwareProfile& profile();
void setupProfile(AppContext& ctx);
void loopProfile(AppContext& ctx);

}  // namespace FlowConnectDisplay
}  // namespace Profiles
