// Fill out your copyright notice in the Description page of Project Settings.


#include "ABPlayerCharacter.h"

#include <memory>

#include "ABWeapon.h"
#include "ABGameInstance.h"
#include "ABAnimInstance.h"
#include "ABCharacterWidget.h"
#include "ABCharacterSetting.h"
#include "ABCharacterStatComponent.h"
#include "Components/WidgetComponent.h"
#include "ABPlayerController.h"
#include "ABPlayerState.h"
#include "ABHUDWidget.h"
#include "ABPlayerController.h"

#include "ABGameMode.h"
//#include "DrawDebugHelpers.h"



#define ASSET_INDEX (1)
/**
 * @brief 생성자
 */
AABPlayerCharacter::AABPlayerCharacter()
	: DirectionToMove(FVector::ZeroVector)
	, ArmLengthTo(0.0f)
	, ArmRotationTo(FRotator::ZeroRotator)
	, ArmLengthSpeed(3.0f)
	, ArmRotationSpeed(10.0f)
	, IsAttacking(false)
	, MaxCombo(4)
	, AttackRange(80.0f)
	, AttackRadius(50.0f)
	, DeadTimer(5.0f)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// >> Init Camera
	_initCamera();
	SetControlMode(EControlMode::DIABLO);
	// << Init Camera
	
	// >> Inti UI
	_initUI();
	HPBarWidget->SetHiddenInGame(true);
	// << Inti UI

	// >> Init Mesh
	_initMesh();
	// << Init Mesh
	
	// >> Init Animation
	_initAnimation();
	AttackEndComboState();
	// << Init Animation

	// >> Init Character Stat
	_initCharacterStat();
	// << Init Character Stat

	// >> Init Physics
	_initPhysics();
	// << Init Physics


	
	


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


	// >> Preint state
	SetActorHiddenInGame(true);
	SetCanBeDamaged(false);
	// << Preint state
}


// Called when the game starts or when spawned
void AABPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


	const auto DefaultSetting = GetDefault<UABCharacterSetting>();

	
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[ASSET_INDEX];
	const auto ABGameInstance = Cast<UABGameInstance>(GetGameInstance());
	ABCHECK(ABGameInstance);
	AssetStreamingHandle = ABGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad
		, FStreamableDelegate::CreateUObject(this, &AABPlayerCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::Loading);
}


/**
 * @brief 카메라 시점 설정
 * @param ControlMode 카메라 시점
 */
void AABPlayerCharacter::SetControlMode(const EControlMode& ControlMode)
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
void AABPlayerCharacter::Tick(float DeltaTime)
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
void AABPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("ViewChange"), EInputEvent::IE_Pressed, this, &AABPlayerCharacter::_viewChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AABPlayerCharacter::Attack);
	
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AABPlayerCharacter::_upDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AABPlayerCharacter::_leftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AABPlayerCharacter::_lookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AABPlayerCharacter::_turn);
}


void AABPlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	ABAnim = Cast<UABAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != ABAnim);
	ABAnim->OnMontageEnded.AddDynamic(this, &AABPlayerCharacter::OnAttackMontageEnded); // 애니메이션 몽타주 재생이 끝나면 호출되는 콜백함수.

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
	ABAnim->OnAttackHitCheck.AddUObject(this, &AABPlayerCharacter::_attackCheck);

	// HP가 0일때 호출할 함수.
	CharacterStat->OnHPIsZero.AddLambda([this]()->void
	{
		ABLOG(Warning, TEXT("OnHPIsZero"));
		ABAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});
}


void AABPlayerCharacter::PossessedBy(AController* NewController)
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
 * @brief 데미지를 주는 함수
 * @param DamageAmount 데미지량
 * @param DamageEvent 데미지의 종류
 * @param EventInstigator 가해자(폰에게 명령을 내린 컨트롤러)
 * @param DamageCauser 데미지 전달을 위해 사용한 도구(현재 이 캐릭터가 도구가 된다.)
 * @return 실제 받은 데미지.
 */
float AABPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	const float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	//ABLOG(Warning, TEXT("Actor : %s took Damage : %f"), *GetName(), FinalDamage);
	//
	//CharacterStat->SetDamage(FinalDamage);
	//
	//if(CurrentState == ECharacterState::Dead)
	//{
	//	if(EventInstigator->IsPlayerController())
	//	{
	//		auto PlayerController = Cast<AABPlayerController>(EventInstigator);
	//		//ABCHECK(nullptr != PlayerController);
	//		PlayerController->NPCKill(this);
	//	}
	//}
	
	
	return FinalDamage;
}


/**
 * @brief 무기를 장착할 수 있는지에 대한 여부 반환
 * @return 무기장착가능 여부 true/false
 */
bool AABPlayerCharacter::CanSetWeapon() const
{
	//return (nullptr == CurrentWeapon);
	return true;
}


/**
 * @brief 캐릭터에 무기를 장착한다.
 * @param NewWeapon 장착하는 무기 
 */
