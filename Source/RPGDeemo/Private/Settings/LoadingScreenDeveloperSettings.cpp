// MyRPGDemo


#include "Blueprint/UserWidget.h"
#include "Settings/LoadingScreenDeveloperSettings.h"

TSubclassOf<UUserWidget> ULoadingScreenDeveloperSettings::GetLoadingScreenWidgetClass() const
{
	return LoadingScreenWidgetClass.LoadSynchronous();
}
