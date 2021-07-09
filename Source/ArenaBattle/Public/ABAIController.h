#pragma once

#include "ArenaBattle.h"
#include "AIController.h"
#include "ABAIController.generated.h"

UCLASS()
class ARENABATTLE_API AABAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AABAIController();
	virtual void OnPossess(APawn* InPawn) override;

	static const FName Key_HomePos;
	static const FName Key_PatrolPos;
	static const FName Key_Target;

	void RunAI();
	void StopAI();

private:
	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAsset;
};
