#pragma once
#include "GMLIB/GMLIB.h"
#include <ll/api/event/entity/MobEvent.h>

namespace GMLIB::Event::EntityEvent {

class MobPickupItemBeforeEvent : public ll::event::Cancellable<ll::event::entity::MobEvent> {
    ItemActor& mItemActor;

public:
    constexpr explicit MobPickupItemBeforeEvent(Mob& mob, ItemActor& itemActor)
    : Cancellable(mob),
      mItemActor(itemActor) {}

    GMLIB_API ItemActor const& getItemActor() const;
};

class MobPickupItemAfterEvent : public ll::event::entity::MobEvent {
    ItemActor& mItemActor;

public:
    constexpr explicit MobPickupItemAfterEvent(Mob& mob, ItemActor& itemActor) : MobEvent(mob), mItemActor(itemActor) {}

    GMLIB_API ItemActor const& getItemActor() const;
};

} // namespace GMLIB::Event::EntityEvent