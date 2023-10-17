// Copyright (c) 2023 Damian Nowakowski. All rights reserved.

#include "EnhancedCodeFlow.h"
#include "ECFSubsystem.h"

#include "CodeFlowActions/ECFTicker.h"
#include "CodeFlowActions/ECFTicker_WithHandle.h"
#include "CodeFlowActions/ECFDelay.h"
#include "CodeFlowActions/ECFDelayTicks.h"
#include "CodeFlowActions/ECFWaitAndExecute.h"
#include "CodeFlowActions/ECFWaitAndExecute_WithDeltaTime.h"
#include "CodeFlowActions/ECFWhileTrueExecute.h"
#include "CodeFlowActions/ECFTimeline.h"
#include "CodeFlowActions/ECFCustomTimeline.h"
#include "CodeFlowActions/ECFTimeLock.h"
#include "CodeFlowActions/ECFDoOnce.h"
#include "CodeFlowActions/ECFDoNTimes.h"
#include "CodeFlowActions/ECFDoNoMoreThanXTime.h"

#include "CodeFlowActions/Coroutines/ECFWaitSeconds.h"
#include "CodeFlowActions/Coroutines/ECFWaitTicks.h"
#include "CodeFlowActions/Coroutines/ECFWaitUntil.h"

ECF_PRAGMA_DISABLE_OPTIMIZATION

/*^^^ ECF Flow Control Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

bool FFlow::IsActionRunning(const UObject* WorldContextObject, const FECFHandle& Handle)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		return ECF->HasAction(Handle);
	else
		return false;
}

void FEnhancedCodeFlow::PauseAction(const UObject* WorldContextObject, const FECFHandle& Handle)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->PauseAction(Handle);
}

void FEnhancedCodeFlow::ResumeAction(const UObject* WorldContextObject, const FECFHandle& Handle)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->ResumeAction(Handle);
}

bool FEnhancedCodeFlow::IsActionPaused(const UObject* WorldContextObject, const FECFHandle& Handle, bool& bIsPaused)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		return ECF->IsActionPaused(Handle, bIsPaused);
	else
		return false;
}

void FEnhancedCodeFlow::SetPause(const UObject* WorldContextObject, bool bPaused)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->bIsECFPaused = bPaused;
}

bool FEnhancedCodeFlow::GetPause(const UObject* WorldContextObject)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		return ECF->bIsECFPaused;
	else
		return false;
}

/*^^^ Stop ECF Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

void FFlow::StopAction(const UObject* WorldContextObject, FECFHandle& Handle, bool bComplete/* = false*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveAction(Handle, bComplete);
}

void FEnhancedCodeFlow::StopInstancedAction(const UObject* WorldContextObject, FECFInstanceId InstanceId, bool bComplete /*= false*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveInstancedAction(InstanceId, bComplete);
}

void FFlow::StopAllActions(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveAllActions(bComplete, InOwner);
}

/*^^^ Ticker ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::AddTicker(const UObject* InOwner, TUniqueFunction<void(float)>&& InTickFunc, TUniqueFunction<void(bool)>&& InCallbackFunc/* = nullptr*/, const FECFActionSettings& Settings/* = {}*/)
{
	return FFlow::AddTicker(InOwner, -1.f, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc), Settings);
}

FECFHandle FFlow::AddTicker(const UObject* InOwner, float InTickingTime, TUniqueFunction<void(float)>&& InTickFunc, TUniqueFunction<void(bool)>&& InCallbackFunc/* = nullptr*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFTicker>(InOwner, Settings, FECFInstanceId(), InTickingTime, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc));
	else
		return FECFHandle();
}

FECFHandle FFlow::AddTicker(const UObject* InOwner, TUniqueFunction<void(float, FECFHandle)>&& InTickFunc, TUniqueFunction<void(bool)>&& InCallbackFunc/* = nullptr*/, const FECFActionSettings& Settings/* = {}*/)
{
	return FFlow::AddTicker(InOwner, -1.f, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc), Settings);
}

