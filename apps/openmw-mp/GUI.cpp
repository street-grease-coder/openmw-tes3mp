//
// Created by koncord on 15.08.17.
//

#include <components/openmw-mp/NetworkMessages.hpp>
#include "GUI.hpp"
#include "Player.hpp"
#include "Networking.hpp"

void GUI::Init(LuaState &lua)
{
    lua.getState()->new_usertype<GUI>("GUI",
                                      "messageBox", &GUI::messageBox,
                                      "customMessageBox", &GUI::customMessageBox,
                                      "inputDialog", &GUI::inputDialog,
                                      "passwordDialog", &GUI::passwordDialog,
                                      "listBox", &GUI::listBox,
                                      "setMapVisibility", &GUI::setMapVisibility,
                                      "setMapVisibilityAll", &GUI::setMapVisibilityAll,
                                      "createWindow", &GUI::createWindow,
                                      "deleteWindow", &GUI::deleteWindow
    );
    Window::Init(lua);
}

GUI::GUI(Player *player): player(player), changed(false)
{

}

GUI::~GUI()
{

}

void GUI::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_GUI_MESSAGEBOX);
    packet->setPlayer(player);
    packet->Send(false);
}

void GUI::messageBox(int id, const char *label)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.type = Player::GUIMessageBox::MessageBox;

    changed = true;
}

void GUI::customMessageBox(int id, const char *label, const char *buttons)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.buttons = buttons;
    player->guiMessageBox.type = Player::GUIMessageBox::CustomMessageBox;

    changed = true;
}

void GUI::inputDialog(int id, const char *label)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.type = Player::GUIMessageBox::InputDialog;

    changed = true;
}

void GUI::passwordDialog(int id, const char *label, const char *note)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.note = note;
    player->guiMessageBox.type = Player::GUIMessageBox::PasswordDialog;

    changed = true;
}

void GUI::listBox(int id, const char *label, const char *items)
{
    player->guiMessageBox.id = id;
    player->guiMessageBox.label = label;
    player->guiMessageBox.data = items;
    player->guiMessageBox.type = Player::GUIMessageBox::ListBox;

    changed = true;
}

void GUI::setMapVisibility(unsigned short targetPID, unsigned short affectedPID, unsigned short state)
{
    LOG_MESSAGE(Log::LOG_WARN, "stub");
}

void GUI::setMapVisibilityAll(unsigned short targetPID, unsigned short state)
{
    LOG_MESSAGE(Log::LOG_WARN, "stub");
}

std::shared_ptr<Window> GUI::createWindow(short x, short y, sol::function fn, sol::this_environment te)
{
    int id = 0;

    for (auto &window : windows)
    {
        if (window.second == nullptr)
        {
            id = window.first;
            break;
        }
    }

    if (id == 0)
        id = lastWindowId++;

    auto window = std::make_shared<Window>(player, id);
    window->setSize(x, y);
    window->setCallback(fn);

    windows[id] = window;
    return window;
}

void GUI::deleteWindow(std::shared_ptr<Window> window)
{
    auto it = windows.find(window->getID());
    if (it != windows.end())
    {
        it->second = nullptr;
    }
}

void GUI::onGUIWindowAction()
{
    auto it = windows.find(player->guiWindow.id);
    if (it != windows.end() && it->second != nullptr)
    {
        it->second->call(player->guiWindow);
    }
}

void QuickKeys::Init(LuaState &lua)
{
    lua.getState()->new_usertype<QuickKeys>("QuickKeys",
        "addQuickKey", &QuickKeys::addQuickKey,
        "getQuickKey", &QuickKeys::getQuickKey,
        "setQuickKey", &QuickKeys::setQuickKey,
        "clear", &QuickKeys::clear,
        "size", &QuickKeys::size
        );
}

QuickKeys::QuickKeys(Player *player) : player(player), changed(false)
{

}

QuickKeys::~QuickKeys()
{

}

void QuickKeys::update()
{
    if (!changed)
        return;
    changed = false;

    auto packet = mwmp::Networking::get().getPlayerPacketController()->GetPacket(ID_PLAYER_QUICKKEYS);
    packet->setPlayer(player);
    packet->Send(false);
    clear();
}

void QuickKeys::addQuickKey(QuickKey quickKey)
{
    player->quickKeyChanges.quickKeys.push_back(quickKey.quickKey);
    changed = true;
}


QuickKey QuickKeys::getQuickKey(int id) const
{
    return QuickKey(player->quickKeyChanges.quickKeys.at(id));
}

void QuickKeys::setQuickKey(int id, QuickKey quickKey)
{
    player->quickKeyChanges.quickKeys.at(id) = quickKey.quickKey;
    changed = true;
}

void QuickKeys::clear()
{
    player->quickKeyChanges.quickKeys.clear();
    changed = true;
}

size_t QuickKeys::size() const
{
    return player->quickKeyChanges.quickKeys.size();
}

void QuickKey::Init(LuaState &lua)
{
    lua.getState()->new_usertype<QuickKey>("QuickKey",
                                          "slot", sol::property(&QuickKey::getSlot, &QuickKey::setSlot),
                                          "type", sol::property(&QuickKey::getType, &QuickKey::setType),
                                          "itemId", sol::property(&QuickKey::getItemId, &QuickKey::setItemId)
    );
}

QuickKey::QuickKey(mwmp::QuickKey &quickKey): quickKey(quickKey)
{

}

int QuickKey::getSlot() const
{
    return quickKey.slot;
}

void QuickKey::setSlot(unsigned short slot)
{
    quickKey.slot = slot;
}

int QuickKey::getType() const
{
    return quickKey.type;
}

void QuickKey::setType(int type)
{
    quickKey.type = type;
}

std::string QuickKey::getItemId() const
{
    return quickKey.itemId;
}

void QuickKey::setItemId(const std::string &itemId)
{
    quickKey.itemId = itemId;
}
