// Fill out your copyright notice in the Description page of Project Settings.


#include "ABSection.h"

// Sets default values
AABSection::AABSection()
	: bNoBattle(false)
	, CurrentState(ESectionState::Ready)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(TEXT("StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'"));
	if(SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}
	else
	{
		ABLOG(Error, TEXT("Failed to load : ABSection static mesh"));
	}

	// >> 철문
	const FString GateAssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);
	if(SM_GATE.Succeeded() == false)
	{
		ABLOG(Error, TEXT("Section Gate Load Failed"));
	}

	static FName GateSockets[] = {
		{TEXT("+XGate")}
		, {TEXT("-XGate")}
		, {TEXT("+YGate")}
		, {TEXT("-YGate")}
	};
	for(FName GateSocket : GateSockets)
	{
		ABCHECK(Mesh->DoesSocketExist(GateSocket));
		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, 80.5f, 0.0f));
		GateMeshes.Add(NewGate);

		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		NewGateTrigger->ComponentTags.Add(GateSocket);
	}
	// << 철문

	// >> 철문 Trigger
	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);
	// << 철문 Trigger
}

// Called when the game starts or when spawned
void AABSection::BeginPlay()
{
	Super::BeginPlay();

	ESectionState sectionState = bNoBattle ? ESectionState::Complete : ESectionState::Ready;
	SetState(sectionState);
}


void AABSection::SetState(const ESectionState& NewState)
{
	switch (NewState)
	{
		case ESectionState::Ready:
		{
			Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
			for(UBoxComponent* GateTrigger : GateTriggers)
			{
				GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(true);
			break;
		}
		case ESectionState::Batte:
		{
			Trigger->SetCollisionProfileName(TEXT("NoCollision"));
			for(UBoxComponent* GateTrigger : GateTriggers)
			{
				GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
			}
			OperateGates(false);
			break;
		}
		case ESectionState::Complete:
		{
			Trigger->SetCollisionProfileName(TEXT("NoCollision"));
			for(UBoxComponent* GateTrigger : GateTriggers)
			{
				GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
			}
			OperateGates(true);
			break;
		}
		default:
		{
			break;
		}
	}

	CurrentState = NewState;
}


void AABSection::OperateGates(bool bOpen)
{
	for(UStaticMeshComponent* Gate : GateMeshes)
	{
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
}


void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(CurrentState == ESectionState::Ready)
	{
		SetState(ESectionState::Batte);
	}
}


void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABCHECK(OverlappedComponent->ComponentTags.Num() == 1);

	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if(Mesh->DoesSocketExist(SocketName) == false)
		return;
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);

	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults
		, NewLocation
		, FQuat::Identity
		, ObjectQueryParam
		, FCollisionShape::MakeSphere(775.0f)
		, CollisionQueryParam
	);

	if(bResult == false)
	{
		auto NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
	else
	{
		ABLOG(Warning, TEXT("New Section area is not empty"));
	}
}


// Called every frame
void AABSection::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


/**
 * @brief 에디터작업에서 선택한 엑터의 속성이나 트랜스폼 정보가 변경될때 함수가 호출됨. 
 * @param Transform 
 */
void AABSection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ESectionState sectionState = bNoBattle ? ESectionState::Complete : ESectionState::Ready;
	SetState(sectionState);
}

