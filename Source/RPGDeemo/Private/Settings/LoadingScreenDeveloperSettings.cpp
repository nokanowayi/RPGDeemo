// MyRPGDemo


#include "Settings/LoadingScreenDeveloperSettings.h"
#include "Blueprint/UserWidget.h"

TSubclassOf<UUserWidget> ULoadingScreenDeveloperSettings::GetLoadingScreenWidgetClass() const
{
	return LoadingScreenWidgetClass.LoadSynchronous();
}
