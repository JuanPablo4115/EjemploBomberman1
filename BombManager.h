// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BombManager.generated.h"

UCLASS()
class MYPROJECT6_API UBombManager : public UObject
{
	GENERATED_BODY()

public:
	static UBombManager* GetInstance(UWorld* World);

	void SpawnBomb(FVector Location);

private:
	static UBombManager* Instance;
};
