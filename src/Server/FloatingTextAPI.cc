#include "Global.h"
#include "mc/world/item/NetworkItemStackDescriptor.h"
#include <GMLIB/Server/BinaryStreamAPI.h>
#include <GMLIB/Server/FloatingTextAPI.h>
#include <GMLIB/Server/LevelAPI.h>
#include <GMLIB/Server/NetworkPacketAPI.h>
#include <mc/network/packet/AddItemActorPacket.h>
#include <mc/network/packet/RemoveActorPacket.h>
#include <mc/network/packet/SetActorDataPacket.h>
#include <mc/util/Random.h>

namespace GMLIB::Server {

std::unordered_map<int64, FloatingText*> mRuntimeFloatingTextList;

int getNextFloatingTextId() {
    auto id = Random::getThreadLocal().nextInt(0, 2147473647);
    while (ll::service::getLevel()->fetchEntity(ActorUniqueID(id))) {
        id = Random::getThreadLocal().nextInt(0, 2147473647);
    }
    return id;
}

FloatingText::FloatingText(std::string text, Vec3 position, DimensionType dimensionId)
: mText(text),
  mPosition(position),
  mDimensionId(dimensionId) {
    mRuntimeId = getNextFloatingTextId();
    mRuntimeFloatingTextList.insert({mRuntimeId, this});
}

FloatingText::~FloatingText() {
    removeFromAllClients();
    mRuntimeFloatingTextList.erase(mRuntimeId);
}

void FloatingText::setPosition(Vec3& pos, DimensionType dimid) {
    mPosition    = pos;
    mDimensionId = dimid;
}

GMLIB::Server::NetworkPacket<15> createAddFloatingTextPacket(FloatingText* ft) {
    auto               item = std::make_unique<ItemStack>(ItemStack{"minecraft:air"});
    auto               nisd = NetworkItemStackDescriptor(*item);
    GMLIB_BinaryStream bs;
    bs.writeVarInt64(ft->getRuntimeID());
    bs.writeUnsignedVarInt64(ft->getRuntimeID());
    bs.writeType(nisd);
    bs.writeVec3(ft->getPos());
    bs.writeVec3(Vec3{0, 0, 0});
    // DataItem
    bs.writeUnsignedVarInt(2);
    bs.writeUnsignedVarInt((uint)0x4);
    bs.writeUnsignedVarInt((uint)0x4);
    bs.writeString(ft->getText());
    bs.writeUnsignedVarInt((uint)0x51);
    bs.writeUnsignedVarInt((uint)0x0);
    bs.writeBool(true);
    bs.writeBool(false);
    GMLIB::Server::NetworkPacket<(int)MinecraftPacketIds::AddItemActor> pkt(bs.getAndReleaseData());
    return pkt;
}

void FloatingText::sendToClient(Player* pl) {
    if (!pl->isSimulatedPlayer() && pl->getDimensionId() == mDimensionId) {
        auto pkt = createAddFloatingTextPacket(this);
        pkt.sendTo(*pl);
    }
}

void FloatingText::sendToAllClients() {
    auto pkt = createAddFloatingTextPacket(this);
    ll::service::getLevel()->forEachPlayer([&](Player& pl) -> bool {
        if (!pl.isSimulatedPlayer() && pl.getDimensionId() == mDimensionId) {
            pkt.sendTo(pl);
        }
        return true;
    });
}

void FloatingText::removeFromAllClients() { RemoveActorPacket(ActorUniqueID(this->mRuntimeId)).sendToClients(); }

void FloatingText::removeFromClient(Player* pl) {
    if (!pl->isSimulatedPlayer()) {
        RemoveActorPacket(ActorUniqueID(this->mRuntimeId)).sendTo(*pl);
    }
}

void FloatingText::setText(std::string newText) { mText = newText; }

FloatingText* FloatingText::getFloatingText(int64 runtimeId) {
    if (mRuntimeFloatingTextList.count(runtimeId)) {
        return mRuntimeFloatingTextList[runtimeId];
    }
    return nullptr;
}

bool FloatingText::deleteFloatingText(int64 runtimeId) {
    auto ft = getFloatingText(runtimeId);
    if (ft) {
        delete ft;
        return true;
    }
    return false;
}

int64 FloatingText::getRuntimeID() { return mRuntimeId; }

std::string FloatingText::getText() { return mText; }

Vec3 FloatingText::getPos() { return mPosition; }

DimensionType FloatingText::getDimensionId() { return mDimensionId; }

} // namespace GMLIB::Server