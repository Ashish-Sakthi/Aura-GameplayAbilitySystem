// Copyright Ashish


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(Mesh);
}

void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO: Change this to apply GamePlay Effects.
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		const UAuraAttributeSet* AuraAttribute = Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		UAuraAttributeSet* MutableAuraAttribute = const_cast<UAuraAttributeSet*>(AuraAttribute);
		MutableAuraAttribute->SetHealth(MutableAuraAttribute->GetHealth() + 50.f);
		MutableAuraAttribute->SetMana(MutableAuraAttribute->GetMana() + 50.f);
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AAuraEffectActor::OnEndOverlap);
}

