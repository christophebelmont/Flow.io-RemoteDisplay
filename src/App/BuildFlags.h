#pragma once

#if !defined(FLOW_PROFILE_FLOW_CONNECT_DISPLAY)
#error "The FlowConnectDisplay firmware profile must be selected."
#endif

#define FLOW_BUILD_IS_FLOWIO 0
#define FLOW_BUILD_IS_WAVESHARE 0
#define FLOW_BUILD_IS_SUPERVISOR 0
#define FLOW_BUILD_IS_FLOW_CONNECT_DISPLAY 1
#define FLOW_BUILD_IS_MICRONOVA 0
#define FLOW_BUILD_PROFILE_NAME "FlowConnectDisplay"
