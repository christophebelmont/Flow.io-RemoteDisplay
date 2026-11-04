#include "Profiles/FlowConnectDisplay/FlowConnectDisplayProfile.h"

namespace Profiles {
namespace FlowConnectDisplay {

ModuleInstances& moduleInstances()
{
    static ModuleInstances instances{};
    return instances;
}

}  // namespace FlowConnectDisplay
}  // namespace Profiles
