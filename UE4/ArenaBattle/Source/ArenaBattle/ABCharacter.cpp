// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"
#include "ABAnimInstance.h"
#include "ABWeapon.h"
#include "DrawDebugHelpers.h"


AABCharacter::AABCharacter()
	: CurrentControlMode(EControlMode::GTA)
	, DirectionToMove(FVector::ZeroVector)
	, ArmLengthTo(0.0f)
	, ArmRotationTo(FRotator::ZeroRotator)
	, ArmLengthSpeed(3.0f)
	, ArmRotationSpeed(10.0f)
	, IsAttacking(false)
	, MaxCombo(4)
	, AttackRange(200.0f)
	, AttackRadius(50.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 400.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_CARDBOARD(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous'"));
	if(SK_CARDBOARD.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_CARDBOARD.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("AnimBlueprint'/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C'"));
	if(WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
	
	SetControlMode(EControlMode::DIABLO);

	// 점프 value
	GetCharacterMovement()->JumpZVelocity = 800.0f;

	AttackEndComboState();
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
}


// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();
}


void AABCharacter::SetControlMode(const EControlMode& ControlMode)
{
	CurrentControlMode = ControlMode;
	
	if(ControlMode == EControlMode::GTA)
	{
		//SpringArm->TargetArmLength = 450.0f;
		//SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		ArmLengthTo = 450.0f;
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
	else if(ControlMode == EControlMode::DIABLO)
	{
		//SpringArm->TargetArmLength = 800.0f;
		//SpringArm->SetRelativeRotation(FRotator(-45.0f, 0.0f, 0.0f));
		ArmLengthTo = 800.0f;
		ArmRotationTo = FRotator(-45.0f, 0.0f, 0.0f);
		SpringArm->bUsePawnControlRotation = false;
		SpringArm->bInheritYaw = false;
		SpringArm->bInheritPitch = false;
		SpringArm->bInheritRoll = false;
		SpringArm->bDoCollisionTest = false;
		
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true; // 컨트롤 회전이 가리키는 방향으로 캐릭터가 부드럽게 회전한다.
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}
}


// Called every frame
void AABCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);

	if(CurrentControlMode == EControlMode::GTA)
	{
		// nothing.
	}
	else if(CurrentControlMode == EControlMode::DIABLO)
	{
		SpringArm->SetRelativeRotation(FMath::RInterpTo(SpringArm->GetRelativeRotation(), ArmRotationTo, DeltaTime, ArmRotationSpeed));
	}

	
	if(CurrentControlMode == EControlMode::GTA)
	{
		// nothing.
	}
	else if(CurrentControlMode == EControlMode::DIABLO)
	{
		if(DirectionToMove.SizeSquared() > 0.0f)
		{
			GetController()->SetControlRotation(FRotationMatrix::MakeFromX(DirectionToMove).Rotator());
			AddMovementInput(DirectionToMove);
		}
	}
}

// Called to bind functionality to input
void AABCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABCharacter::ViewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABCharacter::Attack);
	
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABCharacter::Turn);
}


void AABCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded); // 애니메이션 몽타주 재생이 끝나면 호출되는 콜백함수.

	ABAnim->OnNextAttackCheck.AddLambda([this]()->void
	{
		ABLOG(Warning, TEXT("OnNextAttackCheck"));
		CanNextCombo = false;

		if(IsComboInputOn)
		{
			AttackStartComboState();
			ABAnim->JumpToAttackMontageSection(CurrentCombo);
		}
	});

	// Attack시 Collision 확인.
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);
}


/**
 * @brief 데미지를 주는 함수
 * @param DamageAmount 데미지량
 * @param DamageEvent 데미지의 종류
 * @param EventInstigator 가해자(폰에게 명령을 내린 컨트롤러)
 * @param DamageCauser 데미지 전달을 위해 사용한 도구(현재 이 캐릭터가 도구가 된다.)
 * @return 실제 받은 데미지.
 */
float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	if(FinalDamage > 0.0f)
	{
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	}
	
	return FinalDamage;
}


/**
 * @brief 무기를 장착할 수 있는지에 대한 여부 반환
 * @return 무기장착가능 여부 true/false
 */
bool AABCharacter::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}


/**
 * @brief 캐릭터에 무기를 장착한다.
 * @param NewWeapon 장착하는 무기 
 */
void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	FName WeaponSocket(TEXT("hand_rSocket"));
	if(NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}


/**
 * @brief 모델의 Y축방향으로 이동
 * @param NewAxisValue 이동량
 */
void AABCharacter::UpDown(const float NewAxisValue)
{
	if(CurrentControlMode == EControlMode::GTA)
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::X), NewAxisValue);
	}
	else if(CurrentControlMode ==EControlMode::DIABLO)
	{
		DirectionToMove.X = NewAxisValue;
	}
}


/**
 * @brief 모델의 X축방향으로 이동
 * @param NewAxisValue 이동량
 */
void AABCharacter::LeftRight(float NewAxisValue)
{
	if(CurrentControlMode == EControlMode::GTA)
	{
		AddMovementInput(FRotationMatrix(FRotator(0.0f, GetControlRotation().Yaw, 0.0f)).GetUnitAxis(EAxis::Y), NewAxisValue);
	}
	else if(CurrentControlMode ==EControlMode::DIABLO)
	{
		DirectionToMove.Y = NewAxisValue;
	}
}


/**
 * @brief 모델의 X축 회전 (모델의 Y축 방향)
 * @param NewAxisValue 이동량
 */
void AABCharacter::LookUp(float NewAxisValue)
{
	if(CurrentControlMode == EControlMode::GTA)
	{
		AddControllerPitchInput(NewAxisValue);
	}
	else if(CurrentControlMode ==EControlMode::DIABLO)
	{
		// nothing.
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 모델의 Z축을 기준으로 모델을 회전.
void AABCharacter::Turn(float NewAxisValue)
{
	if(CurrentControlMode == EControlMode::GTA)
	{
		AddControllerYawInput(NewAxisValue);
	}
	else if(CurrentControlMode ==EControlMode::DIABLO)
	{
		// nothing.
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 카메라 시점 변경.
void AABCharacter::ViewChange()
{
	if(CurrentControlMode == EControlMode::GTA)
	{
		GetController()->SetControlRotation(GetActorRotation());
		SetControlMode(EControlMode::DIABLO);
	}
	else if(CurrentControlMode ==EControlMode::DIABLO)
	{
		GetController()->SetControlRotation(SpringArm->GetRelativeRotation());
		SetControlMode(EControlMode::GTA);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// 공격.
void AABCharacter::Attack()
{
	if(IsAttacking)
	{
		ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));

		if(CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else
	{
		ABCHECK(CurrentCombo == 0);
		AttackStartComboState();
		ABAnim->PlayAttackMontage();
		ABAnim->JumpToAttackMontageSection(CurrentCombo);
		IsAttacking = true;
	}
}


void AABCharacter::AttackCheck()
{
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult
		, GetActorLocation()
		, GetActorLocation() + GetActorForwardVector() * 200.0f
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel2
		, FCollisionShape::MakeSphere(50.0f)
		, Params
	);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(GetWorld()
		, Center
		, HalfHeight
		, AttackRadius
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

			FDamageEvent DamageEvent;
			HitResult.Actor->TakeDamage(50.0f, DamageEvent, GetController(), this);
		}
	}
}

void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
}


void AABCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1 , MaxCombo);
}


void AABCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}
