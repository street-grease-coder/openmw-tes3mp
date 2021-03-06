//
// Created by koncord on 16.04.17.
//

#ifndef OPENMW_PROCESSORPLAYERRESURRECT_HPP
#define OPENMW_PROCESSORPLAYERRESURRECT_HPP

#include "../PlayerProcessor.hpp"
#include "apps/openmw/mwmp/Main.hpp"
#include "apps/openmw/mwmp/Networking.hpp"

namespace mwmp
{
    class ProcessorPlayerResurrect final: public PlayerProcessor
    {
    public:
        ProcessorPlayerResurrect()
        {
            BPP_INIT(ID_PLAYER_RESURRECT)
        }

        virtual void Do(PlayerPacket &packet, BasePlayer *player)
        {
            LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "Received ID_PLAYER_RESURRECT from server");
            
            if (isLocal())
            {
                LOG_APPEND(Log::LOG_INFO, "- Packet was about me with resurrectType of %i", (int) player->resurrectType);

                player->creatureStats.mDead = false;

                MWBase::World *world = MWBase::Environment::get().getWorld();
                MWWorld::Ptr playerPtr = world->getPlayerPtr();


                switch(player->resurrectType)
                {
                    case ResurrectType::Regular:
                        break;
                    case ResurrectType::ImperialShrine:
                        world->teleportToClosestMarker(playerPtr, "divinemarker");
                        break;
                    case ResurrectType::TribunalTemple:
                        world->teleportToClosestMarker(playerPtr, "templemarker");
                        break;
                }


                playerPtr.getClass().getCreatureStats(playerPtr).resurrect();

                // The player could have died from a hand-to-hand attack, so reset their fatigue
                // as well
                if (player->creatureStats.mDynamic[2].mMod < 1)
                    player->creatureStats.mDynamic[2].mMod = 1;

                player->creatureStats.mDynamic[2].mCurrent = player->creatureStats.mDynamic[2].mMod;
                MWMechanics::DynamicStat<float> fatigue;
                fatigue.readState(player->creatureStats.mDynamic[2]);
                playerPtr.getClass().getCreatureStats(playerPtr).setFatigue(fatigue);

                // If this player had a weapon or spell readied when dying, they will still have it
                // readied but be unable to use it unless we clear it here
                playerPtr.getClass().getNpcStats(playerPtr).setDrawState(MWMechanics::DrawState_Nothing);

                // Record that the player has died since the last attempt was made to arrest them,
                // used to make guards lenient enough to attempt an arrest again
                player->diedSinceArrestAttempt = true;

                LOG_APPEND(Log::LOG_INFO, "- diedSinceArrestAttempt is now true");

                packet.setPlayer(player);
                packet.Send(serverAddr);

                static_cast<LocalPlayer*>(player)->updateStatsDynamic(true);
                Main::get().getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->setPlayer(player);
                Main::get().getNetworking()->getPlayerPacket(ID_PLAYER_STATS_DYNAMIC)->Send(serverAddr);
            }
            else if (player != 0)
            {
                LOG_APPEND(Log::LOG_INFO, "- Packet was about %s", player->npc.mName.c_str());

                player->creatureStats.mDead = false;
                if (player->creatureStats.mDynamic[0].mMod < 1)
                    player->creatureStats.mDynamic[0].mMod = 1;
                player->creatureStats.mDynamic[0].mCurrent = player->creatureStats.mDynamic[0].mMod;

                MWWorld::Ptr ptr = static_cast<DedicatedPlayer*>(player)->getPtr();

                ptr.getClass().getCreatureStats(ptr).resurrect();

                MWMechanics::DynamicStat<float> health;
                health.readState(player->creatureStats.mDynamic[0]);
                ptr.getClass().getCreatureStats(ptr).setHealth(health);
            }
        }
    };
}

#endif //OPENMW_PROCESSORPLAYERRESURRECT_HPP
