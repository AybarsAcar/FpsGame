// Copyright Aybars Acar


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	// make sure this player controller replicates
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// handle the highlighting that implements from any enemy interface
	CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// add the input mapping context
	// assert that AuraContext is set from the Editor, otherwise, crash
	check(AuraContext);

	auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// access the input component
	auto* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// bind our move input action to enhanced input component pointer we have
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const auto InputAxisVector = InputActionValue.Get<FVector2d>();

	// add movement to our controller pawn
	// figure out which direction is forward for us based on the controller
	const auto Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const auto ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const auto RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// add movement input now to the pawn
	if (auto* ControllerPawn = GetPawn<APawn>())
	{
		ControllerPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControllerPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;

	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (!CursorHit.bBlockingHit) return;

	// see if the object hit implements the IEnemyInterface
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/// Line Trace from cursor. There are several scenarios
	/// a. LastActor is null and ThisActor is null
	///		- we probably hit something not an enemy
	///		- do nothing
	/// b. LastActor is null and ThisActor is valid
	///		- we are hovering over on this actor for the first time
	///		- call Highlight on ThisActor
	///	c. Last Actor is Valid and ThisActor is null
	///		- we hovered over an enemy interface last frame but not hovering over it anymore
	///		- so Unhighlight LastActor
	///	d. Both actors are valid but LastActor != ThisActor
	///		- We were hovering over one enemy and we now hover over another enemy
	///		- Unlightlight LastActor
	///		- Highlight ThisActor
	///	e. Both Actors are valid and LastActor == ThisActor
	///		- We are still over the same enemy
	///		- This means we highlighted it in the frame earlier
	///		- So do nothing

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case b
			ThisActor->HighlightActor();
		}
		else
		{
			// Case a - do nothing
		}
	}

	else // last actor is valid
	{
		if (ThisActor == nullptr)
		{
			// case c
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != ThisActor)
			{
				// case d
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// case e - do nothing
			}
		}
	}
}
