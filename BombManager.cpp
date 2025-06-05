// Fill out your copyright notice in the Description page of Project Settings.


#include "BombManager.h"
#include "Engine/World.h"
#include "BombActor.h"
UBombManager* UBombManager::Instance = nullptr;
UBombManager* Instance;

UBombManager* UBombManager::GetInstance(UWorld* World) {
	if (!Instance && World) {
		//Create a new object instance tied to the world 
		Instance = NewObject<UBombManager>(World);
		Instance->AddToRoot(); 
	}
	return Instance;
}
void UBombManager::SpawnBomb(FVector Location) {
	//spawnear bombbas solo en las esquinas del mapa
	if (Instance) {
		UWorld* World = Instance->GetWorld();
		if (World) {
			World->SpawnActor<ABombActor>(Location,FRotator::ZeroRotator);
		}
	}
}