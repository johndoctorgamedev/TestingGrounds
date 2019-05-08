// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

class UActorPool;

UCLASS()
class TESTINGGROUNDS_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Spawns a random prop */
	UFUNCTION(BlueprintCallable, Category = "Setup")
		void PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn = 1, int MaxSpawn = 1, float Radius = 500.f, float MinScale = 1, float MaxScale = 1);


	UFUNCTION(BlueprintCallable, Category = "Pool")
	void SetPool(UActorPool* InPool);

	


private:
	/** Checks if there is an object in a sphere in a given location */
	bool CanSpawnAtLocation(FVector Location, float Radius);

	/** Returns a valid spawn point for props */
	bool FindEmptyLocation(FVector &EmptyLocation, float Radius);

	/** Places an actor in a given location */
	void PlaceActor(TSubclassOf<AActor> ToSpawn, FVector SpawnPoint, float Rotation, float Scale);

	void PositionNavMeshBoundsVolume();

	UActorPool* Pool;
};
