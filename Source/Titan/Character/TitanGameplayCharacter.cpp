// Copyright Epic Games, Inc. All Rights Reserved.


#include "Character/TitanGameplayCharacter.h"

#include "Logging/TitanLogChannels.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/PoseableMeshComponent.h"
#include "TitanCameraComponent.h"
#include "PlayerInteractionComponent.h"

#include "InputAction.h"
#include "EnhancedInputComponent.h"

#include "GAS/SampleAbilitySystemComponent.h"

#include "TitanMoverComponent.h"
#include "TitanMoverTypes.h"
#include "MoveLibrary/MovementUtils.h"
#include "MoveLibrary/BasedMovementUtils.h"

#include "TitanRaftActor.h"
#include "TitanRaftTeleportEffect.h"
#include "TitanLayeredMove_Jump.h"

#include "Engine/CollisionProfile.h"
#include "TitanWaterDetectionComponent.h"
#include "Engine/World.h"
#include "TitanTeleportPreloader.h"
#include "Framework/SamplePlayerController.h"
#include "Framework/SamplePlayerState.h"
#include "Framework/TitanGameplayPlayerState.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimInstance.h"
#include "Components/SceneComponent.h"
#include "InventoryManagerComponent.h"
#include "QuestComponent.h"


UE_DEFINE_GAMEPLAY_TAG(TAG_Titan_Character_MovementModeChanged, "Titan.Character.MovementModeChanged");

ATitanGameplayCharacter::ATitanGameplayCharacter(const FObjectInitializer& ObjectInitializer)
: ASamplePlayerPawn(ObjectInitializer)
{
	WindVelocity = FVector::ZeroVector;

	// create the collision capsule
	PlayerCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Player Capsule"));

	check(PlayerCapsule);

	SetRootComponent(PlayerCapsule);
	PlayerCapsule->InitCapsuleSize(34.0f, 88.0f);
	PlayerCapsule->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);

	PlayerCapsule->CanCharacterStepUpOn = ECB_No;
	PlayerCapsule->SetShouldUpdatePhysicsVolume(true);
	PlayerCapsule->SetCanEverAffectNavigation(false);
	PlayerCapsule->bDynamicObstacle = true;

	// create the torso skeletal mesh
	TorsoMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso Mesh"));

	check(TorsoMesh);

	TorsoMesh->SetupAttachment(PlayerCapsule);
	TorsoMesh->AlwaysLoadOnClient = true;
	TorsoMesh->AlwaysLoadOnServer = true;
	TorsoMesh->bOwnerNoSee = false;
	TorsoMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	TorsoMesh->bCastDynamicShadow = true;
	TorsoMesh->bAffectDynamicIndirectLighting = true;
	TorsoMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	static FName MeshCollisionProfileName(TEXT("CharacterMesh"));
	TorsoMesh->SetCollisionProfileName(MeshCollisionProfileName);
	TorsoMesh->SetGenerateOverlapEvents(false);
	TorsoMesh->SetCanEverAffectNavigation(false);

	// create the head skinned mesh
	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head Mesh"));

	check(HeadMesh);

	HeadMesh->SetupAttachment(TorsoMesh);
	HeadMesh->AlwaysLoadOnClient = true;
	HeadMesh->AlwaysLoadOnServer = true;
	HeadMesh->bOwnerNoSee = false;
	HeadMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	HeadMesh->bCastDynamicShadow = true;
	HeadMesh->bAffectDynamicIndirectLighting = true;
	HeadMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	HeadMesh->SetCollisionProfileName(MeshCollisionProfileName);
	HeadMesh->SetGenerateOverlapEvents(false);
	HeadMesh->SetCanEverAffectNavigation(false);

	// create the headwear mesh
	HeadwearMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Headwear Mesh"));

	check(HeadwearMesh);

 	HeadwearMesh->SetupAttachment(TorsoMesh);
	HeadwearMesh->AlwaysLoadOnClient = true;
	HeadwearMesh->AlwaysLoadOnServer = true;
	HeadwearMesh->bOwnerNoSee = false;
	HeadwearMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	HeadwearMesh->bCastDynamicShadow = true;
	HeadwearMesh->bAffectDynamicIndirectLighting = true;
	HeadwearMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	HeadwearMesh->SetCollisionProfileName(MeshCollisionProfileName);
	HeadwearMesh->SetGenerateOverlapEvents(false);
	HeadwearMesh->SetCanEverAffectNavigation(false);
	

	// create the lower body mesh
	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs Mesh"));

	check(LegsMesh);

	LegsMesh->SetupAttachment(TorsoMesh);
	LegsMesh->AlwaysLoadOnClient = true;
	LegsMesh->AlwaysLoadOnServer = true;
	LegsMesh->bOwnerNoSee = false;
	LegsMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	LegsMesh->bCastDynamicShadow = true;
	LegsMesh->bAffectDynamicIndirectLighting = true;
	LegsMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	LegsMesh->SetCollisionProfileName(MeshCollisionProfileName);
	LegsMesh->SetGenerateOverlapEvents(false);
	LegsMesh->SetCanEverAffectNavigation(false);
	
	// create the glider skeletal mesh
	GliderMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Glider Mesh"));

	check(GliderMesh);

	GliderMesh->SetupAttachment(PlayerCapsule);
	GliderMesh->AlwaysLoadOnClient = true;
	GliderMesh->AlwaysLoadOnClient = true;
	GliderMesh->AlwaysLoadOnServer = true;
	GliderMesh->bOwnerNoSee = false;
	GliderMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPose;
	GliderMesh->bCastDynamicShadow = true;
	GliderMesh->bAffectDynamicIndirectLighting = true;
	GliderMesh->PrimaryComponentTick.TickGroup = TG_PrePhysics;

	// create the camera
	Camera = CreateDefaultSubobject<UTitanCameraComponent>(TEXT("Camera"));

	check(Camera);

	Camera->SetupAttachment(PlayerCapsule);

	// create the interaction pivot
	InteractionPivot = CreateDefaultSubobject<USceneComponent>(TEXT("Interaction Pivot"));

	check(InteractionPivot);

	InteractionPivot->SetupAttachment(RootComponent);

	// create the interaction box
	InteractionBox = CreateDefaultSubobject<UPlayerInteractionComponent>(TEXT("Interaction Box"));

	check(InteractionBox);

	InteractionBox->SetupAttachment(InteractionPivot);

	// create the Mover component
	CharacterMotionComponent = CreateDefaultSubobject<UTitanMoverComponent>(TEXT("MoverComponent"));
	ensure(CharacterMotionComponent);

	// create the water detection comp
	WaterDetection = CreateDefaultSubobject<UTitanWaterDetectionComponent>(TEXT("Water Detection"));
	check(WaterDetection);

	// add water detection as a tick prerequisite
	AddTickPrerequisiteComponent(WaterDetection);

	// disable Actor-level movement replication, since our Mover component will handle it
	SetReplicatingMovement(false);
}

