// MyRPGDemo

#pragma once

#include "CoreMinimal.h"
#include "LatentActions.h"   // FPendingLatentAction / FLatentResponse 定义在这里（不在 LatentActionManager.h）
#include "WarriorEnumTypes.h"

class FWarriorCountDownAction : public FPendingLatentAction
{
public:
	FWarriorCountDownAction (float TotalCountDownTime,float UpdateInterval,float& OutRemainingTime,EWarriorCountDownActionOutput& CountDownOutput,const FLatentActionInfo& LatentInfo)
	:bNeedToCancel(false)
	,TotalCountDownTime(TotalCountDownTime)
	,UpdateInterval(UpdateInterval)
	,OutRemainingTime(OutRemainingTime)
	,CountDownOutput(CountDownOutput)
	,ExecutionFunction(LatentInfo.ExecutionFunction)
	,OutputLink(LatentInfo.Linkage)
	,CallbackTarget(LatentInfo.CallbackTarget)
	,ElapsedInterval(0.f)
	,ElapsedTimeSinceStart(0.f)
	{
		
	}

	virtual void UpdateOperation(FLatentResponse& Response) override;
	void CancelAction();
private:
	bool bNeedToCancel;
	float TotalCountDownTime;
	float UpdateInterval;
	float& OutRemainingTime;
	EWarriorCountDownActionOutput& CountDownOutput;
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;
	float ElapsedInterval;
	float ElapsedTimeSinceStart;
};

inline void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{ 
		CountDownOutput = EWarriorCountDownActionOutput::Cancelled;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}

	if (ElapsedTimeSinceStart >= TotalCountDownTime)
	{
		CountDownOutput = EWarriorCountDownActionOutput::Completed;
		Response.FinishAndTriggerIf(true,ExecutionFunction,OutputLink,CallbackTarget);
		return;
	}

	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();
		OutRemainingTime = TotalCountDownTime - ElapsedTimeSinceStart;
		CountDownOutput = EWarriorCountDownActionOutput::Updated;
		Response.TriggerLink(ExecutionFunction,OutputLink,CallbackTarget);
		ElapsedInterval = 0.f;
	}
}

inline void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}
