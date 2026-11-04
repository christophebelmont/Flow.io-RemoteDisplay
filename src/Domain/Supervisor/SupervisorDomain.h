#pragma once

#include "Domain/DomainSpec.h"
#include "Domain/Supervisor/SupervisorDefaults.h"

namespace SupervisorDomain {

inline constexpr DomainSpec kSupervisorDomain{
    "Supervisor",
    nullptr,
    0,
    nullptr,
    0,
    nullptr,
    0,
    nullptr,
    nullptr
};

}  // namespace SupervisorDomain
