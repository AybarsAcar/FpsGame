// Copyright Aybars Acar

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

public:
	/// called when the actor is highlighted
	virtual void HighlightActor() = 0;

	/// Called when the actor is un-highlighted
	virtual void UnHighlightActor() = 0;
};
