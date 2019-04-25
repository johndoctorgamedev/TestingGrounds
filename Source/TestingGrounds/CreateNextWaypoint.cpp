// Fill out your copyright notice in the Description page of Project Settings.

#include "CreateNextWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "PatrolRoute.h"
#include "AIController.h"

EBTNodeResult::Type UCreateNextWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) {
	//UE_LOG(LogTemp, Warning, TEXT("CreateNextWaypoint Executed"))
	auto ControlledPawn = OwnerComp.GetAIOwner()->GetPawn();
	auto BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) {
		return EBTNodeResult::Failed;
	}

	// Get patrol points
	auto PatrolComponent = ControlledPawn->FindComponentByClass<UPatrolRoute>();
	if (!ensure(PatrolComponent)) {
		return EBTNodeResult::Failed;
	}

	// set next waypoint
	auto PatrolPoints = PatrolComponent->GetPatrolPoints();
	auto Index = BlackboardComp->GetValueAsInt(IndexKey.SelectedKeyName);
	if (PatrolPoints.Num() == 0) {
		UE_LOG(LogTemp, Warning, TEXT("Guard missing patrol points"))
		return EBTNodeResult::Failed;
	}

	BlackboardComp->SetValueAsObject(WaypointKey.SelectedKeyName, PatrolPoints[Index]);

	//Cycle index
	auto NextIndex = (Index + 1) % PatrolPoints.Num();
	BlackboardComp->SetValueAsInt(IndexKey.SelectedKeyName, NextIndex);

	return EBTNodeResult::Succeeded;
}