FECFHandle FFlow::AddTicker(const UObject* InOwner, float InTickingTime, TUniqueFunction<void(float, FECFHandle)>&& InTickFunc, TUniqueFunction<void(bool)>&& InCallbackFunc/* = nullptr*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFTicker_WithHandle>(InOwner, Settings, FECFInstanceId(), InTickingTime, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc));
	else
		return FECFHandle();
}

void FFlow::RemoveAllTickers(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
	{
		ECF->RemoveActionsOfClass<UECFTicker>(bComplete, InOwner);
		ECF->RemoveActionsOfClass<UECFTicker_WithHandle>(bComplete, InOwner);
	}
}

/*^^^ Delay ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::Delay(const UObject* InOwner, float InDelayTime, TUniqueFunction<void(bool)>&& InCallbackFunc, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFDelay>(InOwner, Settings, FECFInstanceId(), InDelayTime, MoveTemp(InCallbackFunc));
	else
		return FECFHandle();
}

void FFlow::RemoveAllDelays(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFDelay>(bComplete, InOwner);
}

/*^^^ Delay Ticks ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FEnhancedCodeFlow::DelayTicks(const UObject* InOwner, int32 InDelayTicks, TUniqueFunction<void(bool)>&& InCallbackFunc, const FECFActionSettings& Settings)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFDelayTicks>(InOwner, Settings, FECFInstanceId(), InDelayTicks, MoveTemp(InCallbackFunc));
	else
		return FECFHandle();
}

void FEnhancedCodeFlow::RemoveAllDelayTicks(const UObject* WorldContextObject, bool bComplete, UObject* InOwner)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFDelayTicks>(bComplete, InOwner);
}

/*^^^ Wait And Execute ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::WaitAndExecute(const UObject* InOwner, TUniqueFunction<bool()>&& InPredicate, TUniqueFunction<void(bool, bool)>&& InCallbackFunc, float InTimeOut/* = 0.f*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFWaitAndExecute>(InOwner, Settings, FECFInstanceId(), MoveTemp(InPredicate), MoveTemp(InCallbackFunc), InTimeOut);
	else
		return FECFHandle();
}

FECFHandle FFlow::WaitAndExecute(const UObject* InOwner, TUniqueFunction<bool(float)>&& InPredicate, TUniqueFunction<void(bool, bool)>&& InCallbackFunc, float InTimeOut, const FECFActionSettings& Settings)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFWaitAndExecute_WithDeltaTime>(InOwner, Settings, FECFInstanceId(), MoveTemp(InPredicate), MoveTemp(InCallbackFunc), InTimeOut);
	else
		return FECFHandle();
}

void FFlow::RemoveAllWaitAndExecutes(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
	{
		ECF->RemoveActionsOfClass<UECFWaitAndExecute>(bComplete, InOwner);
		ECF->RemoveActionsOfClass<UECFWaitAndExecute_WithDeltaTime>(bComplete, InOwner);
	}
}

/*^^^ While True Execute ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::WhileTrueExecute(const UObject* InOwner, TUniqueFunction<bool()>&& InPredicate, TUniqueFunction<void(float)>&& InTickFunc, TUniqueFunction<void(bool, bool)>&& InCompleteFunc, float InTimeOut/* = 0.f*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFWhileTrueExecute>(InOwner, Settings, FECFInstanceId(), MoveTemp(InPredicate), MoveTemp(InTickFunc), MoveTemp(InCompleteFunc), InTimeOut);
	else
		return FECFHandle();
}

void FFlow::RemoveAllWhileTrueExecutes(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFWhileTrueExecute>(bComplete, InOwner);
}

/*^^^ Timeline ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::AddTimeline(const UObject* InOwner, float InStartValue, float InStopValue, float InTime, TUniqueFunction<void(float, float)>&& InTickFunc, TUniqueFunction<void(float, float, bool)>&& InCallbackFunc/* = nullptr*/, EECFBlendFunc InBlendFunc/* = EECFBlendFunc::ECFBlend_Linear*/, float InBlendExp/* = 0.f*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFTimeline>(InOwner, Settings, FECFInstanceId(), InStartValue, InStopValue, InTime, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc), InBlendFunc, InBlendExp);
	else
		return FECFHandle();
}

void FFlow::RemoveAllTimelines(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFTimeline>(bComplete, InOwner);
}

/*^^^ Custom Timeline ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FFlow::AddCustomTimeline(const UObject* InOwner, UCurveFloat* CurveFloat, TUniqueFunction<void(float, float)>&& InTickFunc, TUniqueFunction<void(float, float, bool)>&& InCallbackFunc/* = nullptr*/, const FECFActionSettings& Settings/* = {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFCustomTimeline>(InOwner, Settings, FECFInstanceId(), CurveFloat, MoveTemp(InTickFunc), MoveTemp(InCallbackFunc));
	else
		return FECFHandle();
}

void FFlow::RemoveAllCustomTimelines(const UObject* WorldContextObject, bool bComplete/* = false*/, UObject* InOwner/* = nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFCustomTimeline>(bComplete, InOwner);
}

/*^^^ Time Lock ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FEnhancedCodeFlow::TimeLock(const UObject* InOwner, float InLockTime, TUniqueFunction<void()>&& InExecFunc, const FECFInstanceId& InstanceId, const FECFActionSettings& Settings /*= {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFTimeLock>(InOwner, Settings, InstanceId, InLockTime, MoveTemp(InExecFunc));
	else
		return FECFHandle();
}

void FEnhancedCodeFlow::RemoveAllTimeLocks(const UObject* WorldContextObject, UObject* InOwner /*= nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFTimeLock>(false, InOwner);
}

/*^^^ Do Once ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FEnhancedCodeFlow::DoOnce(const UObject* InOwner, TUniqueFunction<void()>&& InExecFunc, const FECFInstanceId& InstanceId)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFDoOnce>(InOwner, {}, InstanceId, MoveTemp(InExecFunc));
	else
		return FECFHandle();
}

void FEnhancedCodeFlow::RemoveAllDoOnce(const UObject* WorldContextObject, UObject* InOwner /*= nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFDoOnce>(false, InOwner);
}

/*^^^ Do N Times ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FEnhancedCodeFlow::DoNTimes(const UObject* InOwner, const uint32 InTimes, TUniqueFunction<void(int32)>&& InExecFunc, const FECFInstanceId& InstanceId)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFDoNTimes>(InOwner, {}, InstanceId, InTimes, MoveTemp(InExecFunc));
	else
		return FECFHandle();
}

void FEnhancedCodeFlow::RemoveAllDoNTimes(const UObject* WorldContextObject, UObject* InOwner /*= nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFDoNTimes>(false, InOwner);
}

/*^^^ Do No More Than X Time ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFHandle FEnhancedCodeFlow::DoNoMoreThanXTime(const UObject* InOwner, TUniqueFunction<void()>&& InExecFunc, float InTime, int32 InMaxExecsEnqueue, FECFInstanceId& InstanceId, const FECFActionSettings& Settings /*= {}*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(InOwner))
		return ECF->AddAction<UECFDoNoMoreThanXTime>(InOwner, Settings, InstanceId, MoveTemp(InExecFunc), InTime, InMaxExecsEnqueue);
	else
		return FECFHandle();
}

void FEnhancedCodeFlow::RemoveAllDoNoMoreThanXTimes(const UObject* WorldContextObject, UObject* InOwner /*= nullptr*/)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFDoNoMoreThanXTime>(false, InOwner);
}

