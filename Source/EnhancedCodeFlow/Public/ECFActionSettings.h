// Copyright (c) 2024 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "ECFActionSettings.generated.h"

USTRUCT(BlueprintType)
struct ENHANCEDCODEFLOW_API FECFActionSettings
{
	GENERATED_BODY()

	FECFActionSettings() :
		TickInterval(0.f),
		FirstDelay(0.f),
		bIgnorePause(false),
		bIgnoreGlobalTimeDilation(false),
		bStartPaused(false)
	{

	}

	FECFActionSettings(float InTickInterval, float InFirstDelay = 0.f, bool InIgnorePause = false, bool InIgnoreTimeDilation = false, bool InStartPaused = false) :
		TickInterval(InTickInterval),
		FirstDelay(InFirstDelay),
		bIgnorePause(InIgnorePause),
		bIgnoreGlobalTimeDilation(InIgnoreTimeDilation),
		bStartPaused(InStartPaused)
	{

	}

	UPROPERTY(BlueprintReadWrite)
	float TickInterval = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float FirstDelay = 0.f;

	UPROPERTY(BlueprintReadWrite)
	bool bIgnorePause = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIgnoreGlobalTimeDilation = false;

	UPROPERTY(BlueprintReadWrite)
	bool bStartPaused = false;
};

#define ECF_TICKINTERVAL(_Interval) FECFActionSettings(_Interval, 0.f, false, false, false)
#define ECF_DELAYFIRST(_Delay) FECFActionSettings(0.f, _Delay, false, false, false)
#define ECF_IGNOREPAUSE FECFActionSettings(0.f, 0.f, true, false, false)
#define ECF_IGNORETIMEDILATION FECFActionSettings(0.f, 0.f, false, true, false)
#define ECF_IGNOREPAUSEDILATION FECFActionSettings(0.f, 0.f, true, true, false)
#define ECF_STARTPAUSED FECFActionSettings(0.f, 0.f, false, false, true)
