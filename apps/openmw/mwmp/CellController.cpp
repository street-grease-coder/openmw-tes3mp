#include <components/esm/cellid.hpp>
#include <components/openmw-mp/Log.hpp>
#include <components/openmw-mp/Utils.hpp>

#include "../mwbase/environment.hpp"

#include "../mwworld/containerstore.hpp"
#include "../mwworld/class.hpp"
#include "../mwworld/worldimp.hpp"

#include "CellController.hpp"
#include "Main.hpp"
#include "LocalActor.hpp"
#include "LocalPlayer.hpp"
using namespace mwmp;

std::map<std::string, mwmp::Cell *> CellController::cellsActive;
std::map<std::string, std::string> CellController::localActorsToCells;
std::map<std::string, std::string> CellController::dedicatedActorsToCells;

mwmp::CellController::CellController()
{

}

mwmp::CellController::~CellController()
{

}

void CellController::updateLocal(bool forceUpdate)
{
    for (std::map<std::string, mwmp::Cell *>::iterator it = cellsActive.begin(); it != cellsActive.end();)
    {
        mwmp::Cell *mpCell = it->second;

        if (!MWBase::Environment::get().getWorld()->isCellActive(mpCell->getCellStore()))
        {
            mpCell->uninitializeLocalActors();
            mpCell->uninitializeDedicatedActors();
            cellsActive.erase(it++);
        }
        else
        {
            //LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "Updating mwmp::Cell %s", mpCell->getDescription().c_str());
            mpCell->updateLocal(forceUpdate);
            ++it;
        }
    }
}

void CellController::updateDedicated(float dt)
{
    for (std::map<std::string, mwmp::Cell *>::iterator it = cellsActive.begin(); it != cellsActive.end(); ++it)
    {
        it->second->updateDedicated(dt);
    }
}

void CellController::initializeCell(const ESM::Cell& cell)
{
    std::string mapIndex = cell.getDescription();

    // If this key doesn't exist, create it
    if (cellsActive.count(mapIndex) == 0)
    {
        MWWorld::CellStore *cellStore = getCellStore(cell);

        if (!cellStore) return;

        mwmp::Cell *mpCell = new mwmp::Cell(cellStore);
        cellsActive[mapIndex] = mpCell;

        LOG_MESSAGE_SIMPLE(Log::LOG_INFO, "- Initialized mwmp::Cell %s", mpCell->getDescription().c_str());
    }
}

void CellController::initializeLocalActors(const ESM::Cell& cell)
{
    std::string mapIndex = cell.getDescription();    

    initializeCell(cell);

    // If this now exists, initialize local actors in it
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->uninitializeDedicatedActors();
        cellsActive[mapIndex]->initializeLocalActors();
    }
}

void CellController::readPositions(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readPositions(actorList);
    }
}

void CellController::readAnimFlags(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readAnimFlags(actorList);
    }
}

void CellController::readAnimPlay(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readAnimPlay(actorList);
    }
}

void CellController::readStatsDynamic(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readStatsDynamic(actorList);
    }
}

void CellController::readSpeech(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readSpeech(actorList);
    }
}

void CellController::readAttack(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readAttack(actorList);
    }
}

void CellController::readCellChange(ActorList& actorList)
{
    std::string mapIndex = actorList.cell.getDescription();

    initializeCell(actorList.cell);

    // If this now exists, send it the data
    if (cellsActive.count(mapIndex) > 0)
    {
        cellsActive[mapIndex]->readCellChange(actorList);
    }
}

void CellController::setLocalActorRecord(std::string actorIndex, std::string cellIndex)
{
    localActorsToCells[actorIndex] = cellIndex;
}

void CellController::removeLocalActorRecord(std::string actorIndex)
{
    localActorsToCells.erase(actorIndex);
}

bool CellController::isLocalActor(MWWorld::Ptr ptr)
{
    if (ptr.mRef == NULL)
        return false;

    std::string mapIndex = generateMapIndex(ptr);

    return (localActorsToCells.count(mapIndex) > 0);
}

bool CellController::isLocalActor(std::string refId, int refNumIndex, int mpNum)
{
    std::string mapIndex = generateMapIndex(refId, refNumIndex, mpNum);
    return (localActorsToCells.count(mapIndex) > 0);
}

LocalActor *CellController::getLocalActor(MWWorld::Ptr ptr)
{
    std::string actorIndex = generateMapIndex(ptr);
    std::string cellIndex = localActorsToCells.at(actorIndex);

    return cellsActive.at(cellIndex)->getLocalActor(actorIndex);
}

LocalActor *CellController::getLocalActor(std::string refId, int refNumIndex, int mpNum)
{
    std::string actorIndex = generateMapIndex(refId, refNumIndex, mpNum);
    std::string cellIndex = localActorsToCells.at(actorIndex);

    return cellsActive.at(cellIndex)->getLocalActor(actorIndex);
}

void CellController::setDedicatedActorRecord(std::string actorIndex, std::string cellIndex)
{
    dedicatedActorsToCells[actorIndex] = cellIndex;
}

void CellController::removeDedicatedActorRecord(std::string actorIndex)
{
    dedicatedActorsToCells.erase(actorIndex);
}

