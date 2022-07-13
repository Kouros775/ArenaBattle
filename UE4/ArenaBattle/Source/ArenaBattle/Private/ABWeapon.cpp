// Fill out your copyright notice in the Description page of Project Settings.


#include "ABWeapon.h"


// Sets default values
AABWeapon::AABWeapon()
	: AttackRange(150.0f)
	, AttackDamageMin(-2.5f)
	, AttackDamageMax(10.0f)
	, AttackModifierMin(0.85f)
	, AttackModifierMax(1.25f)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WEAPON"));
	RootComponent = Weapon;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_WEAPON(TEXT("SkeletalMesh'/Game/InfinityBladeWeapons/Weapons/Blade/Silly_Weapons/Blade_ChickenBlade/SK_Blade_ChickenBlade.SK_Blade_ChickenBlade'"));
	if(SK_WEAPON.Succeeded())
	{
		Weapon->SetSkeletalMesh(SK_WEAPON.Object);
	}

	// 실제 충돌은 캐릭터에서 하고, 무기는 악세사리이므로 충돌제거.
	Weapon->SetCollisionProfileName(TEXT("NoCollision"));
}


float AABWeapon::GetAttackRange() const
{
	return AttackRange;
}

float AABWeapon::GetAttackDamage() const
{
	return AttackDamage;
}

float AABWeapon::GetAttackModifier() const
{
	return AttackModifier;
}

// Called when the game starts or when spawned
void AABWeapon::BeginPlay()
{
	Super::BeginPlay();

	AttackDamage = FMath::RandRange(AttackDamageMin, AttackDamageMax);
	AttackModifier = FMath::RandRange(AttackModifierMin, AttackModifierMax);

	ABLOG(Warning, TEXT("Weapon Dmg : %f, modifier : %f"), AttackDamage, AttackModifier);
}

// Called every frame
void AABWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

