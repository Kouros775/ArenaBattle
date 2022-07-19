// Fill out your copyright notice in the Description page of Project Settings.


#include "ABItemBox.h"
#include "ABWeapon.h"
#include "ABPlayerCharacter.h"
//#include "ABCharacter.h"


// Sets default values
AABItemBox::AABItemBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Box = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BOX"));
	Effect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("EFFECT"));
	
	RootComponent = Trigger;
	Box->SetupAttachment(Trigger);
	Effect->SetupAttachment(Trigger);

	
	Trigger->SetBoxExtent(FVector(40.0f, 42.0f, 30.0f));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SK_BOX(TEXT("StaticMesh'/Game/InfinityBladeGrassLands/Environments/Breakables/StaticMesh/Box/SM_Env_Breakables_Box1.SM_Env_Breakables_Box1'")); 
	if(SK_BOX.Succeeded())
	{
		Box->SetStaticMesh(SK_BOX.Object);
	}
	Box->SetRelativeLocation(FVector(0.0f, -3.5f, -30.0f));


	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_CHESTOPEN(TEXT("ParticleSystem'/Game/InfinityBladeGrassLands/Effects/FX_Treasure/Chest/P_TreasureChest_Open_Mesh.P_TreasureChest_Open_Mesh'"));
	if(P_CHESTOPEN.Succeeded())
	{
		Effect->SetTemplate(P_CHESTOPEN.Object);
		Effect->bAutoActivate = false;
	}


	
	Trigger->SetCollisionProfileName(TEXT("ItemBox"));
	Box->SetCollisionProfileName(TEXT("NoCollision"));

	// 무기에 대한 정보를 담는 클래스를 지정.
	WeaponItemClass = AABWeapon::StaticClass();
}

// Called when the game starts or when spawned
void AABItemBox::BeginPlay()
{
	Super::BeginPlay();
	
}


void AABItemBox::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABItemBox::OnCharacterOverLap);
}


void AABItemBox::OnCharacterOverLap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABLOG_S(Warning);

	const auto PlayerCharacter = Cast<AABPlayerCharacter>(OtherActor);
	ABCHECK(PlayerCharacter);

	if(PlayerCharacter && WeaponItemClass)
	{
		if(PlayerCharacter->CanSetWeapon() == true)
		{
			const auto NewWeapon = GetWorld()->SpawnActor<AABWeapon>(WeaponItemClass, FVector::ZeroVector, FRotator::ZeroRotator);
			PlayerCharacter->SetWeapon(NewWeapon);

			Effect->Activate(true);
			Box->SetHiddenInGame(true, true);
			SetActorEnableCollision(false);
			Effect->OnSystemFinished.AddDynamic(this, &AABItemBox::OnEffectFinished);
		}
	}
	else
	{
		ABLOG(Warning, TEXT("%s can't equip weapon currently"), *PlayerCharacter->GetName());
	}
}


void AABItemBox::OnEffectFinished(UParticleSystemComponent* PSystem)
{
	Destroy();
}


// Called every frame
void AABItemBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

