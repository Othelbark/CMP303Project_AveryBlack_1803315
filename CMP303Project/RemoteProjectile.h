#pragma once
#include "Projectile.h"
#include "RemoteObject.h"

class RemoteProjectile :
    public Projectile, public RemoteObject
{
public:
    RemoteProjectile();
    ~RemoteProjectile();

    void update(float dt) override;

};

