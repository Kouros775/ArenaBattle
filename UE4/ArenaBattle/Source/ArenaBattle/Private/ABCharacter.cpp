// Fill out your copyright notice in the Description page of Project Settings.


#include "ABCharacter.h"

#include <memory>

#include "ABWeapon.h"
#include "ABGameInstance.h"
#include "ABAnimInstance.h"
#include "ABAIController.h"
#include "ABCharacterWidget.h"
#include "ABCharacterSetting.h"
#include "ABCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABGameMode.h"


AABCharacter::AABCharacter()
	: DirectionToMove(FVector::ZeroVector)
	, ArmLengthTo(0.0f)
	, ArmRotationTo(FRotator::ZeroRotator)
	, ArmLengthSpeed(3.0f)
	, ArmRotationSpeed(10.0f)
	, IsAttacking(false)
	, MaxCombo(4)
	, AttackRange(80.0f)
	, AttackRadius(50.0f)
	, AssetIndex(4)
	, DeadTimer(5.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	
	
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());
	
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Cardboard(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous'"));
	if(SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("AnimBlueprint'/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C'"));
	if(WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
	
	SetControlMode(EControlMode::DIABLO);

	// ?????? value
	GetCharacterMovement()->JumpZVelocity = 400.0f;

	AttackEndComboState();
	
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));


	// >> HP Bar Widget
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/UI_HPBar.UI_HPBar_C'"));
	if(UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}
	// << HP Bar Widget

	// >> AI Controller Set
	// ??????????????? ???????????? ???????????? ????????? ?????? ???????????? ai set
	AIControllerClass = AABAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	// << AI Controller Set


	// >> ArenaBattle Setting
	//const auto DefaultSetting = GetDefault<UABCharacterSetting>();
	//if(DefaultSetting->CharacterAssets.Num() > 0)
	//{
	//	for(auto CharacterAsset : DefaultSetting->CharacterAssets)
	//	{
	//		ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
	//	}
	//}

	CharacterAssetToLoad = FSoftObjectPath(nullptr);
	// << ArenaBattle Setting


	// >>
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	//bCanBeDamaged = false;
	SetCanBeDamaged(false);
	
	// <<
	
}


// Called when the game starts or when spawned
void AABCharacter::BeginPlay()
{
	Super::BeginPlay();

	bIsPlayer = IsPlayerControlled();
	if(bIsPlayer == true)
	{
		ABPlayerController = Cast<AABPlayerController>(GetController());
		ABCHECK(ABPlayerController);
	}
	else
	{
		ABAIController = Cast<AABAIController>(GetController());
		ABCHECK(ABAIController);
	}

	const auto DefaultSetting = GetDefault<UABCharacterSetting>();

	if(bIsPlayer == true)
	{
		AssetIndex = 4;
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}

	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	const auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad
		, FStreamableDelegate::CreateUObject(this, &AABCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::Loading);
}


/**
 * @brief ????????? ?????? ??????
 * @param ControlMode ????????? ??????
 */
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

		
		GetCharacterMovement()->bOrientRotationToMovement = true; // ???????????? ???????????? ???????????? ???????????? ?????????????????? ?????????.
		GetCharacterMovement()->bUseControllerDesiredRotation = false; // ????????? ????????? ???????????? ???????????? ???????????? ???????????? ????????????.
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
		GetCharacterMovement()->bUseControllerDesiredRotation = true; // ????????? ????????? ???????????? ???????????? ???????????? ???????????? ????????????.
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
	}
	else if(ControlMode == EControlMode::NPC)
	{
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
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
	ABAnim->OnMontageEnded.AddDynamic(this, &AABCharacter::OnAttackMontageEnded); // ??????????????? ????????? ????????? ????????? ???????????? ????????????.

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

	// Attack??? Collision ??????.
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABCharacter::AttackCheck);

	// HP??? 0?????? ????????? ??????.
	CharacterStat->OnHPIsZero.AddLambda([this]()->void
	{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});
}


void AABCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(IsPlayerControlled())
	{
		SetControlMode(EControlMode::DIABLO);
		GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	}
	else
	{
		SetControlMode(EControlMode::NPC);
		GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	}
}


/**
 * @brief ???????????? ?????? ??????
 * @param DamageAmount ????????????
 * @param DamageEvent ???????????? ??????
 * @param EventInstigator ?????????(????????? ????????? ?????? ????????????)
 * @param DamageCauser ????????? ????????? ?????? ????????? ??????(?????? ??? ???????????? ????????? ??????.)
 * @return ?????? ?????? ?????????.
 */
float AABCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);

	CharacterStat->SetDamage(FinalDamage);

	if(CurrentState == ECharacterState::Dead)
	{
		if(EventInstigator->IsPlayerController())
		{
			auto PlayerController = Cast<AABPlayerController>(EventInstigator);
			//ABCHECK(nullptr != PlayerController);
			PlayerController->NPCKill(this);
		}
	}
	
	
	return FinalDamage;
}


/**
 * @brief ????????? ????????? ??? ???????????? ?????? ?????? ??????
 * @return ?????????????????? ?????? true/false
 */
