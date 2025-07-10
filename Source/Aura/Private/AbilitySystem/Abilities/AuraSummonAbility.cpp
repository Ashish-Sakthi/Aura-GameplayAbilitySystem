// Copyright Ashish


#include "AbilitySystem/Abilities/AuraSummonAbility.h"

#include "Kismet/KismetSystemLibrary.h"

TArray<FVector> UAuraSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const float DeltaSpread = SpawnSpread / NumMinions;

	
	// Calculate the starting vector rotated half the spread angle to the left
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread/2, FVector::UpVector);
	TArray<FVector> SpawnLocations;
	for (int32 i = 0; i < NumMinions; i++)
	{
		// Rotate the spawn direction for each minion based on delta spread
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector);
		// Get random spawn location between min and max distance along the direction
		const FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance);
		SpawnLocations.Add(ChosenSpawnLocation);
		
		DrawDebugSphere(GetWorld(), ChosenSpawnLocation, 18.f, 12, FColor::Cyan, false, 3.f );
		UKismetSystemLibrary::DrawDebugArrow(GetAvatarActorFromActorInfo(), Location, Location + Direction * MaxSpawnDistance, 4.f, FLinearColor::Green, 3.f );
		DrawDebugSphere(GetWorld(), Location + Direction * MinSpawnDistance, 12.f, 12, FColor::Red, false, 3.f );
		DrawDebugSphere(GetWorld(), Location + Direction * MaxSpawnDistance, 12.f, 12, FColor::Red, false, 3.f );
	}
	return SpawnLocations;
}