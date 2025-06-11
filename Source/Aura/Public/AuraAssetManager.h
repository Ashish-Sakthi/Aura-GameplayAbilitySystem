// Copyright Ashish

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * UAuraAssetManager is a custom implementation of the UAssetManager class.
 * It is responsible for initializing and managing game-specific assets.
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
};