ATitanGameplayPlayerState* ATitanGameplayCharacter::GetTitanPlayerState()
{
	return TitanPlayerState;
}

void ATitanGameplayCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATitanGameplayCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (ensure(CharacterMotionComponent))
	{
		CharacterMotionComponent->InputProducer = this;

		// register to the movement mode changed delegate
		CharacterMotionComponent->OnMovementModeChanged.AddDynamic(this, &ATitanGameplayCharacter::OnMovementModeChanged);
	}

	if (TorsoMesh)
	{
		// force animation tick after movement component updates
		if (TorsoMesh->PrimaryComponentTick.bCanEverTick && GetMoverComponent())
		{
			TorsoMesh->PrimaryComponentTick.AddPrerequisite(GetMoverComponent(), GetMoverComponent()->PrimaryComponentTick);
		}


		if (HeadMesh && HeadwearMesh && LegsMesh)
		{
			// force animation tick after movement component updates
			if (HeadMesh->PrimaryComponentTick.bCanEverTick && GetMoverComponent())
			{
				HeadMesh->PrimaryComponentTick.AddPrerequisite(GetMoverComponent(), GetMoverComponent()->PrimaryComponentTick);
			}

			// force animation tick after movement component updates
			if (HeadwearMesh->PrimaryComponentTick.bCanEverTick && GetMoverComponent())
			{
				HeadwearMesh->PrimaryComponentTick.AddPrerequisite(GetMoverComponent(), GetMoverComponent()->PrimaryComponentTick);
			}

			// force animation tick after movement component updates
			if (LegsMesh->PrimaryComponentTick.bCanEverTick && GetMoverComponent())
			{
				LegsMesh->PrimaryComponentTick.AddPrerequisite(GetMoverComponent(), GetMoverComponent()->PrimaryComponentTick);
			}
		}
	}
}

void ATitanGameplayCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (SamplePlayerState)
	{
		// set the cast PlayerState pointer
		TitanPlayerState = Cast<ATitanGameplayPlayerState>(SamplePlayerState);
	}
	else
	{
		// clear the cast PlayerState pointer
		TitanPlayerState = nullptr;
	}

	// reset the cached input state
	CachedMoveInputIntent = FVector::ZeroVector;
	CachedTurnInput = FRotator::ZeroRotator;
	CachedLookInput = FRotator::ZeroRotator;
	bWantsToJump = false;
	bWantsToSprint = false;
	bWantsToGlide = false;
	bWantsToAutoWalk = false;

	// initialize the camera pitch limits
	Camera->InitializeCameraForPlayer();
}

void ATitanGameplayCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// spin the camera based on input
	if (SamplePlayerController)
	{
		// apply control inputs
		const float TimeDilation = GetActorTimeDilation();

		if (TimeDilation > 0.0f)
		{
			SamplePlayerController->AddYawInput(CachedLookInput.Yaw * CameraRotationRateYaw * DeltaTime / TimeDilation);
			SamplePlayerController->AddPitchInput(-CachedLookInput.Pitch * CameraRotationRatePitch * DeltaTime / TimeDilation);
		}

		if (CachedMoveInputIntent.Size() > 0.0f)
		{
			if (ShouldAlignCameraOnMovement())
			{
				AlignCameraToFacing(DeltaTime, CameraMovementAlignSpeed * CachedMoveInputIntent.GetClampedToMaxSize(1.0f).Size());
			}
		}

		// calculate the time since our last relevant input
		float TimeSinceLastInput = GetWorld()->GetTimeSeconds() - CameraAutoAlignLastInputTime;

		float AutoAlignTime = bOverrideCameraAutoAlign ? OverrideCameraAutoAlignTime : CameraAutoAlignTime;

		// check if it's time to auto align the camera
		if (ShouldAutoAlignCamera() && TimeSinceLastInput >= AutoAlignTime)
		{
			float AutoAlignSpeed = bOverrideCameraAutoAlign ? OverrideCameraAutoAlignSpeed : CameraAutoAlignSpeed;

			AlignCameraToFacing(DeltaTime, AutoAlignSpeed);
		}
	}

	// align the interaction box to the view rotation
	InteractionPivot->SetWorldRotation(GetViewRotation());

	// call the update stamina delegate
	OnStaminaChanged.Broadcast(GetStaminaPercent(), IsExhausted());
}

void ATitanGameplayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Move
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATitanGameplayCharacter::Move);
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATitanGameplayCharacter::MoveCompleted);
		}

		// Look
		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ATitanGameplayCharacter::Look);
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Completed, this, &ATitanGameplayCharacter::LookCompleted);
		}

		// Jump
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ATitanGameplayCharacter::Jump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ATitanGameplayCharacter::StopJumping);
		}

		// Autorun
		if (AutoWalkAction)
		{
			EnhancedInputComponent->BindAction(AutoWalkAction, ETriggerEvent::Completed, this, &ATitanGameplayCharacter::AutoWalk);
		}
		
		// Camera distance adjust
		if (CameraDistanceAction)
		{
			EnhancedInputComponent->BindAction(CameraDistanceAction, ETriggerEvent::Triggered, this, &ATitanGameplayCharacter::AdjustCameraDistance);
		}

		// Interact
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &ATitanGameplayCharacter::Interact);
		}

	}
}

void ATitanGameplayCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// set up the input vector. We flip the axis so they correspond with the expected Mover input
	CachedMoveInputIntent.X = FMath::Clamp(MovementVector.Y, -1.0f, 1.0f);
	CachedMoveInputIntent.Y = FMath::Clamp(MovementVector.X, -1.0f, 1.0f);

	// cancel autorun if the input intent is nonzero
	if (!CachedMoveInputIntent.IsNearlyZero())
	{
		bWantsToAutoWalk = false;
	}

	// broadcast the delegate
	OnMoved.Broadcast(MovementVector);

	// update the camera auto align timeout
	UpdateCameraAutoAlignInputTime();
}

void ATitanGameplayCharacter::MoveCompleted(const FInputActionValue& Value)
{
	// zero out the cached input
	CachedMoveInputIntent = FVector::ZeroVector;

	// broadcast the delegate
	OnMoved.Broadcast(FVector2D::ZeroVector);
}

void ATitanGameplayCharacter::Look(const FInputActionValue& Value)
{
	// check if we should ignore the input
	if (!bCameraInputDisabled)
	{
		// input is a Vector2D
		FVector2D LookAxisVector = Value.Get<FVector2D>();

		// set up the look input rotator
		CachedLookInput.Yaw = CachedTurnInput.Yaw = FMath::Clamp(LookAxisVector.X, -1.0f, 1.0f);
		CachedLookInput.Pitch = CachedTurnInput.Pitch = FMath::Clamp(LookAxisVector.Y, -1.0f, 1.0f);
	}
	else
	{
		// assume 0 input if camera input is disabled
		CachedLookInput.Yaw = CachedTurnInput.Yaw = 0.0f;
		CachedLookInput.Pitch = CachedTurnInput.Pitch = 0.0f;
	}

	// update the camera auto align timeout
	UpdateCameraAutoAlignInputTime();
}