bool CellController::isDedicatedActor(std::string refId, int refNumIndex, int mpNum)
{
    std::string mapIndex = generateMapIndex(refId, refNumIndex, mpNum);
    return (dedicatedActorsToCells.count(mapIndex) > 0);
}

bool CellController::isDedicatedActor(MWWorld::Ptr ptr)
{
    if (ptr.mRef == NULL)
        return false;

    std::string mapIndex = generateMapIndex(ptr);

    return (dedicatedActorsToCells.count(mapIndex) > 0);
}

DedicatedActor *CellController::getDedicatedActor(MWWorld::Ptr ptr)
{
    std::string actorIndex = generateMapIndex(ptr);
    std::string cellIndex = dedicatedActorsToCells.at(actorIndex);

    return cellsActive.at(cellIndex)->getDedicatedActor(actorIndex);
}

DedicatedActor *CellController::getDedicatedActor(std::string refId, int refNumIndex, int mpNum)
{
    std::string actorIndex = generateMapIndex(refId, refNumIndex, mpNum);
    std::string cellIndex = dedicatedActorsToCells.at(actorIndex);

    return cellsActive.at(cellIndex)->getDedicatedActor(actorIndex);
}

std::string CellController::generateMapIndex(std::string refId, int refNumIndex, int mpNum)
{
    std::string mapIndex = "";
    mapIndex += refId;
    mapIndex += "-" + Utils::toString(refNumIndex);
    mapIndex += "-" + Utils::toString(mpNum);
    return mapIndex;
}

std::string CellController::generateMapIndex(MWWorld::Ptr ptr)
{
    return generateMapIndex(ptr.getCellRef().getRefId(),
        ptr.getCellRef().getRefNum().mIndex, ptr.getCellRef().getMpNum());
}

std::string CellController::generateMapIndex(BaseActor baseActor)
{
    return generateMapIndex(baseActor.refId, baseActor.refNumIndex, baseActor.mpNum);
}

bool CellController::isActiveCell(const ESM::Cell& cell)
{
    return (cellsActive.count(cell.getDescription()) > 0);
}

Cell *CellController::getCell(const ESM::Cell& cell)
{
    return cellsActive.at(cell.getDescription());
}

MWWorld::CellStore *CellController::getCellStore(const ESM::Cell& cell)
{
    MWWorld::CellStore *cellStore;

    if (cell.isExterior())
        cellStore = MWBase::Environment::get().getWorld()->getExterior(cell.mData.mX, cell.mData.mY);
    else
    {
        try
        {
            cellStore = MWBase::Environment::get().getWorld()->getInterior(cell.mName);
        }
        catch (std::exception&)
        {
            cellStore = NULL;
        }
    }

    return cellStore;
}

bool CellController::isSameCell(const ESM::Cell& cell, const ESM::Cell& otherCell)
{
    if (cell.isExterior() && otherCell.isExterior())
    {
        if (cell.mData.mX == otherCell.mData.mX && cell.mData.mY == otherCell.mData.mY)
            return true;
    }
    else if (Misc::StringUtils::ciEqual(cell.mName, otherCell.mName))
        return true;

    return false;
}

void CellController::openContainer(const MWWorld::Ptr &container, bool loot)
{
    // Record this as the player's current open container
    Main::get().getLocalPlayer()->storeCurrentContainer(container, loot);

    LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "Container \"%s\" (%d) is opened. Loot: %s",
                       container.getCellRef().getRefId().c_str(), container.getCellRef().getRefNum().mIndex,
                       loot ? "true" : "false");

    MWWorld::ContainerStore &cont = container.getClass().getContainerStore(container);
    for (MWWorld::ContainerStoreIterator iter = cont.begin(); iter != cont.end(); iter++)
    {
        int count = iter->getRefData().getCount();
        const std::string &name = iter->getCellRef().getRefId();

        LOG_APPEND(Log::LOG_VERBOSE, " - Item. Refid: \"%s\" Count: %d", name.c_str(), count);

        /*if(::Misc::StringUtils::ciEqual(name, "gold_001"))
            cont.remove("gold_001", count, container);*/
    }

}

void CellController::closeContainer(const MWWorld::Ptr &container)
{
    Main::get().getLocalPlayer()->clearCurrentContainer();

    // If the player died while in a container, the container's Ptr could be invalid now
    if (!container.isEmpty())
    {
        LOG_MESSAGE_SIMPLE(Log::LOG_VERBOSE, "Container \"%s\" (%d) is closed.", container.getCellRef().getRefId().c_str(),
                           container.getCellRef().getRefNum().mIndex);

        MWWorld::ContainerStore &cont = container.getClass().getContainerStore(container);
        for (MWWorld::ContainerStoreIterator iter = cont.begin(); iter != cont.end(); iter++)
        {
            LOG_APPEND(Log::LOG_VERBOSE, " - Item. Refid: \"%s\" Count: %d", iter->getCellRef().getRefId().c_str(),
                       iter->getRefData().getCount());
        }
    }

    Main::get().getLocalPlayer()->updateInventory();
}

int CellController::getCellSize() const
{
    return 8192;
}
