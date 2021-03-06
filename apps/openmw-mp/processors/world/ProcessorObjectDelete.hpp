#ifndef OPENMW_PROCESSOROBJECTDELETE_HPP
#define OPENMW_PROCESSOROBJECTDELETE_HPP

#include "../WorldProcessor.hpp"

namespace mwmp
{
    class ProcessorObjectDelete final: public WorldProcessor
    {
    public:
        ProcessorObjectDelete()
        {
            BPP_INIT(ID_OBJECT_DELETE)
        }

        void Do(WorldPacket &packet, const std::shared_ptr<Player> &player, BaseEvent &event) override
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received %s from %s", strPacketID.c_str(), player->npc.mName.c_str());
            
            packet.Send(true);

            auto objCtrl = Networking::get().getState().getObjectCtrl();
            auto objects = objCtrl.copyObjects(event);

            Networking::get().getState().getEventCtrl().Call<CoreEvent::ON_OBJECT_DELETE>(player, objects);

            objCtrl.sendObjects(player, objects, event.cell);
        }
    };
}

#endif //OPENMW_PROCESSOROBJECTDELETE_HPP