void ATitanGameplayCharacter::LookCompleted(const FInputActionValue& Value)
{
	// zero out the cached input
	CachedLookInput = FRotator::ZeroRotator;
}

void ATitanGameplayCharacter::Jump()
{
	// is this the first frame we want to jump?
	bWantsToJump = !bIsJumpPressed;

	// update the flag
	bIsJumpPressed = true;

	// broadcast the delegate
	OnJumped.Broadcast(true);

	// update the camera auto align timeout
	UpdateCameraAutoAlignInputTime();
}

void ATitanGameplayCharacter::StopJumping()
{
	// reset the flag
	bIsJumpPressed = bWantsToJump = false;

	// broadcast the delegate
	OnJumped.Broadcast(false);
}

void ATitanGameplayCharacter::AutoWalk()
{
	// toggle autorun
	bWantsToAutoWalk = !bWantsToAutoWalk;
}

void ATitanGameplayCharacter::AdjustCameraDistance(const FInputActionValue& Value)
{
	Camera->AdjustArmLengthMultiplier(Value.Get<float>());
}

void ATitanGameplayCharacter::Interact()
{
	// tell the interaction box to request an interaction if there's a valid target
	InteractionBox->RequestInteraction();
}

void ATitanGameplayCharacter::AddWind(const FVector& Wind)
{
	WindVelocity += Wind;
}

void ATitanGameplayCharacter::Sprint()
{
	// is this the first frame we want to sprint?
	bWantsToSprint = !bIsSprintPressed;

	// update the flag
	bIsSprintPressed = true;
}

void ATitanGameplayCharacter::StopSprinting()
{
	// reset the flag
	bIsSprintPressed = bWantsToSprint = false;
}

void ATitanGameplayCharacter::Glide()
{
	// is this the first frame we want to glide?
	bWantsToGlide = !bIsGlidePressed;

	// update the flag
	bIsGlidePressed = true;
}

void ATitanGameplayCharacter::StopGliding()
{
	// reset the flag
	bIsGlidePressed = bWantsToGlide = false;
}

void ATitanGameplayCharacter::Aim()
{
	// set the flag
	bIsAimPressed = true;
}

void ATitanGameplayCharacter::StopAiming()
{
	// reset the flag
	bIsAimPressed = false;
}

UPrimitiveComponent* ATitanGameplayCharacter::GetMovementBase() const
{
	return CharacterMotionComponent ? CharacterMotionComponent->GetMovementBase() : nullptr;
}

void ATitanGameplayCharacter::RequestMoveByIntent(const FVector& DesiredIntent)
{
	CachedMoveInputIntent = DesiredIntent;
}

void ATitanGameplayCharacter::RequestMoveByVelocity(const FVector& DesiredVelocity)
{
	// left as stub on purpose. We're not an AI so we move by intent, not velocity
}

float ATitanGameplayCharacter::GetStamina() const
{
	// get the mover component
	if (UMoverComponent* MoverComp = GetMoverComponent())
	{
		// get the sync state
		if (const FTitanStaminaSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanStaminaSyncState>())
		{
			// return the current stamina value
			return SyncState->GetStamina();
		}
	}

	return 0.0f;
}

float ATitanGameplayCharacter::GetStaminaPercent() const
{
	// get the mover component
	if (UMoverComponent* MoverComp = GetMoverComponent())
	{
		// get the sync state
		if (const FTitanStaminaSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanStaminaSyncState>())
		{
			// catch invalid max stamina
			if (SyncState->GetMaxStamina() == 0.0)
			{
				return 0.0f;
			}

			// return the stamina ratio
			return SyncState->GetStamina() / SyncState->GetMaxStamina();
			
		}
	}

	return 0.0f;
}

bool ATitanGameplayCharacter::IsExhausted() const
{
	// get the mover component
	if (UMoverComponent* MoverComp = GetMoverComponent())
	{
		// get the sync state
		if (const FTitanStaminaSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanStaminaSyncState>())
		{

			// return the stamina ratio
			return SyncState->IsExhausted();
		}		
	}

	return false;
}

bool ATitanGameplayCharacter::HasMovementTagExact(const FGameplayTag& Tag)
{
	// get the mover component
	if (UMoverComponent* MoverComp = GetMoverComponent())
	{
		// get the sync state
		if (const FTitanTagsSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanTagsSyncState>())
		{
			// return the tag match
			return SyncState->HasTagExact(Tag);
		}
	}

	return false;
}

