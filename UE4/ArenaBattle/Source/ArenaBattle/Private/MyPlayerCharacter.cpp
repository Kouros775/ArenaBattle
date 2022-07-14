// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerCharacter.h"

#include "ArenaBattle.h"
#include "DrawDebugHelpers.h"


// Sets default values
AMyPlayerCharacter::AMyPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_initSkeletalMesh();
	_initCamera();
	_initCollision();
	_initAnimation();
}

// Called when the game starts or when spawned
void AMyPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}



// Called every frame
void AMyPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	// >> 캐릭터 이동 & 카메라 이동
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AMyPlayerCharacter::_upDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AMyPlayerCharacter::_leftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AMyPlayerCharacter::_lookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AMyPlayerCharacter::_turn);
	// << 캐릭터 이동 & 카메라 이동
	
	// >> 캐릭터 액션.
	PlayerInputComponent->BindAction(TEXT("Interact"), EInputEvent::IE_Pressed, this, &AMyPlayerCharacter::_interact);
	// << 캐릭터 액션.
}


/**
 * @brief 카메라를 생성하고 초기화한다.
 */
void AMyPlayerCharacter::_initCamera()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 70.0f));
	SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
	SpringArm->TargetArmLength = 260.0f;
	//ArmLengthTo = 450.0f;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritRoll = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bDoCollisionTest = true;
	bUseControllerRotationYaw = false;

		
	GetCharacterMovement()->bOrientRotationToMovement = true; // 캐릭터가 움직이는 방향으로 자동으로 회전시켜주는 플래그.
	GetCharacterMovement()->bUseControllerDesiredRotation = false; // 컨트롤 회전이 가리키는 방향으로 캐릭터가 부드럽게 회전한다.
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
}



/**
 * @brief 캐릭터의 메쉬를 초기화한다.
 */
void AMyPlayerCharacter::_initSkeletalMesh() const
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Cardboard(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous'"));
	if(SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}
}


/**
 * @brief 캐릭터의 충돌 관련하여 초기화한다.
 */
void AMyPlayerCharacter::_initCollision()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("NPC"));

	if(IsPlayerControlled())
	{
		GetCapsuleComponent()->SetCollisionProfileName(TEXT("MyPlayer"));
	}

	//SetActorEnableCollision(false);
}


/**
 * @brief 캐릭터의 애니메이션 관련하여 초기화한다. 
 */
void AMyPlayerCharacter::_initAnimation()
{
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> animBlueprint(TEXT
		("AnimBlueprint'/Game/Animations/MyPlayerAnimBlueprint.MyPlayerAnimBlueprint_C'"));
	if(animBlueprint.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(animBlueprint.Class);
	}
}


/**
 * @brief 캐릭터의 앞뒤 이동
 * @param paramAxis 축정보 
 */
void AMyPlayerCharacter::_upDown(const float paramAxis)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X)
		, paramAxis);
}


/**
 * @brief 캐릭터의 왼쪽 오른쪽 이동 
 * @param paramAxis 축정보
 */
void AMyPlayerCharacter::_leftRight(const float paramAxis)
{
	AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y)
		, paramAxis);
}


/**
 * @brief 캐릭터의 Y축 방향 회전
 * @param paramAxis 축정보
 */
void AMyPlayerCharacter::_lookUp(const float NewAxisValue)
{
	AddControllerPitchInput(NewAxisValue);
}


/**
 * @brief 캐릭터의 Z축 방향 회전
 * @param paramAxis 축정보
 */
void AMyPlayerCharacter::_turn(const float NewAxisValue)
{
	AddControllerYawInput(NewAxisValue);
}


/**
 * @brief Actor와 상호작용하는 함수
 */
void AMyPlayerCharacter::_interact()
{
	const float FinalAttackRange = 100.0f;
	const float checkRadius = 100.0f;
	
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	auto gameTraceChannel = (IsPlayerControlled() == true) ? ECollisionChannel::ECC_GameTraceChannel4 : ECollisionChannel::ECC_GameTraceChannel5;
	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult
		, GetActorLocation()
		, GetActorLocation() + GetActorForwardVector() * FinalAttackRange
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel2
		, FCollisionShape::MakeSphere(checkRadius)
		, Params
	);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
                                                      	float HalfHeight = FinalAttackRange * 0.5f + checkRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld()
		, Center
		, HalfHeight
		, checkRadius
		, CapsuleRot
		, DrawColor
		, false
		, DebugLifeTime);
#endif
	

	
	if(bResult)
	{
		if(HitResult.Actor.IsValid())
		{
			ABLOG(Warning, TEXT("Hit Actor Name : %s"), *HitResult.Actor->GetName());
		}
	}
}