// MyRPGDemo


#include "Controllers/FrontendPlayerController.h"

#include "Camera/CameraActor.h"
#include "Kismet/GameplayStatics.h"
#include "Settings/FrontendGameUserSettings.h"

void AFrontendPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	TArray<AActor*> CameraActors;
	UGameplayStatics::GetAllActorsOfClassWithTag(this,ACameraActor::StaticClass(),FName("Default"),CameraActors);

	if (!CameraActors.IsEmpty())
	{
		SetViewTarget(CameraActors[0]);
	}
	UFrontendGameUserSettings* FrontendGameUserSettings = UFrontendGameUserSettings::Get();
	if (FrontendGameUserSettings->GetLastCPUBenchmarkResult() == -1.f||FrontendGameUserSettings->GetLastGPUBenchmarkResult() == -1.f)
	{
		FrontendGameUserSettings->RunHardwareBenchmark();
		FrontendGameUserSettings->ApplyHardwareBenchmarkResults();
	}
}
