#pragma once
/**
 * @file FlowConnectDisplayUdpClientModule.h
 * @brief Flow Connect FCD UDP bridge between flow.io and a local Nextion.
 */

#include <WiFiUdp.h>

#include "Core/Hmi/HmiUdpProtocol.h"
#include "Core/Module.h"
#include "Core/NvsKeys.h"
#include "Core/Services/Services.h"
#include "Modules/HMIModule/Drivers/NextionDriver.h"

class FlowConnectDisplayUdpClientModule : public Module {
public:
    ModuleId moduleId() const override { return ModuleId::FlowConnectDisplayUdpClient; }
    const char* taskName() const override { return "fcdudp"; }
    BaseType_t taskCore() const override { return 1; }
    uint16_t taskStackSize() const override { return 4096; }
    uint8_t taskCount() const override { return 1; }
    const ModuleTaskSpec* taskSpecs() const override { return singleLoopTaskSpec(); }
    uint8_t dependencyCount() const override { return 2; }
    ModuleId dependency(uint8_t i) const override {
        if (i == 0) return ModuleId::LogHub;
        if (i == 1) return ModuleId::Wifi;
        return ModuleId::Unknown;
    }

    void init(ConfigStore& cfg, ServiceRegistry& services) override;
    void loop() override;

    bool begin();
    void tick(uint32_t nowMs);

private:
    static constexpr uint32_t HelloPeriodMs = 1000U;
    static constexpr uint32_t SleepHelloPeriodMs = 30000U;
    static constexpr uint32_t PingPeriodMs = 2000U;
    static constexpr uint32_t SleepPingPeriodMs = 30000U;
    // Three missed 2 s heartbeats detect a dead link while allowing packet loss.
    static constexpr uint32_t LinkTimeoutMs = 6000U;
    static constexpr uint32_t SleepLinkTimeoutMs = 120000U;
    static constexpr uint32_t AckRetryMs = 150U;
    static constexpr uint8_t AckMaxAttempts = 7U;
    static constexpr uint32_t HomeRefreshThrottleMs = 10000U;
    static constexpr uint32_t PageProbePeriodMs = 5000U;
    static constexpr uint32_t VersionProbeRetryMs = 2000U;
    static constexpr uint32_t VersionRecheckPeriodMs = 60000U;
    static constexpr uint32_t ConfigRenderDelayMs = 120U;
    static constexpr uint32_t ConfigRenderRetryMs = 320U;
    static constexpr uint8_t ConfigRenderPasses = 3U;
    static constexpr uint32_t ConfigValuesDelayMs = 80U;
    static constexpr uint8_t ConfigValuesPasses = 2U;
    static constexpr uint8_t NEXTION_EVENT_QUEUE_SIZE = 6U;
    static constexpr uint32_t InputLockMaxMs = 5000U;
    static constexpr uint16_t NextionSleepNoTouchSeconds = 30U;
    static constexpr const char* FlowConnectionStateObject = "tFConnectState";
    static constexpr const char* FlowCommunicationValueObject = "globals.vaFlowComm";

    struct ConfigData {
        char token[33]{};
    } cfgData_{};

    ConfigVariable<char,0> tokenVar_{
        NVS_KEY(NvsKeys::FlowConnectDisplay::UdpToken), "token", "fcd/udp",
        ConfigType::CharArray, cfgData_.token, ConfigPersistence::Persistent, sizeof(cfgData_.token)
    };

    WiFiUDP udp_{};
    NextionDriver nextion_{};
    ConfigStore* cfgStore_ = nullptr;
    const WifiService* wifiSvc_ = nullptr;

    uint8_t rxBuf_[HMI_UDP_MAX_PACKET]{};
    uint8_t txBuf_[HMI_UDP_MAX_PACKET]{};

    IPAddress flowIp_{};
    uint16_t flowPort_ = HMI_UDP_PORT;
    bool started_ = false;
    bool nextionReadyLogged_ = false;
    bool nextionDetectedLogged_ = false;
    bool nextionProbeTimeoutLogged_ = false;
    bool linked_ = false;
    bool discoveryWarningLogged_ = false;
    bool lostShown_ = false;
    bool inputLocked_ = false;
    bool flowConnectInitialized_ = false;
    bool flowConnectVisible_ = false;
    bool flowCommunicationValueKnown_ = false;
    bool flowCommunicationValue_ = false;
    bool configPageActive_ = false;
    bool wifiFactoryResetPending_ = false;
    uint8_t lastFlowConnectPage_ = 0xFFU;

    uint16_t txSeq_ = 1;
    uint16_t lastAck_ = 0;
    uint16_t lastRxSeq_ = 0;