/*^^^ Wait Seconds (Coroutine) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFCoroutineTask_WaitSeconds FEnhancedCodeFlow::WaitSeconds(const UObject* InOwner, float InTime, const FECFActionSettings& Settings /*= {}*/)
{
	return FECFCoroutineTask_WaitSeconds(InOwner, Settings, InTime);
}

void FEnhancedCodeFlow::RemoveAllWaitSeconds(const UObject* WorldContextObject, UObject* InOwner)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFWaitSeconds>(false, InOwner);
}

/*^^^ Wait Ticks (Coroutine) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFCoroutineTask_WaitTicks FEnhancedCodeFlow::WaitTicks(const UObject* InOwner, int32 InTicks, const FECFActionSettings& Settings)
{
	return FECFCoroutineTask_WaitTicks(InOwner, Settings, InTicks);
}

void FEnhancedCodeFlow::RemoveAllWaitTicks(const UObject* WorldContextObject, UObject* InOwner)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFWaitTicks>(false, InOwner);
}

/*^^^ Wait Until (Coroutine) ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

FECFCoroutineTask_WaitUntil FEnhancedCodeFlow::WaitUntil(const UObject* InOwner, TUniqueFunction<bool(float)>&& InPredicate, float InTimeOut, const FECFActionSettings& Settings)
{
	return FECFCoroutineTask_WaitUntil(InOwner, Settings, MoveTemp(InPredicate), InTimeOut);
}

void FEnhancedCodeFlow::RemoveAllWaitUntil(const UObject* WorldContextObject, UObject* InOwner)
{
	if (UECFSubsystem* ECF = UECFSubsystem::Get(WorldContextObject))
		ECF->RemoveActionsOfClass<UECFWaitUntil>(false, InOwner);
}

ECF_PRAGMA_ENABLE_OPTIMIZATION
