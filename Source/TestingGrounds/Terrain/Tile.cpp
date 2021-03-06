// Fill out your copyright notice in the Description page of Project Settings.

#include "Tile.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "NavigationSystem.h"
#include "ActorPool.h"

#define MAX_ATTEMPTS 100

// Sets default values
ATile::ATile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
}

void ATile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (Pool != nullptr && NavMeshBoundsVolume != nullptr) {
		Pool->Return(NavMeshBoundsVolume);
	}

	TArray<AActor*> AttachedActors;
	GetAttachedActors(AttachedActors);
	for (AActor* AttachedActor : AttachedActors) 
	{
		AttachedActor->Destroy();
	}

}

void ATile::SetPool(UActorPool* InPool)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s] Setting Pool %s"), *(this->GetName()), *(InPool->GetName()))
	Pool = InPool;
	PositionNavMeshBoundsVolume();
}

void ATile::PositionNavMeshBoundsVolume()
{
	FVector Origin;
	FVector BoundSize;
	NavMeshBoundsVolume = Pool->Checkout();
	if (NavMeshBoundsVolume == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("[%s] Not enough actors in pool"), *(this->GetName()))
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("[%s] Checked out: %s"), *(this->GetName()), *(NavMeshBoundsVolume->GetName()))
	GetActorBounds(true, Origin, BoundSize);
	NavMeshBoundsVolume->SetActorLocation(GetActorLocation() + FVector(BoundSize.X, 0, 0));
	FNavigationSystem::Build(*GetWorld());
}

// Called every frame
void ATile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

template<class T>
void ATile::RandomlyPlaceActors(TSubclassOf<T> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale)
{
	int NumberToSpawn = FMath::RandRange(MinSpawn, MaxSpawn);
	for (size_t i = 0; i < NumberToSpawn; i++) {
		FSpawnPosition SpawnPosition;
		SpawnPosition.Scale = FMath::RandRange(MinScale, MaxScale);
		bool bFound = FindEmptyLocation(SpawnPosition.Location, Radius * SpawnPosition.Scale);
		if (bFound) {
			SpawnPosition.Rotation = FMath::RandRange(-180.f, 180.f);
			PlaceActor(ToSpawn, SpawnPosition);
		}
	}
}

void ATile::PlaceActors(TSubclassOf<AActor> ToSpawn, int MinSpawn, int MaxSpawn, float Radius, float MinScale, float MaxScale) 
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, MinScale, MaxScale);
}

void ATile::PlaceAIPawns(TSubclassOf<APawn> ToSpawn, int MinSpawn, int MaxSpawn, float Radius)
{
	RandomlyPlaceActors(ToSpawn, MinSpawn, MaxSpawn, Radius, 1, 1);
}

/** Returns a valid spawn point for props */
bool ATile::FindEmptyLocation(FVector &EmptyLocation, float Radius) {
	FBox Bounds = GetComponentsBoundingBox(false);
	for (int32_t i = 0; i < MAX_ATTEMPTS; i++) {
		FVector CandidatePoint = FMath::RandPointInBox(Bounds);
		CandidatePoint.Z = 0;
		if (CanSpawnAtLocation(CandidatePoint, Radius)) {
			EmptyLocation = CandidatePoint;
			return true;
		}
	}
	return false;
}

/** Places an actor in a given location */
void ATile::PlaceActor(TSubclassOf<AActor> ToSpawn, FSpawnPosition SpawnPosition) {
	AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ToSpawn);
	if (SpawnedActor)
	{
		SpawnedActor->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		SpawnedActor->SetActorLocation(SpawnPosition.Location);
		SpawnedActor->SetActorRotation(FRotator(0, SpawnPosition.Rotation, 0));
		SpawnedActor->SetActorScale3D(FVector(SpawnPosition.Scale));
	}

}

void ATile::PlaceActor(TSubclassOf<APawn> ToSpawn, FSpawnPosition SpawnPosition)
{
	FRotator Rotation = FRotator(0, SpawnPosition.Rotation, 0);
	APawn* Spawned = GetWorld()->SpawnActor<APawn>(ToSpawn, SpawnPosition.Location, Rotation);
	if (Spawned) 
	{
		Spawned->AttachToActor(this, FAttachmentTransformRules(EAttachmentRule::KeepWorld, false));
		Spawned->SpawnDefaultController();
		Spawned->Tags.Add(FName("Guard"));
	}
}

bool ATile::CanSpawnAtLocation(FVector Location, float Radius) {
	FHitResult HitResult;
	bool HasHit = GetWorld()->SweepSingleByChannel(
		HitResult,
		Location,
		Location,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(Radius));

	FColor ResultColor = HasHit ? FColor::Red : FColor::Green;
	//DrawDebugCapsule(GetWorld(), Location, 0, Radius, FQuat::Identity, ResultColor, true, 100.f);

	return !HasHit;
}
