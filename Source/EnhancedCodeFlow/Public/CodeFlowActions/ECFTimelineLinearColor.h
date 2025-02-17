// Copyright (c) 2025 Damian Nowakowski. All rights reserved.

#pragma once

#include "ECFActionBase.h"
#include "ECFTypes.h"
#include "ECFTimelineLinearColor.generated.h"

ECF_PRAGMA_DISABLE_OPTIMIZATION

UCLASS()
class ENHANCEDCODEFLOW_API UECFTimelineLinearColor: public UECFActionBase
{
	GENERATED_BODY()

	friend class UECFSubsystem;

private:

	FLinearColor GetValue()
	{
		switch (BlendFunc)
		{
		case EECFBlendFunc::ECFBlend_Linear:
			return FMath::Lerp(StartValue, StopValue, CurrentTime / Time);
		case EECFBlendFunc::ECFBlend_Cubic:
			return FMath::CubicInterp(StartValue, FLinearColor::Black, StopValue, FLinearColor::Black, CurrentTime / Time);
		case EECFBlendFunc::ECFBlend_EaseIn:
			return FMath::Lerp(StartValue, StopValue, FMath::Pow(CurrentTime / Time, BlendExp));
		case EECFBlendFunc::ECFBlend_EaseOut:
			return FMath::Lerp(StartValue, StopValue, FMath::Pow(CurrentTime / Time, 1.f / BlendExp));
		case EECFBlendFunc::ECFBlend_EaseInOut:
			return FMath::InterpEaseInOut(StartValue, StopValue, CurrentTime / Time, BlendExp);
		}
		return FLinearColor::Black;
	}

protected:

	TUniqueFunction<void(FLinearColor, float)> TickFunc;
	TUniqueFunction<void(FLinearColor, float, bool)> CallbackFunc;
	TUniqueFunction<void(FLinearColor, float)> CallbackFunc_NoStopped;
	FLinearColor StartValue;
	FLinearColor StopValue;
	float Time;
	EECFBlendFunc BlendFunc;
	float BlendExp;

	float CurrentTime;
	FLinearColor CurrentValue;

	bool Setup(FLinearColor InStartValue, FLinearColor InStopValue, float InTime, TUniqueFunction<void(FLinearColor, float)>&& InTickFunc, TUniqueFunction<void(FLinearColor, float, bool)>&& InCallbackFunc, EECFBlendFunc InBlendFunc, float InBlendExp)
	{
		StartValue = InStartValue;
		StopValue = InStopValue;
		Time = InTime;

		TickFunc = MoveTemp(InTickFunc);
		CallbackFunc = MoveTemp(InCallbackFunc);

		BlendFunc = InBlendFunc;
		BlendExp = InBlendExp;

		if (TickFunc && Time > 0 && BlendExp != 0 && StartValue != StopValue)
		{
			SetMaxActionTime(Time);
			CurrentTime = 0.f;
			return true;
		}
		else
		{
			ensureMsgf(false, TEXT("ECF - Timeline Linear Color failed to start. Are you sure the Ticking time is greater than 0 and Ticking Function are set properly? /n Remember, that BlendExp must be different than zero and StartValue and StopValue must not be the same!"));
			return false;
		}
	}

	bool Setup(FLinearColor InStartValue, FLinearColor InStopValue, float InTime, TUniqueFunction<void(FLinearColor, float)>&& InTickFunc, TUniqueFunction<void(FLinearColor, float)>&& InCallbackFunc, EECFBlendFunc InBlendFunc, float InBlendExp)
	{
		CallbackFunc_NoStopped = MoveTemp(InCallbackFunc);
		return Setup(InStartValue, InStopValue, InTime, MoveTemp(InTickFunc), [this](FLinearColor FwdValue, float FwdTime, bool bStopped)
		{
			if (CallbackFunc_NoStopped)
			{
				CallbackFunc_NoStopped(FwdValue, FwdTime);
			}
		}, InBlendFunc, InBlendExp);
	}

	void Reset(bool bCallUpdate) override
	{
		CurrentTime = 0.f;
		CurrentValue = GetValue();

		if (bCallUpdate)
		{
			TickFunc(CurrentValue, CurrentTime);
		}
	}

	void Tick(float DeltaTime) override
	{
#if STATS
		DECLARE_SCOPE_CYCLE_COUNTER(TEXT("Timeline Linear Color - Tick"), STAT_ECFDETAILS_TIMELINELINEARCOLOR, STATGROUP_ECFDETAILS);
#endif
		CurrentTime = FMath::Clamp(CurrentTime + DeltaTime, 0.f, Time);
		CurrentValue = GetValue();

		TickFunc(CurrentValue, CurrentTime);

		if (CurrentTime >= Time)
		{
			Complete(false);
			MarkAsFinished();
		}
	}

	void Complete(bool bStopped) override
	{
		if (CallbackFunc)
		{
			CallbackFunc(CurrentValue, CurrentTime, bStopped);
		}
	}
};

ECF_PRAGMA_ENABLE_OPTIMIZATION