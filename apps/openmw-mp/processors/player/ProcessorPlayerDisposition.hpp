#ifndef OPENMW_PROCESSORPLAYERDISPOSITION_HPP
#define OPENMW_PROCESSORPLAYERDISPOSITION_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerDisposition final: public PlayerProcessor
    {
    public:
        ProcessorPlayerDisposition()
        {
            BPP_INIT(ID_PLAYER_DISPOSITION)
        }

        void Do(PlayerPacket &packet, const std::shared_ptr<Player> &player) override
        {
            DEBUG_PRINTF(strPacketID.c_str());

            packet.Send(true);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_DISPOSITION>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERDISPOSITION_HPP
