// Copyright Epic Games, Inc. All Rights Reserved.
#include "MyProject6PlayerController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MyProject6Character.h"
#include "Pared.h"
#include "Engine/World.h"

AMyProject6PlayerController::AMyProject6PlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void AMyProject6PlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
	if (bMoveToMouseCursor)
	{
		MoveToMouseCursor();
	}
}

void AMyProject6PlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("SetDestination", IE_Pressed, this, &AMyProject6PlayerController::OnSetDestinationPressed);
	InputComponent->BindAction("SetDestination", IE_Released, this, &AMyProject6PlayerController::OnSetDestinationReleased);

	// support touch devices 
	InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AMyProject6PlayerController::MoveToTouchLocation);
	InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AMyProject6PlayerController::MoveToTouchLocation);

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &AMyProject6PlayerController::OnResetVR);
	InputComponent->BindAction("CambiarMesh", IE_Pressed, this, &AMyProject6PlayerController::OnCambiarMalla);
	InputComponent->BindAction("CambiarMaterial", IE_Pressed, this, &AMyProject6PlayerController::OnCambiarMaterial);
	InputComponent->BindAction("DejarBomba", IE_Pressed, this, &AMyProject6PlayerController::OnDejarBomba);


}
void AMyProject6PlayerController::OnCambiarMalla() {
	for (AActor* Actor : ActoresRegistrados) {
		APared* MiActor = Cast<APared>(Actor);
		if (MiActor) {
			MiActor->CambiarMesh();
		}
	}
}
void AMyProject6PlayerController::OnCambiarMaterial() {
	for (AActor* Actor : ActoresRegistrados) {
		APared* MiActor = Cast<APared>(Actor);
		if (MiActor) {
			MiActor->CambiarMaterial();
		}
	}
}
void AMyProject6PlayerController::OnDejarBomba()
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	// Cast al personaje si es necesario
	AMyProject6Character* PlayerCharacter = Cast<AMyProject6Character>(ControlledPawn);
	if (PlayerCharacter)
	{
		PlayerCharacter->PlaceBomb();
	}
}
void AMyProject6PlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AMyProject6PlayerController::MoveToMouseCursor()
{
	if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
	{
		if (AMyProject6Character* MyPawn = Cast<AMyProject6Character>(GetPawn()))
		{
			if (MyPawn->GetCursorToWorld())
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, MyPawn->GetCursorToWorld()->GetComponentLocation());
			}
		}
	}
	else
	{
		// Trace to see what is under the mouse cursor
		FHitResult Hit;
		GetHitResultUnderCursor(ECC_Visibility, false, Hit);

		if (Hit.bBlockingHit)
		{
			// We hit something, move there
			SetNewMoveDestination(Hit.ImpactPoint);
		}
	}
}

void AMyProject6PlayerController::MoveToTouchLocation(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	FVector2D ScreenSpaceLocation(Location);

	// Trace to see what is under the touch location
	FHitResult HitResult;
	GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
	if (HitResult.bBlockingHit)
	{
		// We hit something, move there
		SetNewMoveDestination(HitResult.ImpactPoint);
	}
}

void AMyProject6PlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const MyPawn = GetPawn();
	if (MyPawn)
	{
		float const Distance = FVector::Dist(DestLocation, MyPawn->GetActorLocation());

		// We need to issue move command only if far enough in order for walk animation to play correctly
		if ((Distance > 120.0f))
		{
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DestLocation);
		}
	}
}

void AMyProject6PlayerController::OnSetDestinationPressed()
{
	// set flag to keep updating destination until released
	bMoveToMouseCursor = true;
}

void AMyProject6PlayerController::OnSetDestinationReleased()
{
	// clear flag to indicate we should stop updating the destination
	bMoveToMouseCursor = false;
}