bool AABCharacter::CanSetWeapon() const
{
	//return (nullptr == CurrentWeapon);
	return true;
}


/**
 * @brief ???????????? ????????? ????????????.
 * @param NewWeapon ???????????? ?????? 
 */
void AABCharacter::SetWeapon(AABWeapon* NewWeapon)
{
	ABCHECK(nullptr != NewWeapon);
	if(nullptr != CurrentWeapon)
	{
		CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}
	

	
	const FName WeaponSocket(TEXT("hand_rSocket"));
	if(NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
	}
}


/**
 * @brief ????????? Y??????????????? ??????
 * @param NewAxisValue ?????????
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
 * @brief ????????? X??????????????? ??????
 * @param NewAxisValue ?????????
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
 * @brief ????????? X??? ?????? (????????? Y??? ??????)
 * @param NewAxisValue ?????????
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
/// ????????? Z?????? ???????????? ????????? ??????.
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
/// ????????? ?????? ??????.
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
/// ??????.
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

void AABCharacter::SetCharacterState(const ECharacterState& NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::Loading:
		{
			if(bIsPlayer)
			{
				DisableInput(ABPlayerController);

				ABPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);
				
				const auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
				ABCHECK(nullptr != ABPlayerState);
				CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());
			}
			else
			{
				auto ABGameMode = Cast<AABGameMode>(GetWorld()->GetAuthGameMode());
				ABCHECK(nullptr != ABGameMode);
				int32 TargetLevel = FMath::CeilToInt((float)ABGameMode->GetScore() * 0.8f);
				int32 FinalLevel = FMath::Clamp<int32>(TargetLevel, 1, 20);
				ABLOG(Warning, TEXT("New NPC Level : %d"), FinalLevel);
				CharacterStat->SetNewLevel(FinalLevel);
			}
			SetActorHiddenInGame(true);
			HPBarWidget->SetHiddenInGame(true);
			//bCanBeDamaged = false;
			SetCanBeDamaged(false);
			break;
		}
	case ECharacterState::Ready:
		{
			SetActorHiddenInGame(false);
			HPBarWidget->SetHiddenInGame(false);
			SetCanBeDamaged(true);

			CharacterStat->OnHPIsZero.AddLambda([this]()->void
			{
				SetCharacterState(ECharacterState::Dead);
			});

			const auto CharacterWidget = Cast<UABCharacterWidget>(HPBarWidget->GetUserWidgetObject());
			ABCHECK(nullptr != CharacterWidget);
			CharacterWidget->BindCharacterStat(CharacterStat);
			
			if(bIsPlayer)
			{
				SetControlMode(EControlMode::DIABLO);
				GetCharacterMovement()->MaxWalkSpeed = 600.0f;
				EnableInput(ABPlayerController);
			}
			else
			{
				SetControlMode(EControlMode::NPC);
				GetCharacterMovement()->MaxWalkSpeed = 400.0f;
				ABAIController->RunAI();
			}
			break;
		}
	case ECharacterState::Dead:
		{
			SetActorEnableCollision(false);
			GetMesh()->SetHiddenInGame(false);
			HPBarWidget->SetHiddenInGame(true);
			ABAnim->SetDeadAnim();
			SetCanBeDamaged(false);

			if(bIsPlayer)
			{
				DisableInput(ABPlayerController);
			}
			else
			{
				ABAIController->StopAI();
			}

			GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
			{
				if(bIsPlayer)
				{
					ABPlayerController->RestartLevel();
				}
				else
				{
					Destroy();
				}
			}), DeadTimer, false);
			break;
		}

	default:
		{
			break;
		}
	}
}


ECharacterState AABCharacter::GetCharacterState() const
{
	return CurrentState;
}


void AABCharacter::AttackCheck()
{
	float FinalAttackRange = GetFinalAttackRange();
	
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	bool bResult = GetWorld()->SweepSingleByChannel(
		HitResult
		, GetActorLocation()
		, GetActorLocation() + GetActorForwardVector() * FinalAttackRange
		, FQuat::Identity
		, ECollisionChannel::ECC_GameTraceChannel2
		, FCollisionShape::MakeSphere(50.0f)
		, Params
	);

#if ENABLE_DRAW_DEBUG
	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
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
			HitResult.Actor->TakeDamage(GetFinalAttackDamage(), DamageEvent, GetController(), this);
		}
	}
}


void AABCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
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


void AABCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(AssetLoaded != nullptr);
	GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetCharacterState(ECharacterState::Ready);
}


int32 AABCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABCharacter::GetFinalAttackRange() const
{
	float attackRange;

	if(nullptr != CurrentWeapon)
		attackRange = CurrentWeapon->GetAttackRange();
	else
	{
		attackRange = this->AttackRange;
	}

	return attackRange;
}

float AABCharacter::GetFinalAttackDamage() const
{
	float rtn = 0.0f;

	if(nullptr != CurrentWeapon)
	{
		rtn = (CharacterStat->GetAttack() + CurrentWeapon->GetAttackDamage()) * CurrentWeapon->GetAttackModifier();
	}
	else
	{
		rtn = CharacterStat->GetAttack() * 1.0f;
	}

	return rtn;
}