bool ATitanGameplayCharacter::HasMovementTagAny(const FGameplayTag& Tag)
{
	// get the mover component
	if (UMoverComponent* MoverComp = GetMoverComponent())
	{
		// get the sync state
		if (const FTitanTagsSyncState* SyncState = MoverComp->GetSyncState().SyncStateCollection.FindDataByType<FTitanTagsSyncState>())
		{
			// return the tag match
			return SyncState->HasTagAny(Tag);
		}
	}

	return false;
}

void ATitanGameplayCharacter::QueueTeleportMove(const FVector& TeleportLocation)
{
	if (bTeleportQueued)
	{
		UE_LOG(LogTitan, Warning, TEXT("Teleport already queued."));

		return;
	}

	QueuedTeleportLocation = TeleportLocation;

	FTransform PreloaderTransform(FRotator::ZeroRotator, QueuedTeleportLocation, FVector::OneVector);

	ATitanTeleportPreloader* Preloader = GetWorld()->SpawnActor<ATitanTeleportPreloader>(ATitanTeleportPreloader::StaticClass(), PreloaderTransform);

	if (Preloader)
	{
		bTeleportQueued = true;
		bCompletedPreloadingTeleport = bCompletedTeleportAnimation = false;

		GetMoverComponent()->WaitForTeleport();

		Preloader->OnPreloadComplete.BindDynamic(this, &ATitanGameplayCharacter::OnTeleportPreloadReady);

		if (UAnimInstance* AnimInstance = TorsoMesh->GetAnimInstance())
		{
			if (TeleportMontage)
			{
				AnimInstance->Montage_Play(TeleportMontage, 1.0f, EMontagePlayReturnType::Duration, 0.0f, true);
				AnimInstance->Montage_SetNextSection(TeleportLoopSection, TeleportLoopSection, TeleportMontage);

				UE_LOG(LogTitan, Error, TEXT("Teleport Queued"));
			}
		}

	}
	else
	{
		UE_LOG(LogTitan, Error, TEXT("Could not spawn teleport preloader"));
	}
}

bool ATitanGameplayCharacter::IsTeleporting() const
{
	return bTeleportQueued;
}

void ATitanGameplayCharacter::OnTeleportAnimationReady()
{
	if (bTeleportQueued)
	{
		UE_LOG(LogTitan, Error, TEXT("Teleport Animation Ready"));

		bCompletedTeleportAnimation = true;

		FinalizeTeleport();
	}	
}

void ATitanGameplayCharacter::CheckTeleportReady()
{
	if (bTeleportQueued && bCompletedPreloadingTeleport)
	{
		if (TeleportMontage)
		{
			if (UAnimInstance* AnimInstance = TorsoMesh->GetAnimInstance())
			{
				UE_LOG(LogTitan, Error, TEXT("Exiting Teleport Loop"));

				AnimInstance->Montage_SetNextSection(TeleportLoopSection, TeleportEndSection, TeleportMontage);
			}
		}
	}	
}

void ATitanGameplayCharacter::SetMovementDisabled(bool bNewDisabledState)
{
	// cache the previous state
	const bool bLastState = GetMoverComponent()->IsMovementDisabled();

	// is there a change?
	if (bLastState != bNewDisabledState)
	{
		// set the mover comp to the new value
		GetMoverComponent()->SetMovementDisabled(bNewDisabledState);
		
		// call the delegate
		OnMovementDisabledStateChanged.Broadcast(bNewDisabledState);
	}
}

void ATitanGameplayCharacter::SetCameraDisabled(bool bNewDisabledState)
{
	bCameraInputDisabled = bNewDisabledState;
}

void ATitanGameplayCharacter::OnTeleportPreloadReady()
{
	UE_LOG(LogTitan, Error, TEXT("Teleport Preload Ready"));

	bCompletedPreloadingTeleport = true;

	FinalizeTeleport();
}

void ATitanGameplayCharacter::FinalizeTeleport()
{
	if (bTeleportQueued && bCompletedPreloadingTeleport && bCompletedTeleportAnimation)
	{
		UE_LOG(LogTitan, Error, TEXT("Finalizing Teleport"));

		bTeleportQueued = false;

		GetMoverComponent()->TeleportAndFall(QueuedTeleportLocation);
	}
	
}

void ATitanGameplayCharacter::OnMovementModeChanged(const FName& PreviousModeName, const FName& NewModeName)
{
	FGameplayEventData Payload;

	// pass the gameplay event to the ability system so abilities can react to the mode change
	if (SamplePlayerState)
	{
		SamplePlayerState->GetAbilitySystemComponent()->HandleGameplayEvent(TAG_Titan_Character_MovementModeChanged, &Payload);
	}

}

