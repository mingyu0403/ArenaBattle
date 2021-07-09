// Fill out your copyright notice in the Description page of Project Settings.


#include "ABAnimInstance.h"

UABAnimInstance::UABAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("AnimMontage'/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage'"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
}

void UABAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	APawn* Pawn = TryGetPawnOwner();
	if (!::IsValid(Pawn)) return;

	if(!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		ACharacter* Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
	}
}

void UABAnimInstance::PlayAttackMontage()
{
	if (IsDead) return;

	Montage_Play(AttackMontage, 1.0f);
}

void UABAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	if (IsDead) return;

	if (Montage_IsPlaying(AttackMontage))
	{
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
	}
}

void UABAnimInstance::AnimNotify_AttackHitCheck()
{
	OnAttackHitCheck.Broadcast();
}

void UABAnimInstance::AnimNotify_NextAttackCheck()
{
	OnNextAttackCheck.Broadcast();
}

FName UABAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}
