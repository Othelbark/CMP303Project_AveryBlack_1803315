#pragma once
#include "MinionObject.h"
#include "RemoteObject.h"

class RemoteMinion :
    public MinionObject, public RemoteObject
{
public:
    RemoteMinion();
    ~RemoteMinion();

    void update(float dt) override;
};