void ATitanGameplayCharacter::ProduceInput_Implementation(int32 SimTimeMs, FMoverInputCmdContext& InputCmdResult)
{
	float DeltaMs = (float)SimTimeMs;

	FCharacterDefaultInputs& DefaultKinematicInputs = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FCharacterDefaultInputs>();
	FTitanMovementInputs& TitanInputs = InputCmdResult.InputCollection.FindOrAddMutableDataByType<FTitanMovementInputs>();

	static const FCharacterDefaultInputs DoNothingInput;

	// do we have a controller?
	if (Controller == nullptr)
	{
		// are we the authority?
		if (GetLocalRole() == ENetRole::ROLE_Authority && GetRemoteRole() == ENetRole::ROLE_SimulatedProxy)
		{
			// no controller, so pass a do nothing input
			DefaultKinematicInputs = DoNothingInput;
		}

		// no need to run input code without a controller
		return;
	}

	// is movement disabled ?
	if (GetMoverComponent()->IsMovementDisabled())
	{
		// pass a do nothing input
		DefaultKinematicInputs = DoNothingInput;
	}

	DefaultKinematicInputs.ControlRotation = FRotator::ZeroRotator;

	// copy the control rotation
	if (SamplePlayerController)
	{
		DefaultKinematicInputs.ControlRotation = SamplePlayerController->GetControlRotation();
	}

	// force the forward input intent if autowalk is on
	FVector MoveInputIntent = CachedMoveInputIntent;

	if (bWantsToAutoWalk)
	{
		MoveInputIntent.X = 1.0f;
	}

	// use only the control rotation yaw to avoid tapering our inputs if looking at the character from a too low or too high angle
	FRotator ControlFacing = FRotator::ZeroRotator;
	ControlFacing.Yaw = DefaultKinematicInputs.ControlRotation.Yaw;

	// set the move input
	DefaultKinematicInputs.SetMoveInput(EMoveInputType::DirectionalIntent, ControlFacing.RotateVector(MoveInputIntent));

	// check if we have a nonzero input
	static float RotationMagMin(1e-3f);
	const bool bHasAffirmativeMoveInput = (DefaultKinematicInputs.GetMoveInput().Size() >= RotationMagMin);

	// Figure out the orientation intent for the character
	DefaultKinematicInputs.OrientationIntent = FVector::ZeroVector;

	if (bIsAimPressed)
	{
		// set the orientation intent to the camera forward vector
		DefaultKinematicInputs.OrientationIntent = Camera->GetViewRotation().RotateVector(FVector::ForwardVector);

		// save the last nonzero input
		LastAffirmativeMoveInput = DefaultKinematicInputs.OrientationIntent;
	}
	// do we have an affirmative input intent?
	else if (bHasAffirmativeMoveInput)
	{
		// set the orientation intent to the movement direction
		DefaultKinematicInputs.OrientationIntent = DefaultKinematicInputs.GetMoveInput();

		// save the last nonzero input
		LastAffirmativeMoveInput = DefaultKinematicInputs.OrientationIntent;

	}
	else
	{
		// no input intent, so keep the last orientation from input
		DefaultKinematicInputs.OrientationIntent = LastAffirmativeMoveInput;
	}

	// cancel out any z intent to keep the actor vertical
	DefaultKinematicInputs.OrientationIntent = DefaultKinematicInputs.OrientationIntent.GetSafeNormal2D();

	// set the jump inputs
	DefaultKinematicInputs.bIsJumpPressed = bIsJumpPressed;
	DefaultKinematicInputs.bIsJumpJustPressed = bWantsToJump;

	// set the sprint inputs
	TitanInputs.bIsSprintPressed = bIsSprintPressed;
	TitanInputs.bIsSprintJustPressed = bWantsToSprint;

	// set the glide inputs
	TitanInputs.bIsGlidePressed = bIsGlidePressed;
	TitanInputs.bIsGlideJustPressed = bWantsToGlide;

	// set the wind velocity
	TitanInputs.Wind = WindVelocity;
	
	// Convert inputs to be relative to the current movement base (depending on options and state)
	DefaultKinematicInputs.bUsingMovementBase = false;
	
	// get the Mover component
	if (const UMoverComponent* MoverComp = GetMoverComponent())
	{
		// do we have a base?
		if (UPrimitiveComponent* MovementBase = MoverComp->GetMovementBase())
		{
			FName MovementBaseBoneName = MoverComp->GetMovementBaseBoneName();

			FVector RelativeMoveInput, RelativeOrientDir;


			UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, DefaultKinematicInputs.GetMoveInput(), RelativeMoveInput);
			UBasedMovementUtils::TransformWorldDirectionToBased(MovementBase, MovementBaseBoneName, DefaultKinematicInputs.OrientationIntent, RelativeOrientDir);

			DefaultKinematicInputs.SetMoveInput(DefaultKinematicInputs.GetMoveInputType(), RelativeMoveInput);
			DefaultKinematicInputs.OrientationIntent = RelativeOrientDir;

			DefaultKinematicInputs.bUsingMovementBase = true;
			DefaultKinematicInputs.MovementBase = MovementBase;
			DefaultKinematicInputs.MovementBaseBoneName = MovementBaseBoneName;
		}
	}

	// Clear/consume the inputs
	bWantsToJump = false;
	bWantsToGlide = false;
	bWantsToSprint = false;
}

