#pragma once

namespace Debug
{
	static void Print(const FString& String, const FColor& Color = FColor::White, float Duration = 2.0f, int32 InKey = -1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(InKey, Duration, Color, String);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *String);
		}
	}

	static void Print(const FString& String, const float FloatValue,const FColor& Color = FColor::White, float Duration = 4.0f, int32 InKey = -1)
	{
		if (GEngine)
		{
			const FString FinalString = String + FString::Printf(TEXT(" : %f"), FloatValue);

			
			GEngine->AddOnScreenDebugMessage(InKey, Duration, Color, FinalString);

			UE_LOG(LogTemp, Warning, TEXT("%s"), *FinalString);
		}
	}
}

class WarriorDebugHelper
{
public:

	
};
