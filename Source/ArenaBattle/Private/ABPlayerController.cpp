#include "ABPlayerController.h"
#include "ABHUDWidget.h"
#include "ABPlayerState.h"
#include "ABCharacter.h"

AABPlayerController::AABPlayerController()
{
	static ConstructorHelpers::FClassFinder<UABHUDWidget> UI_HUD_C(TEXT("WidgetBlueprint'/Game/Book/UI/UI_HUD.UI_HUD_C'"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
	}
}

void AABPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AABPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

UABHUDWidget* AABPlayerController::GetHUDWidget() const
{
	return HUDWidget;
}

void AABPlayerController::NPCKill(AABCharacter* KilledNPC) const
{
	ABPlayerState->AddExp(KilledNPC->GetExp());
}

void AABPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	HUDWidget = CreateWidget<UABHUDWidget>(this, HUDWidgetClass);
	HUDWidget->AddToViewport();

	ABPlayerState = Cast<AABPlayerState>(PlayerState);
	if (nullptr == ABPlayerState) return;
	HUDWidget->BindPlayerState(ABPlayerState);
	ABPlayerState->OnPlayerStateChanged.Broadcast();
}
