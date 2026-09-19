// MyRPGDemo


#include "Controllers/FrontendPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"

void AFrontendPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,ACameraActor::StaticClass(),FName("Default"),CameraActors);

	if (!CameraActors.IsEmpty())
	{
		SetViewTarget(CameraActors[0]);
	}
}
