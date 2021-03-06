#ifndef OPENMW_PROCESSORPLAYERLEVEL_HPP
#define OPENMW_PROCESSORPLAYERLEVEL_HPP

#include "../PlayerProcessor.hpp"

namespace mwmp
{
    class ProcessorPlayerLevel final: public PlayerProcessor
    {
    public:
        ProcessorPlayerLevel()
        {
            BPP_INIT(ID_PLAYER_LEVEL)
        }

        void Do(PlayerPacket &packet, const std::shared_ptr<Player> &player) override
        {
            if (!player->creatureStats.mDead)
                Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_PLAYER_LEVEL>(player);
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERLEVEL_HPP
