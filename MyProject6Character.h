// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BombActor.h"
#include "CoinActor.h"
#include "MyProject6Character.generated.h"

UCLASS(Blueprintable)
class AMyProject6Character : public ACharacter
{
	GENERATED_BODY()

public:
	AMyProject6Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void CollectCoin(ACoinActor* coin);

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;
	UPROPERTY(EditAnywhere, Category = "Bomba")
	TArray<ABombActor*> SpawnedBombs;
	UPROPERTY(EditAnywhere, Category = "Bomba")
	UMaterial* CharacterMaterial;
public:
	void PlaceBomb();
protected:
	void ChangeBombMaterials();
	TMap<ECoinType, ACoinActor> MonedasUnicasRecolectadas;
};

