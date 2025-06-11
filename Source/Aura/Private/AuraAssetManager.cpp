// Copyright Ashish


#include "AuraAssetManager.h"
#include "AuraGameplayTags.h"

/*
 * Asset Manager needs to be set in the engine config to work.
 * In [Engine.Engine]
 * AssetManagerClassName=/Script/ProjName.AssetManagerName
*/

//Getter function to get the Asset Manager instance.
UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

//Virtual function to initialize Gameplay Tags.
void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	//Initialize Gameplay Tags before loading any assets.
	FAuraGameplayTags::InitializeNativeGameplayTags();
}
