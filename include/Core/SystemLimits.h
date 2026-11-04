#pragma once

#include <stddef.h>
#include <stdint.h>

#include "Board/BoardCapacityProfile.h"

/**
 * @file SystemLimits.h
 * @brief Shared compile-time limits used across Core and modules.
 */

namespace Limits {

/** @brief Topic buffer length used by network snapshot publications. */
constexpr size_t TopicBuf = 70;
/** @brief JSON capacity for configuration patch parsing. */
constexpr size_t JsonPatchBuf = 1024;
/** @brief JSON capacity for command payload parsing. */
constexpr size_t JsonCmdBuf = 1024;
/** @brief JSON capacity for configuration payload parsing. */
constexpr size_t JsonCfgBuf = 1024;
/** @brief JSON capacity for command args parsing in `TimeModule::parseCmdArgsObject_`. */
constexpr size_t JsonCmdTimeBuf = 768;
/** @brief JSON capacity for command args parsing in `PoolDeviceModule::parseCmdArgsObject_`. */
constexpr size_t JsonCmdPoolDeviceBuf = 256;
/** @brief JSON capacity for `ConfigStore::applyJson` root document. Kept aligned with cfg patch size limits. */
constexpr size_t JsonConfigApplyBuf = JsonCfgBuf;
/** @brief Maximum number of registered config variables in `ConfigStore` metadata table.
 *  Sized for current FlowIO/Supervisor profiles with additional headroom for local TFT
 *  sensor/alarm slots, dashboard/LCD and PoolLogic extensions while staying bounded. */
constexpr size_t MaxConfigVars = 768;
/** @brief Maximum NVS key length (without null terminator) enforced by `ConfigTypes::NVS_KEY`. */
constexpr size_t MaxNvsKeyLen = 15;
/** @brief FreeRTOS log queue length used by `LogHub` (`LogHubModule::init`).
 *  Sized for high boot/runtime bursts on ESP32-S3 + Arduino 3.x while keeping bounded memory usage. */
constexpr uint16_t LogQueueLen = 256;
/** @brief FreeRTOS event queue length used by `EventBus` (`EventBus::QUEUE_LENGTH`).
 *  Sized to absorb startup bursts while limiting DRAM usage. */
constexpr uint8_t EventQueueLen = 40;
/** @brief Maximum number of EventBus subscribers (`EventBus::MAX_SUBSCRIBERS`). */
#ifndef FLOW_EVENT_SUBSCRIBERS_MAX
#define FLOW_EVENT_SUBSCRIBERS_MAX 50
#endif
constexpr uint8_t EventSubscribersMax = FLOW_EVENT_SUBSCRIBERS_MAX;

/** @brief Core runtime limits shared by the module framework. */
namespace Core {
namespace Capacity {
/** @brief Maximum number of modules registered in `ModuleManager`. */
constexpr size_t MaxModules = 28;
/** @brief Maximum number of declared tasks tracked by `ModuleManager`. */
constexpr size_t MaxModuleTasks = 32;
}  // namespace Capacity

namespace Task {
/** @brief Default FreeRTOS task stack size used by `Module::taskStackSize`. */
constexpr uint16_t DefaultStackSize = 3072;
}  // namespace Task

namespace Timing {
/** @brief Small cooperative delay inserted after each `Module::loop` execution. */
constexpr uint32_t LoopDelayMs = 10;
}  // namespace Timing
}  // namespace Core

/** @brief Configuration tree limits shared by local and remote explorers. */
namespace Config {
namespace Capacity {
/** @brief Maximum number of unique config branches returned by `ConfigStore::listModules`. */
constexpr uint8_t ModuleListMax = 160;
/** @brief FreeRTOS queue length for serialized ConfigStore persistence requests. */
constexpr uint8_t PersistenceQueueLen = 16;
/** @brief Maximum runtime blob payload length accepted by ConfigStore async persistence. */
constexpr size_t RuntimeBlobAsyncMax = 192;
}  // namespace Capacity
}  // namespace Config

/** @brief Default momentary digital output pulse duration in ms (`IOModule`). */
constexpr uint16_t MomentaryPulseMs = 500;
/** @brief Default periodic trace interval for ORP/pH/PSI calc logs (`IOModule`, `trace_period_ms`). */
constexpr uint32_t IoTracePeriodMs = 10000;
/** @brief HA command payload buffer length for IO output switches (`IOModule::haSwitchPayloadOn_/Off_`). */
constexpr size_t IoHaSwitchPayloadBuf = 128;

/** @brief IO module compile-time capacities resolved from the active board profile. */
namespace Io {
constexpr uint8_t MaxAnalogEndpoints = BoardCapacityProfile::kIoCapacity.analogEndpoints;
constexpr uint8_t MaxDigitalInputs = BoardCapacityProfile::kIoCapacity.digitalInputs;
constexpr uint8_t MaxDigitalOutputs = BoardCapacityProfile::kIoCapacity.digitalOutputs;
constexpr uint8_t AnalogConfigSlots = BoardCapacityProfile::kIoCapacity.analogConfigSlots;
constexpr uint8_t DigitalInputConfigSlots = BoardCapacityProfile::kIoCapacity.digitalInputConfigSlots;
constexpr uint8_t DigitalOutputConfigSlots = BoardCapacityProfile::kIoCapacity.digitalOutputConfigSlots;
#if defined(FLOW_BOARD_WAVESHARE_ESP32_S3)
constexpr uint8_t MaxDomainSlots = 20;
constexpr uint8_t MaxDomainIoSlotBindings = 20;
// pd07 remains the water-heater device; eight indices are therefore required
// even though the Waveshare domain has seven typed pool-device presets.
constexpr uint8_t MaxPoolDevices = 8;
#else
constexpr uint8_t MaxDomainSlots = 24;
constexpr uint8_t MaxDomainIoSlotBindings = 24;
constexpr uint8_t MaxPoolDevices = 16;
#endif

static_assert(MaxAnalogEndpoints > 0, "IO analogEndpoints must be at least 1");
static_assert(MaxDigitalInputs > 0, "IO digitalInputs must be at least 1");
static_assert(MaxDigitalOutputs > 0, "IO digitalOutputs must be at least 1");
static_assert(AnalogConfigSlots >= 6, "IO analogConfigSlots must keep legacy a00..a05 config vars");
static_assert(DigitalInputConfigSlots >= 5, "IO digitalInputConfigSlots must keep legacy i00..i04 config vars");
static_assert(DigitalOutputConfigSlots >= 8, "IO digitalOutputConfigSlots must keep legacy d00..d07 config vars");
static_assert(AnalogConfigSlots >= MaxAnalogEndpoints, "analog config slots must cover runtime analog endpoints");
static_assert(DigitalInputConfigSlots >= MaxDigitalInputs, "digital input config slots must cover runtime inputs");
static_assert(DigitalOutputConfigSlots >= MaxDigitalOutputs, "digital output config slots must cover runtime outputs");
static_assert(MaxDomainSlots > 0, "domain slot capacity must be non-zero");
static_assert(MaxDomainIoSlotBindings >= MaxDomainSlots, "domain IO binding capacity must cover domain slots");
static_assert(MaxPoolDevices > 0, "pool device capacity must be non-zero");
}  // namespace Io

/** @brief Alarm engine compile-time capacities and defaults. */
namespace Alarm {
/** @brief Maximum number of alarm slots managed by `AlarmModule`. */
constexpr uint16_t MaxAlarms = 16;
/** @brief Maximum JSON buffer used for alarm snapshot serialization. */
constexpr size_t SnapshotJsonBuf = 1536;
/** @brief Default alarm evaluation period in ms (`AlarmModule::loop`). */
constexpr uint32_t DefaultEvalPeriodMs = 250;
/** @brief JSON capacity for alarm command args parsing. */
constexpr size_t JsonCmdBuf = 256;
}  // namespace Alarm

/** @brief Shared WiFi runtime timings and buffers. */
namespace Wifi {
/** @brief Maximum number of scanned networks retained for provisioning UIs. */
#if defined(FLOW_PROFILE_FLOW_CONNECT_DISPLAY)
constexpr uint8_t MaxScanResults = 8;
#elif defined(FLOW_PROFILE_WAVESHARE)
constexpr uint8_t MaxScanResults = 12;
#else
constexpr uint8_t MaxScanResults = 24;
#endif

namespace Buffers {
/** @brief JSON document/output buffer used for WiFi scan status snapshots. */
#if defined(FLOW_PROFILE_FLOW_CONNECT_DISPLAY)
constexpr size_t ScanStatusJson = 1536;
#elif defined(FLOW_PROFILE_WAVESHARE)
constexpr size_t ScanStatusJson = 4096;
#else
constexpr size_t ScanStatusJson = 3072;
#endif
}  // namespace Buffers

namespace Timing {
/** @brief Minimum spacing between scan attempts when throttling WiFi scans. */
constexpr uint32_t ScanThrottleMs = 8000U;
/** @brief Initial delay before the first STA connect attempt after boot/config load. */
constexpr uint32_t InitialConnectDelayMs = 1200U;
/** @brief Startup window during which disconnect logs stay at debug level. */
constexpr uint32_t StartupTransientLogWindowMs = 10000U;
/** @brief Minimum spacing between repeated empty-SSID warnings. */
constexpr uint32_t EmptySsidLogIntervalMs = 10000U;
/** @brief Loop delay while WiFi runtime is disabled. */
constexpr uint32_t DisabledLoopDelayMs = 2000U;
/** @brief Loop delay while retries are disabled and WiFi is idle. */
constexpr uint32_t IdleRetryDisabledLoopDelayMs = 500U;
/** @brief Poll delay while waiting for the initial connect deadline. */
constexpr uint32_t IdleConnectPollDelayMs = 200U;
/** @brief Delay after issuing a STA connect attempt. */
constexpr uint32_t IdlePostConnectDelayMs = 1000U;
/** @brief Log cadence while the STA connect attempt is in progress. */
constexpr uint32_t ConnectingLogIntervalMs = 3000U;
/** @brief Delay before forcing a reconnect kick during connection attempts. */
constexpr uint32_t ReconnectKickDelayMs = 4000U;
/** @brief Timeout for a WiFi STA connect attempt before entering error wait. */
constexpr uint32_t ConnectTimeoutMs = 15000U;
/** @brief Loop delay while waiting for a WiFi STA connection. */
constexpr uint32_t ConnectingLoopDelayMs = 200U;
/** @brief Loop delay while WiFi remains connected. */
constexpr uint32_t ConnectedLoopDelayMs = 1000U;
/** @brief Error wait duration before retrying a failed WiFi connection. */
constexpr uint32_t ErrorWaitDurationMs = 5000U;
/** @brief Loop delay while staying in WiFi error-wait state. */
constexpr uint32_t ErrorWaitLoopDelayMs = 500U;
}  // namespace Timing
}  // namespace Wifi

/** @brief Home Assistant auto-discovery publication pacing limits. */
namespace Ha {
namespace Capacity {
constexpr uint8_t MaxSensors = BoardCapacityProfile::kHaCapacity.sensors;
constexpr uint8_t MaxBinarySensors = BoardCapacityProfile::kHaCapacity.binarySensors;
constexpr uint8_t MaxSwitches = BoardCapacityProfile::kHaCapacity.switches;
constexpr uint8_t MaxNumbers = BoardCapacityProfile::kHaCapacity.numbers;
constexpr uint8_t MaxButtons = BoardCapacityProfile::kHaCapacity.buttons;
constexpr uint8_t MaxSelects = BoardCapacityProfile::kHaCapacity.selects;

static_assert(MaxSensors > 0, "HA sensors capacity must be at least 1");
static_assert(MaxBinarySensors > 0, "HA binarySensors capacity must be at least 1");
static_assert(MaxSwitches > 0, "HA switches capacity must be at least 1");
static_assert(MaxNumbers > 0, "HA numbers capacity must be at least 1");
static_assert(MaxButtons > 0, "HA buttons capacity must be at least 1");
static_assert(MaxSelects > 0, "HA selects capacity must be at least 1");
}  // namespace Capacity

namespace Timing {
/** @brief Delay in ms between each HA discovery entity publish in `HAModule`. */
constexpr uint32_t DiscoveryStepMs = 200;
}  // namespace Timing
}  // namespace Ha

/** @brief Shared heap guards before network publications. */
namespace NetworkPublish {
/** @brief Minimum free 8-bit heap (bytes) required before attempting publish. */
constexpr uint32_t MinFreeHeapBytes = 4000U;
/** @brief Minimum largest 8-bit free block (bytes) required before attempting publish. */
constexpr uint32_t MinLargestBlockBytes = 4096U;
}  // namespace NetworkPublish

/** @brief Boot orchestration timings used in `main.cpp` staged startup. */
namespace Boot {
/** @brief Delays for services that may start during boot. */
#if defined(FLOW_PROFILE_MICRONOVA)
constexpr uint32_t WifiProvisioningStartDelayMs = 500;
constexpr uint32_t IoStartDelayMs = 8000;
constexpr uint32_t MicronovaBusStartDelayMs = 12000;
constexpr uint32_t MicronovaBoilerStartDelayMs = 16000;
constexpr uint32_t WebInterfaceStartDelayMs = 9000;
#else
constexpr uint32_t WifiProvisioningStartDelayMs = 0;
constexpr uint32_t IoStartDelayMs = 0;
constexpr uint32_t MicronovaBusStartDelayMs = 0;
constexpr uint32_t MicronovaBoilerStartDelayMs = 0;
constexpr uint32_t WebInterfaceStartDelayMs = 10000;
#endif
/** @brief Delay in ms before enabling HA auto-discovery publishing (`HAModule::setStartupReady`). */
constexpr uint32_t HaStartDelayMs = 15000;
/** @brief Delay in ms before enabling PoolLogic control loop (`PoolLogicModule::setStartupReady`). */
constexpr uint32_t PoolLogicStartDelayMs = 10000;
}  // namespace Boot

/** @brief Firmware update networking limits shared by the update module. */
namespace FirmwareUpdate {
namespace Buffers {
/** @brief JSON document capacity used to validate downloaded firmware manifests. */
constexpr size_t ManifestParseJson = 8192U;
/** @brief API response buffer for manifest checks (`/api/fwupdate/check`). */
constexpr size_t ManifestResponseJson = 8192U;
}  // namespace Buffers
namespace Http {
/** @brief HTTP connect timeout used for firmware and cfgdocs downloads. */
constexpr uint16_t ConnectTimeoutMs = 15000U;
/** @brief HTTP request timeout used once the download has started. */
constexpr uint32_t RequestTimeoutMs = 60000U;
/** @brief Timeout while waiting for the next chunk from a download stream. */
constexpr uint32_t StreamReadTimeoutMs = 15000U;
/** @brief Scratch buffer size used to copy download streams chunk by chunk. */
constexpr size_t StreamChunkBytes = 1024U;
}  // namespace Http
}  // namespace FirmwareUpdate

}  // namespace Limits