void ATitanGameplayCharacter::AlignCameraToFacing(float DeltaTime, float AlignSpeed)
{
	// get the camera facing vector
	FVector CameraFacing = Camera->GetViewRotation().RotateVector(FVector::ForwardVector);
	CameraFacing = CameraFacing.GetSafeNormal2D();

	// dot product with our right vector to get the yaw input strength
	float FacingDot = -FVector::DotProduct(CameraFacing, GetActorRightVector());

	// rotate the camera facing through a controller yaw input
	if (SamplePlayerController)
	{
		SamplePlayerController->AddYawInput(FacingDot * AlignSpeed * DeltaTime);
	}

}

bool ATitanGameplayCharacter::ShouldAutoAlignCamera()
{
	// skip auto alignment if we're aiming the grapple
	return !bIsAimPressed || bOverrideCameraAutoAlign;
}

void ATitanGameplayCharacter::UpdateCameraAutoAlignInputTime()
{
	CameraAutoAlignLastInputTime = GetWorld()->GetTimeSeconds();
}

void ATitanGameplayCharacter::SetCameraAutoAlignState(bool bEnable, float AutoAlignTime, float AutoAlignSpeed)
{
	bOverrideCameraAutoAlign = bEnable;

	OverrideCameraAutoAlignTime = AutoAlignTime;
	OverrideCameraAutoAlignSpeed = AutoAlignSpeed;
}

bool ATitanGameplayCharacter::ShouldAlignCameraOnMovement()
{
	return (bAlignCameraOnMovement && !bIsAimPressed);
}

const ATitanRaft* ATitanGameplayCharacter::GetRaft() const
{
	return CurrentRaft;
}

void ATitanGameplayCharacter::InitializeRaft(ATitanRaft* PilotedRaft)
{
	// ensure the raft is valid
	check(PilotedRaft);

	// save a reference to the raft
	CurrentRaft = PilotedRaft;

	// disable collision
	SetActorEnableCollision(false);

	// subscribe to the input delegates
	OnMoved.AddDynamic(PilotedRaft, &ATitanRaft::OnMoveInput);
	OnJumped.AddDynamic(PilotedRaft, &ATitanRaft::OnJumpInput);
	OnMovementDisabledStateChanged.AddDynamic(PilotedRaft, &ATitanRaft::OnMovementDisabledStateChanged);

	// queue an instant move to attach the pilot to the raft
	TSharedPtr<FTitanRaftTeleportEffect> RaftTeleport = MakeShared<FTitanRaftTeleportEffect>();
	RaftTeleport->Raft = PilotedRaft;

	GetMoverComponent()->QueueInstantMovementEffect(RaftTeleport);

	const UTitanMovementSettings* TitanSettings = GetMoverComponent()->FindSharedSettings<UTitanMovementSettings>();
	check(TitanSettings);

	GetMoverComponent()->QueueNextMode(TitanSettings->RaftMovementModeName, false);

	// add ourselves as a tick prereq for the camera
	GetCamera()->PrimaryComponentTick.AddPrerequisite(PilotedRaft, PilotedRaft->PostPhysicsTickFunction);

	// check if the pawn started out with movement disabled
	PilotedRaft->SetInitialMovementDisabledState(GetMoverComponent()->IsMovementDisabled());
}