void AABPlayerCharacter::SetWeapon(AABWeapon* NewWeapon)
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
 * @brief 모델의 Y축방향으로 이동
 * @param NewAxisValue 이동량
 */
void AABPlayerCharacter::_upDown(const float NewAxisValue)
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
void AABPlayerCharacter::_leftRight(float NewAxisValue)
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
void AABPlayerCharacter::_lookUp(float NewAxisValue)
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
void AABPlayerCharacter::_turn(float NewAxisValue)
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
void AABPlayerCharacter::_viewChange()
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
void AABPlayerCharacter::Attack()
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


/**
 * @brief State를 설정한다
 * @param paramState 설정할 State
 */
void AABPlayerCharacter::SetCharacterState(const ECharacterState& paramState)
{
	ABCHECK(CurrentState != paramState);
	CurrentState = paramState;

	switch (CurrentState)
	{
	case ECharacterState::Loading:
		{
			DisableInput(_getPlayerController());

			_getPlayerController()->GetHUDWidget()->BindCharacterStat(CharacterStat);
				
			const auto ABPlayerState = Cast<AABPlayerState>(GetPlayerState());
			ABCHECK(nullptr != ABPlayerState);
			CharacterStat->SetNewLevel(ABPlayerState->GetCharacterLevel());

			
			SetActorHiddenInGame(true);
			HPBarWidget->SetHiddenInGame(true);
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
			
			
			SetControlMode(EControlMode::DIABLO);
			GetCharacterMovement()->MaxWalkSpeed = 600.0f;
			EnableInput(_getPlayerController());
			
			break;
		}
	case ECharacterState::Dead:
		{
			SetActorEnableCollision(false);
			GetMesh()->SetHiddenInGame(false);
			HPBarWidget->SetHiddenInGame(true);
			ABAnim->SetDeadAnim();
			SetCanBeDamaged(false);

			
			DisableInput(_getPlayerController());

			GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]()->void
			{
				_getPlayerController()->RestartLevel();
			}), DeadTimer, false);
			break;
		}

	default:
		{
			break;
		}
	}
}


ECharacterState AABPlayerCharacter::GetCharacterState() const
{
	return CurrentState;
}


void AABPlayerCharacter::_attackCheck()
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

//#if ENABLE_DRAW_DEBUG
//	FVector TraceVec = GetActorForwardVector() * FinalAttackRange;
//	FVector Center = GetActorLocation() + TraceVec * 0.5f;
//	float HalfHeight = FinalAttackRange * 0.5f + AttackRadius;
//	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
//	FColor DrawColor = bResult ? FColor::Red : FColor::Green;
//	float DebugLifeTime = 5.0f;
//
//	DrawDebugCapsule(GetWorld()
//		, Center
//		, HalfHeight
//		, AttackRadius
//		, CapsuleRot
//		, DrawColor
//		, false
//		, DebugLifeTime);
//#endif
	

	
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


void AABPlayerCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	ABCHECK(IsAttacking);
	ABCHECK(CurrentCombo > 0);
	IsAttacking = false;
	AttackEndComboState();
	OnAttackEnd.Broadcast();
}


void AABPlayerCharacter::AttackStartComboState()
{
	CanNextCombo = true;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1 , MaxCombo);
}


void AABPlayerCharacter::AttackEndComboState()
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}


void AABPlayerCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(AssetLoaded != nullptr);
	//GetMesh()->SetSkeletalMesh(AssetLoaded);

	SetCharacterState(ECharacterState::Ready);
}


/**
 * @brief 카메라 초기화.
 */
void AABPlayerCharacter::_initCamera()
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	
	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
}


/**
 * @brief UI 초기화
 */
void AABPlayerCharacter::_initUI()
{
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));
	HPBarWidget->SetupAttachment(GetMesh());

	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/UI_HPBar.UI_HPBar_C'"));
	if(UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}
}


/**
 * @brief Mesh 초기화
 */
void AABPlayerCharacter::_initMesh()
{
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SK_Cardboard(TEXT("SkeletalMesh'/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Barbarous.SK_CharM_Barbarous'"));
	if(SK_Cardboard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(SK_Cardboard.Object);
	}
}


/**
 * @brief Animation 초기화
 */
void AABPlayerCharacter::_initAnimation()
{
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("AnimBlueprint'/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C'"));
	if(WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}
}


/**
 * @brief Character 스탯 초기화
 */
void AABPlayerCharacter::_initCharacterStat()
{
	CharacterStat = CreateDefaultSubobject<UABCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	GetCharacterMovement()->JumpZVelocity = 400.0f;
}


/**
 * @brief 물리관련(충돌 포함) 초기화
 */
void AABPlayerCharacter::_initPhysics()
{
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("ABCharacter"));
}


/**
 * @brief PlayerController 반환
 * @return AABPlayerController
 */
AABPlayerController* AABPlayerCharacter::_getPlayerController() const
{
	return Cast<AABPlayerController>(GetController());
}


int32 AABPlayerCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}

float AABPlayerCharacter::GetFinalAttackRange() const
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

float AABPlayerCharacter::GetFinalAttackDamage() const
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