    uint32_t lastHelloMs_ = 0;
    uint32_t lastPingMs_ = 0;
    uint32_t lastSeenMs_ = 0;
    uint32_t lastHomeRefreshRequestMs_ = 0;
    uint32_t lastPageProbeMs_ = 0;
    uint32_t lastVersionProbeMs_ = 0;
    uint32_t discoveryStartedMs_ = 0;
    uint32_t nextionProbeStartedMs_ = 0;
    uint32_t nextionLastProbeMs_ = 0;
    uint32_t inputLockedAtMs_ = 0;
    uint32_t wifiFactoryResetAtMs_ = 0;
    uint8_t lastLoggedNextionPage_ = 0xFFU;
    uint8_t lastLoggedLogicalPage_ = 0xFFU;
    uint8_t lastLoggedConfigPage_ = 0xFFU;
    uint8_t lastLoggedConfigPageCount_ = 0xFFU;
    uint8_t lastLoggedConfigRows_ = 0xFFU;
    ConfigMenuMode lastLoggedConfigMode_ = (ConfigMenuMode)0xFFU;
    char lastLoggedConfigPath_[sizeof(ConfigMenuView::breadcrumb)]{};

    uint8_t pendingBuf_[HMI_UDP_MAX_PACKET]{};
    size_t pendingLen_ = 0;
    uint16_t pendingSeq_ = 0;
    HmiUdpMsgType pendingType_ = HmiUdpMsgType::Error;
    uint8_t pendingAttempts_ = 0;
    uint32_t pendingLastSendMs_ = 0;

    ConfigMenuView configView_{};
    bool configBatchActive_ = false;
    bool configRenderPending_ = false;
    uint8_t configRenderRemaining_ = 0;
    uint32_t configRenderDueMs_ = 0;
    bool configValuesPending_ = false;
    uint8_t configValuesRemaining_ = 0;
    uint32_t configValuesDueMs_ = 0;

    HmiEvent eventQueue_[NEXTION_EVENT_QUEUE_SIZE]{};
    uint8_t eventHead_ = 0;
    uint8_t eventTail_ = 0;

    void sendHello_(uint32_t nowMs, bool force = false);
    void probeNextionPresence_(uint32_t nowMs);
    void probeNextionVersion_(uint32_t nowMs, bool force = false);
    void sendPing_(uint32_t nowMs, uint32_t periodMs = PingPeriodMs);
    void readUdp_(uint32_t nowMs);
    void handlePacket_(const HmiUdpHeader& header, const uint8_t* payload, uint32_t nowMs);
    bool sendPacket_(HmiUdpMsgType type, const void* payload, uint8_t payloadLen, uint8_t flags = 0);
    bool sendAck_(uint16_t seq);
    void pollNextion_();
    void serviceEventTx_();
    bool enqueueEvent_(const HmiEvent& event);
    bool dequeueEvent_(HmiEvent& out);
    bool peekEvent_(HmiEvent& out) const;
    void popEvent_();
    bool sendEvent_(const HmiEvent& event);
    bool queueReliablePacket_(HmiUdpMsgType type, const void* payload, uint8_t payloadLen);
    bool sendRtcReadResponse_();
    bool handleLocalCommand_(const HmiEvent& event);
    bool requestWifiFactoryReset_();
    void serviceWifiFactoryReset_(uint32_t nowMs);
    void requestFullRefresh_(const char* reason, bool force = false);
    void servicePageProbe_(uint32_t nowMs);
    void scheduleConfigRender_(uint32_t nowMs, const char* reason);
    void scheduleConfigValuesRefresh_(uint32_t nowMs, const char* reason);
    void serviceConfigRendering_(uint32_t nowMs);
    void resetConfigView_();
    void setInputLocked_(bool locked, const char* reason, uint32_t nowMs);
    void logDisplayState_(const char* reason, bool force = false);
    void logEvent_(const char* prefix, const HmiEvent& event) const;
    static bool isMenuEvent_(HmiEventType type);
    void servicePendingAck_(uint32_t nowMs);
    void setFlowConnectionVisible_(bool visible, const char* reason, bool force = false);
    void setFlowCommunicationValue_(bool active, const char* reason, bool force = false);
    void markSeen_(uint32_t nowMs);
    void handleLinkLost_();
    bool wifiConnected_() const;
    static void copyText_(char* out, size_t outLen, const char* in);
    static void nextionDebugCallback_(void* ctx, const char* kind, const uint8_t* data, uint8_t len);
    static const char* msgTypeName_(HmiUdpMsgType type);
    static const char* eventTypeName_(HmiEventType type);
    static const char* commandName_(HmiCommandId command);
    static const char* menuModeName_(ConfigMenuMode mode);
    static const char* widgetName_(ConfigMenuWidget widget);
};