void ATitanGameplayCharacter::DeInitRaft(ATitanRaft* PilotedRaft, bool bDismount)
{
	check(PilotedRaft);

	if (bDismount)
	{
		// change the movement mode on the pilot
		GetMoverComponent()->QueueNextMode(PilotedRaft->GetDismountMovementMode());

		// queue a jump layered move to dismount the raft
		TSharedPtr<FTitanLayeredMove_Jump> JumpMove = MakeShared<FTitanLayeredMove_Jump>();
		JumpMove->UpwardsSpeed = PilotedRaft->GetDismountImpulse();
		JumpMove->Momentum = PilotedRaft->GetDismountMomentum();

		GetMoverComponent()->QueueLayeredMove(JumpMove);
	}

	// unsubscribe from the pilot's delegates
	OnJumped.RemoveDynamic(PilotedRaft, &ATitanRaft::OnJumpInput);
	OnMoved.RemoveDynamic(PilotedRaft, &ATitanRaft::OnMoveInput);
	OnMovementDisabledStateChanged.RemoveDynamic(PilotedRaft, &ATitanRaft::OnMovementDisabledStateChanged);

	// enable collision
	SetActorEnableCollision(true);

	// remove ourselves as a tick prereq for the camera
	GetCamera()->PrimaryComponentTick.RemovePrerequisite(PilotedRaft, PilotedRaft->PostPhysicsTickFunction);

	// clear the reference to the raft
	CurrentRaft = nullptr;
}

void ATitanGameplayCharacter::RaftPostPhysicsTick(float DeltaTime, ATitanRaft* PilotedRaft)
{
	// once the raft has updated, we tell Mover to teleport the Pawn to the pilot's spot
	// this prevents sync lag due to the raft and the Pawn updating on different threads and frequencies
	GetMoverComponent()->TeleportImmediately(PilotedRaft->GetPilotLocation(), PilotedRaft->GetPilotRotation(), PilotedRaft->GetPilotVelocity());
}

FVector ATitanGameplayCharacter::GetPilotVelocity() const
{
	// pass the bounding capsule's velocity
	return GetRootComponent()->GetComponentVelocity();
}

FRotator ATitanGameplayCharacter::GetPilotControlRotation() const
{
	// pass back the pawn's control rotation
	return GetControlRotation();
}

void ATitanGameplayCharacter::SetWaterPlaneHeight(float Height, bool bEnable)
{
	// if we're on water, enable camera vertical min bounds so the camera stays above the water surface
	GetCamera()->SetCameraBoundsMinZ(bEnable, Height);
}

void ATitanGameplayCharacter::AlignCameraToVector(const FVector& InFacing, float DeltaTime, float AlignSpeed)
{
	// get the camera right vector
	FVector CameraRight = Camera->GetViewRotation().RotateVector(FVector::RightVector);
	CameraRight = CameraRight.GetSafeNormal2D();

	// dot product with our right vector to get the yaw input strength
	float FacingDot = FVector::DotProduct(CameraRight, InFacing);

	// rotate the camera facing through a controller yaw input
	if (SamplePlayerController)
	{
		SamplePlayerController->AddYawInput(FacingDot * AlignSpeed * DeltaTime);
	}
}

FTransform ATitanGameplayCharacter::GetGliderLeftHandTransform_Implementation() const
{
	// stub, override from BP
	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetGliderRightHandTransform_Implementation() const
{
	// stub, override from BP
	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetGliderPelvisTransform_Implementation() const
{
	// stub, override from BP
	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetGliderFeetTransform_Implementation() const
{
	// stub, override from BP
	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetRaftLeftHandTransform_Implementation() const
{
	if (CurrentRaft)
	{
		return CurrentRaft->GetLeftHandTransform();
	}

	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetRaftRightHandTransform_Implementation() const
{
	if (CurrentRaft)
	{
		return CurrentRaft->GetRightHandTransform();
	}

	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetRaftLeftFootTransform_Implementation() const
{
	if (CurrentRaft)
	{
		return CurrentRaft->GetLeftFootTransform();
	}

	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetRaftRightFootTransform_Implementation() const
{
	if (CurrentRaft)
	{
		return CurrentRaft->GetRightFootTransform();
	}

	return FTransform::Identity;
}

FTransform ATitanGameplayCharacter::GetRaftPelvisTransform_Implementation() const
{
	if (CurrentRaft)
	{
		return CurrentRaft->GetPelvisTransform();
	}

	return FTransform::Identity;
}

UInventoryManagerComponent* ATitanGameplayCharacter::GetInventoryManagerComponent() const
{
	// reroute to the PlayerState's inventory component
	if (ATitanGameplayPlayerState* PS = Cast<ATitanGameplayPlayerState>(GetPlayerState()))
	{
		return PS->GetInventory();
	}

	return nullptr;
}

UQuestManagerComponent* ATitanGameplayCharacter::GetQuestManagerComponent() const
{
	// reroute to the PlayerState's quest component
	if (ATitanGameplayPlayerState* PS = Cast<ATitanGameplayPlayerState>(GetPlayerState()))
	{
		return PS->GetQuestManager();
	}

	return nullptr;
}
