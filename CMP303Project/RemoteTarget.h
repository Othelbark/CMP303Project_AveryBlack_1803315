#pragma once
#include "TargetObject.h"
#include "RemoteObject.h"

class RemoteTarget :
    public TargetObject, public RemoteObject
{
};

