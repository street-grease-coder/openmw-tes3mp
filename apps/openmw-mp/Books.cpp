//
// Created by koncord on 15.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>

#include "Script/LuaState.hpp"
#include "Networking.hpp"

#include "Books.hpp"
#include "Player.hpp"

void Books::Init(LuaState &lua)
{
    lua.getState()->new_usertype<Books>("Books",
                                        "addBook", &Books::addBook,
                                        "getBookId", &Books::getBookId,
                                        "getChanges", &Books::getChanges,
                                        "reset", &Books::reset
    );
}

Books::Books(Player *player) : player(player), changed(false)
{

}

Books::~Books()
{

}

void Books::addBook(const std::string &bookId)
{
    if (!changed)
        reset();
    player->bookChanges.books.push_back({bookId});
    changed = true;
}

std::string Books::getBookId(unsigned i) const
{
    if (i >= player->bookChanges.books.size())
        return "invalid";

    return player->bookChanges.books.at(i).bookId;
}

unsigned Books::getChanges() const
{
    return player->bookChanges.books.size();
}

void Books::reset()
{
    player->bookChanges.books.clear();
}

void Books::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_BOOK);

    packet->setPlayer(player);
    packet->Send(/*toOthers*/ false);
}
