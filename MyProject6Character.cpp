// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyProject6Character.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "BombActor.h"
#include "CoinActor.h"
#include "BombManager.h"

AMyProject6Character::AMyProject6Character()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Create a decal in the world to show the cursor's location
	CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());

	// Activate ticking in order to update the cursor every frame.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialAsset(TEXT("Material'/Game/StarterContent/Materials/M_Concrete_Grime'"));
	if (MaterialAsset.Succeeded())
	{
		//MeshComponent->SetMaterial(0, MaterialAsset.Object);  // Set the material to the first material slot
		CharacterMaterial = MaterialAsset.Object;
	}
}
void AMyProject6Character::PlaceBomb()
{
	/*FVector SpawnLocation = GetActorLocation() - GetActorForwardVector() * 200.0f;
	FRotator SpawnRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	ABombActor* NewBomb = GetWorld()->SpawnActor<ABombActor>(ABombActor::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

	if (NewBomb)
	{
		// Add to the array of spawned bombs
		SpawnedBombs.Add(NewBomb);
		// Optionally set a timer to destroy the bomb after a delay
		//NewBomb->SetLifeSpan(5.0f);
	}



	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Bomb spawned behind player."));*/

	UBombManager* BombManager = UBombManager::GetInstance(GetWorld());
	if (BombManager) {
		FVector BombLocation = GetActorLocation();
		BombManager->SpawnBomb(BombLocation);
	}
}
void AMyProject6Character::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}
}
void AMyProject6Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("PlaceBomb", IE_Pressed, this, &AMyProject6Character::PlaceBomb);
	PlayerInputComponent->BindAction("ChangeBombsMaterial", IE_Pressed, this, &AMyProject6Character::ChangeBombMaterials);

}

void AMyProject6Character::ChangeBombMaterials()
{
	if (CharacterMaterial)
	{
		// Loop through all spawned bombs and change their material
		for (ABombActor* Bomb : SpawnedBombs)
		{
			if (Bomb && Bomb->StaticMeshComponent)
			{
				Bomb->StaticMeshComponent->SetMaterial(0, CharacterMaterial);
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Character material is not assigned!"));
	}
}
void AMyProject6Character::CollectCoin(ACoinActor *coin) {
	UE_LOG(LogTemp, Warning, TEXT("Coin was recollected from the player class"));
	if (MonedasUnicasRecolectadas.Contains(coin->CoinType)) {
		
	}
}
