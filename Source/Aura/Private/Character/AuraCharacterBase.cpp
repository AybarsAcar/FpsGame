// Copyright Aybars Acar


#include "Character/AuraCharacterBase.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	// init the weapon
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");

	// attach the weapon to a socket on the mesh
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	// weapon should have no collision
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}
