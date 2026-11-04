#include "Board/BoardCatalog.h"

#include "App/BuildFlags.h"
#include "Board/FlowIODINBoard.h"

namespace BoardCatalog {

const BoardSpec& flowIODINv1()
{
    return BoardProfiles::kFlowIODINv1;
}

const BoardSpec& activeBoard()
{
    return flowIODINv1();
}

}  // namespace BoardCatalog
