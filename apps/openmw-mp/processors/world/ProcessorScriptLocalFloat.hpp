#ifndef OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP
#define OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP

#include "../WorldProcessor.hpp"

namespace mwmp
{
    class ProcessorScriptLocalFloat final: public WorldProcessor
    {
    public:
        ProcessorScriptLocalFloat()
        {
            BPP_INIT(ID_SCRIPT_LOCAL_FLOAT)
        }
    };
}

#endif //OPENMW_PROCESSORSCRIPTLOCALFLOAT_HPP
