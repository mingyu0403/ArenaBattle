#include "ABSection.h"
#include "ABCharacter.h"
#include "ABItemBox.h"

AABSection::AABSection()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MESH"));
	RootComponent = Mesh;
	
	FString AssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_SQUARE.SM_SQUARE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_SQUARE(*AssetPath);
	if (SM_SQUARE.Succeeded())
	{
		Mesh->SetStaticMesh(SM_SQUARE.Object);
	}

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("TRIGGER"));
	Trigger->SetBoxExtent(FVector(775.0f, 775.0f, 300.0f));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetRelativeLocation(FVector(0.0f, 0.0f, 250.0f));
	Trigger->SetCollisionProfileName(TEXT("ABTrigger"));

	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnTriggerBeginOverlap);

	FString GateAssetPath = TEXT("StaticMesh'/Game/Book/StaticMesh/SM_GATE.SM_GATE'");
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM_GATE(*GateAssetPath);

	static FName GateSockets[] = {
		{TEXT("+XGate")},
		{TEXT("-XGate")},
		{TEXT("+YGate")},
		{TEXT("-YGate")},
	};

	for (FName GateSocket : GateSockets)
	{
		if (Mesh->DoesSocketExist(GateSocket) == false)
		{
			UE_LOG(LogClass, Warning, TEXT("DoesSocketExist"));
			return;
		}

		UStaticMeshComponent* NewGate = CreateDefaultSubobject<UStaticMeshComponent>(*GateSocket.ToString());
		NewGate->SetStaticMesh(SM_GATE.Object);
		NewGate->SetupAttachment(RootComponent, GateSocket);
		NewGate->SetRelativeLocation(FVector(0.0f, -80.5f, 0.0f));
		GateMeshes.Add(NewGate);

		UBoxComponent* NewGateTrigger = CreateDefaultSubobject<UBoxComponent>(*GateSocket.ToString().Append(TEXT("Trigger")));
		NewGateTrigger->SetBoxExtent(FVector(100.0f, 100.0f, 300.0f));
		NewGateTrigger->SetupAttachment(RootComponent, GateSocket);
		NewGateTrigger->SetRelativeLocation(FVector(70.0f, 0.0f, 250.0f));
		NewGateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		GateTriggers.Add(NewGateTrigger);

		NewGateTrigger->OnComponentBeginOverlap.AddDynamic(this, &AABSection::OnGateTriggerBeginOverlap);
		NewGateTrigger->ComponentTags.Add(GateSocket);
	}

	bNoBattle = false;

	EnemySpawnTime = 2.0f;
	ItemBoxSpawnTime = 5.0f;
}

void AABSection::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::BeginPlay()
{
	Super::BeginPlay();
	
	SetState(bNoBattle ? ESectionState::COMPLETE : ESectionState::READY);
}

void AABSection::SetState(ESectionState NewState)
{
	switch (NewState)
	{
	case ESectionState::READY:
	{
		Trigger->SetCollisionProfileName(TEXT("ABTrigger"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(true);
		break;
	}
	case ESectionState::BATTLE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("NoCollision"));
		}
		OperateGates(false);
		GetWorld()->GetTimerManager().SetTimer(SpawnNPCTimerHandle, FTimerDelegate::CreateUObject(this, &AABSection::OnNPCSpawn), EnemySpawnTime, false);
		GetWorld()->GetTimerManager().SetTimer(SpawnItemBoxTimerHandle, FTimerDelegate::CreateLambda(
			[this]()->void
			{
				FVector2D RandXY = FMath::RandPointInCircle(600.0f);
				GetWorld()->SpawnActor<AABItemBox>(GetActorLocation() + FVector(RandXY, 30.0f), FRotator::ZeroRotator);
			}
		), ItemBoxSpawnTime, false);
		break;
	}
	case ESectionState::COMPLETE:
	{
		Trigger->SetCollisionProfileName(TEXT("NoCollision"));
		for (UBoxComponent* GateTrigger : GateTriggers)
		{
			GateTrigger->SetCollisionProfileName(TEXT("ABTrigger"));
		}
		OperateGates(true);
		break;
	}
	}
	CurrentState = NewState;
}

void AABSection::OperateGates(bool bOpen)
{
	for (UStaticMeshComponent* Gate : GateMeshes)
	{
		Gate->SetRelativeRotation(bOpen ? FRotator(0.0f, -90.0f, 0.0f) : FRotator::ZeroRotator);
	}
}

void AABSection::OnTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (CurrentState == ESectionState::READY)
	{
		SetState(ESectionState::BATTLE);
	}
}

void AABSection::OnGateTriggerBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogClass, Warning, TEXT("OnGateTriggerBeginOverlap"));

	if (OverlappedComponent->ComponentTags.Num() != 1)
	{
		return;
	}

	FName ComponentTag = OverlappedComponent->ComponentTags[0];
	FName SocketName = FName(*ComponentTag.ToString().Left(2));
	if (Mesh->DoesSocketExist(SocketName) == false)
	{
		return;
	}
	
	FVector NewLocation = Mesh->GetSocketLocation(SocketName);

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionQueryParam(NAME_None, false, this);
	FCollisionObjectQueryParams ObjectQueryParam(FCollisionObjectQueryParams::InitType::AllObjects);
	bool bResult = GetWorld()->OverlapMultiByObjectType(
		OverlapResults,
		NewLocation,
		FQuat::Identity,
		ObjectQueryParam,
		FCollisionShape::MakeSphere(775.0f),
		CollisionQueryParam
	);

	if (bResult == false)
	{
		UE_LOG(LogClass, Warning, TEXT("Spawn NewSection"));

		AABSection* NewSection = GetWorld()->SpawnActor<AABSection>(NewLocation, FRotator::ZeroRotator);
	}
}

void AABSection::OnNPCSpawn()
{
	GetWorld()->SpawnActor<AABCharacter>(GetActorLocation() + FVector::UpVector * 88.0f, FRotator::ZeroRotator);
}

