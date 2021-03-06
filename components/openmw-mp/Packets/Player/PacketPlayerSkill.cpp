//
// Created by koncord on 17.03.16.
//

#include "PacketPlayerSkill.hpp"

#include <components/openmw-mp/NetworkMessages.hpp>
#include <components/esm/creaturestats.hpp>

using namespace mwmp;

PacketPlayerSkill::PacketPlayerSkill(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_SKILL;
}

void PacketPlayerSkill::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);
    
    uint32_t count;

    if (send)
        count = static_cast<uint32_t>(player->skillIndexChanges.size());

    RW(count, send);

    if (!send)
    {
        player->skillIndexChanges.clear();
        player->skillIndexChanges.resize(count);
    }

    for (auto && skillId : player->skillIndexChanges)
    {
        RW(skillId, send);
        RW(player->npcStats.mSkills[skillId], send);
    }
}
