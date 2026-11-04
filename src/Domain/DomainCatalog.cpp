#include "Domain/DomainCatalog.h"

#include "App/BuildFlags.h"
#include "Domain/Supervisor/SupervisorDomain.h"

namespace DomainCatalog {

const DomainSpec& supervisor()
{
    return SupervisorDomain::kSupervisorDomain;
}

const DomainSpec& activeDomain()
{
    return supervisor();
}

}  // namespace DomainCatalog
