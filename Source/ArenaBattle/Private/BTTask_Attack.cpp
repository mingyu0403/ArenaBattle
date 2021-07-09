#include "BTTask_Attack.h"
#include "ABAIController.h"
#include "ABCharacter.h"

UBTTask_Attack::UBTTask_Attack()
{
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AABCharacter* ABCharacter = Cast<AABCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == ABCharacter) return EBTNodeResult::Failed;

	ABCharacter->Attack();
	IsAttacking = true;
	ABCharacter->OnAttackEnd.AddLambda(
		[this]()->void
		{
			IsAttacking = false;
		}
	);

	return EBTNodeResult::InProgress;
}

void UBTTask_Attack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	if (IsAttacking == false)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}
