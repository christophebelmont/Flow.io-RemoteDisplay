#include "Profiles/FlowConnectDisplay/FlowConnectDisplayProfile.h"

#include "Board/BoardCatalog.h"
#include "Core/FirmwareVersion.h"
#include "Domain/DomainCatalog.h"

namespace Profiles {
namespace FlowConnectDisplay {

const FirmwareProfile& profile()
{
    static const FirmwareProfile kProfile{
        "FlowConnectDisplay",
        &BoardCatalog::flowIODINv1(),
        &DomainCatalog::supervisor(),
        {
            "Flow Connect Display",
            "flow-connect-display",
            FirmwareVersion::Full,
            "rt"
        },
        nullptr,
        setupProfile,
        loopProfile
    };
    return kProfile;
}

}  // namespace FlowConnectDisplay
}  // namespace Profiles
