#pragma once

#include "Board/FlowIODINBoard.h"

namespace BoardCapacityProfile {

inline constexpr const IoCapacitySpec& kIoCapacity = BoardProfiles::kFlowIODINv1.ioCapacity;
inline constexpr const HaCapacitySpec& kHaCapacity = BoardProfiles::kFlowIODINv1.haCapacity;

}  // namespace BoardCapacityProfile
