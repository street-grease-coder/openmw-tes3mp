#include "PacketPlayerStatsDynamic.hpp"
#include <components/openmw-mp/NetworkMessages.hpp>

using namespace mwmp;

PacketPlayerStatsDynamic::PacketPlayerStatsDynamic(RakNet::RakPeerInterface *peer) : PlayerPacket(peer)
{
    packetID = ID_PLAYER_STATS_DYNAMIC;
}

void PacketPlayerStatsDynamic::Packet(RakNet::BitStream *bs, bool send)
{
    PlayerPacket::Packet(bs, send);

    uint32_t count;
    if (send)
        count = static_cast<uint32_t>(player->statsDynamicIndexChanges.size());

    RW(count, send);

    if (!send)
    {
        player->statsDynamicIndexChanges.clear();
        player->statsDynamicIndexChanges.resize(count);
    }

    for (auto &&statsDynamicIndex : player->statsDynamicIndexChanges)
    {
        RW(statsDynamicIndex, send);

        RW(player->creatureStats.mDynamic[statsDynamicIndex], send);
    }
}
