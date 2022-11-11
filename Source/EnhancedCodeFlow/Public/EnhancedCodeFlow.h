// Copyright (c) 2022 Damian Nowakowski. All rights reserved.

/**
 * Library of static functions used to launch Code Flow Actions.
 * You must use this library to control the plugin.
 * Most of the functions requires WorldContextObject to determine the
 * exact world this function should be launched in. This is needed in case
 * of multiple worlds running in the same editor instance (e.g. when testing multiplayer).
 * Every Action function require it's owner, so it can be properly cleaned up in case the owner
 * would be destroyed.
 * Launching action will return a handle to it. If the Handle is not valid (check IsValid() function)
 * it means the action couldn't be launched. Handles will be invalidated after action has finished.
 * Callbacks should be defined using lambdas.
 * Example of the plugin usage when using Delay Action:
 * FECFHandle DelayHandle = FFlow::Delay(this, 2.f, [this](float DeltaTime)
 * {
 *     // Stuff to do after 2 seconds delay.
 * });
 * Every function accepts an optional parameter of FECFActionSettings which can control
 * tick interval and flag to ignore global time dilation.
 */

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ECFHandle.h"
#include "ECFTypes.h"
#include "ECFActionSettings.h"
#include "ECFInstanceId.h"

class ENHANCEDCODEFLOW_API FEnhancedCodeFlow
{

public:
	
	/*^^^ ECF Flow Control Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/
	
	/**
	 * Checks if the action pointed by given handle is running.
	 */
	static bool IsActionRunning(const UObject* WorldContextObject, const FECFHandle& Handle);

	/**
	 * Pause ticking in the action pointed by given handle.
	 */
	static void PauseAction(const UObject* WorldContextObject, const FECFHandle& Handle);

	/**
	 * Resume ticking in the action pointed by given handle.
	 */
	static void ResumeAction(const UObject* WorldContextObject, const FECFHandle& Handle);

	/**
	 * Resume ticking in the action pointed by given handle.
	 * Returns false if there is no action.
	 */
	static bool IsActionPaused(const UObject* WorldContextObject, const FECFHandle& Handle, bool &bIsPaused);

	/**
	 * Sets if the ECF system is paused or not.
	 */
	static void SetPause(const UObject* WorldContextObject, bool bPaused);

	/**
	 * Checks if the ECF system is paused or not.
	 */
	static bool GetPause(const UObject* WorldContextObject);

	/*^^^ Stop ECF Functions ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Stops the running action pointed by given handle. Invalidates given handle.
	 * bComplete param indicates if the action should be completed when stopped (run callback), or simply stopped.
	 */
	static void StopAction(const UObject* WorldContextObject, FECFHandle& Handle, bool bComplete = false);

	/**
	 * Stops the running action with the given InstanceId.
	 * bComplete param indicates if the action should be completed when stopped (run callback), or simply stopped.
	 */
	static void StopInstancedAction(const UObject* WorldContextObject, FECFInstanceId InstanceId, bool bComplete = false);
	
	/**
	 * Stops all running actions.
	 * If owner is defined it will remove all actions from the given owner.
	 * Otherwise it will stop all the actions from everywhere.
	 * bComplete param indicates if the action should be completed when stopped (run callback), or simply stopped.
	 */
	static void StopAllActions(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Ticker ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Creates a ticker. It can tick specified amount of time or until it won't be stopped or when owning object won't be destroyed.
	 * To run ticker infinitely set InTickingTime to -1.
	 * @param InTickingTime [optional] - how long the ticker should tick. -1 means it will tick until it is explicitly stopped.
	 * @param InTickFunc - a ticking function can be: [](float DeltaTime) -> void, or [](float DeltaTime, FECFHandle TickerHandle) -> void.
	 * @param InCallbackFunc [optional] - a function which will be run after the last tick occurs. Must be: []() -> void.
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle AddTicker(UObject* InOwner, TUniqueFunction<void(float)>&& InTickFunc, TUniqueFunction<void()>&& InCallbackFunc = nullptr, const FECFActionSettings& Settings = {});
	static FECFHandle AddTicker(UObject* InOwner, float InTickingTime, TUniqueFunction<void(float)>&& InTickFunc, TUniqueFunction<void()>&& InCallbackFunc = nullptr, const FECFActionSettings& Settings = {});
	static FECFHandle AddTicker(UObject* InOwner, TUniqueFunction<void(float, FECFHandle)>&& InTickFunc, TUniqueFunction<void()>&& InCallbackFunc = nullptr, const FECFActionSettings& Settings = {});
	static FECFHandle AddTicker(UObject* InOwner, float InTickingTime, TUniqueFunction<void(float, FECFHandle)>&& InTickFunc, TUniqueFunction<void()>&& InCallbackFunc = nullptr, const FECFActionSettings& Settings = {});

	/**
	 * Removes all running tickers.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove tickers only from the given owner. Otherwise
	 *                             it will remove tickers from everywhere.
	 */
	static void RemoveAllTickers(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Delay ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Execute specified action after some time.
	 * @param InDelayTiem - time in seconds to wait before executing action.
	 * @param InCallbackFunc - a callback with action to execute. Must be: []() -> void. 
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle Delay(UObject* InOwner, float InDelayTime, TUniqueFunction<void()>&& InCallbackFunc, const FECFActionSettings& Settings = {});

	/**
	 * Stops all delays. Callbacks will not be executed.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove delayed actions only from the given owner. Otherwise
	 *                             it will remove delayed actions from everywhere.
	 */
	static void RemoveAllDelays(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Wait And Execute ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Waits until specific conditions are made and then execute code.
	 * @param InPredicate -	a function that decides if the action should launch. 
	 *                      If it returns true it means the action must be launched. Must be: []() -> bool.
	 * @param InCallbackFunc - a callback with action to execute. Must be: []() -> void.
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle WaitAndExecute(UObject* InOwner, TUniqueFunction<bool()>&& InPredicate, TUniqueFunction<void()>&& InCallbackFunc, const FECFActionSettings& Settings = {});

	/**
	 * Stops "wait and execute" actions. Callbacks will not be executed.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove "wait and execute" actions only from the given owner. 
	 *                             Otherwise it will remove all "wait and execute" actions from everywhere.
	 */
	static void RemoveAllWaitAndExecutes(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ While True Execute ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * While the specific condition is true the function will tick.
	 * @param InPredicate - a function that decides if the action should tick. 
	 *                      If it returns true - it means the action will tick. Must be: []() -> bool.
	 * @param InTickFunc -  a ticking function must be: [](float DeltaTime) -> void.
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle WhileTrueExecute(UObject* InOwner, TUniqueFunction<bool()>&& InPredicate, TUniqueFunction<void(float)>&& InTickFunc, const FECFActionSettings& Settings = {});

	/**
	 * Stops "while true execute" actions.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove "while true execute" actions only from the given owner. 
	 *							   Otherwise it will remove all "while true execute" actions from everywhere.
	 */
	static void RemoveAllWhileTrueExecutes(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Timeline ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Adds a simple timeline that runs in a given range during a given time.
	 * @param InStartValue -    the value from which this timeline will start.
	 * @param InStopValue -     the value to which this timeline will go. Must be different than InStartValue.
	 * @param InTime -          how long the timeline will be processed? Must be greater than 0.
	 * @param InTickFunc -      ticking function executed when timeline is processed. It's param represents current value. Must be: [](float CurrentValue, float CurrentTime) -> void.
	 * @param InCallbackFunc -  [optional] function which will be launched when timeline reaches end. Must be: [](float CurrentValue, float CurrentTime) -> void.
	 * @param InBlendFunc -     [optional] a function used to update timeline. By default it is Linear.
	 * @param InBlendExp -      [optional] an exponent, used by certain blend functions (EaseIn, EaseOut, EaseInOut) to control the shape of the timeline curve.
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle AddTimeline(UObject* InOwner, float InStartValue, float InStopValue, float InTime, TUniqueFunction<void(float, float)>&& InTickFunc, TUniqueFunction<void(float, float)>&& InCallbackFunc = nullptr, EECFBlendFunc InBlendFunc = EECFBlendFunc::ECFBlend_Linear, float InBlendExp = 1.f, const FECFActionSettings& Settings = {});

	/**
	 * Stops timelines. Will not launch callback functions.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove timelines only from the given owner. 
	 *                             Otherwise it will remove all timelines from everywhere.
	 */
	static void RemoveAllTimelines(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Custom Timeline ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Adds a custom timeline defined by a float curve.
	 * @param CurveFloat - a curve that defines this timeline.
	 * @param InTickFunc - a ticking executed when timeline is processed. IOt's param represents current value. Must be: [](float CurrentValue, float CurrentTime) -> void. 
	 * @param InCallbackFunc - [optional] function which will be launched when timeline reaches end. Must be: [](float CurrentValue, float CurrentTime) -> void.
	 * @param Settings [optional] - an extra settings to apply to this action.
	 */
	static FECFHandle AddCustomTimeline(UObject* InOwner, UCurveFloat* CurveFloat, TUniqueFunction<void(float, float)>&& InTickFunc, TUniqueFunction<void(float, float)>&& InCallbackFunc = nullptr, const FECFActionSettings& Settings = {});

	/**
	 * Stops custom timelines. Will not launch callback functions.
	 * @param bComplete			 - indicates if the action should be completed when stopped (run callback), or simply stopped.
	 * @param InOwner [optional] - if defined it will remove custom timelines only from the given owner. 
	 *                             Otherwise it will remove all custom timelines from everywhere.
	 */
	static void RemoveAllCustomTimelines(const UObject* WorldContextObject, bool bComplete = false, UObject* InOwner = nullptr);

	/*^^^ Time Lock ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Allow to run the code only once in a given time. (Locks the ability to run the code for a specific amount of time in seconds).
	 * @param InLockTime - time in seconds the lock will persist.
	 * @param InExecFunc - the function to execute.
	 * @param InstanceId - the id of the instance of this action.
	 */
	static FECFHandle TimeLock(UObject* InOwner, float InLockTime, TUniqueFunction<void()>&& InExecFunc, const FECFInstanceId& InstanceId, const FECFActionSettings& Settings = {});

	/**
	 * Stops time locks.
	 * @param InOwner [optional] - if defined it will remove time locks only from the given owner.
	 *                             Otherwise it will remove all time locks from everywhere.
	 */
	static void RemoveAllTimeLocks(const UObject* WorldContextObject, UObject* InOwner = nullptr);

	/*^^^ Do Once ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Run this code of block only once per instance.
	 * @param InExecFunc - the function to execute.
	 * @param InstanceId - the id of the instance of this action.
	 */
	static FECFHandle DoOnce(UObject* InOwner, TUniqueFunction<void()>&& InExecFunc, const FECFInstanceId& InstanceId);

	/**
	 * Stops DoOnces.
	 * @param InOwner [optional] - if defined it will remove time locks only from the given owner.
	 *                             Otherwise it will remove all time locks from everywhere.
	 */
	static void RemoveAllDoOnce(const UObject* WorldContextObject, UObject* InOwner = nullptr);

	/*^^^ Do N Times ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Run this code of block only N times per instance.
	 * @param InTimes - how many times it can be executed.
	 * @param InExecFunc - the function to execute. The function has a counter of executions.
	 * @param InstanceId - the id of the instance of this action.
	 */
	static FECFHandle DoNTimes(UObject* InOwner, const uint32 InTimes, TUniqueFunction<void(int32)>&& InExecFunc, const FECFInstanceId& InstanceId);

	/**
	 * Stops DoNTimes.
	 * @param InOwner [optional] - if defined it will remove time locks only from the given owner.
	 *                             Otherwise it will remove all time locks from everywhere.
	 */
	static void RemoveAllDoNTimes(const UObject* WorldContextObject, UObject* InOwner = nullptr);

	/*^^^ Do No More Than X Time ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^*/

	/**
	 * Run this code of block now and if run again - wait until the time after the previous run is no shorter than the given time.
	 * It can enqueue the given number of executions (must be at least 1).
	 * @param InExecFunc - the function to execute.
	 * @param InTime - the time of for how long the two code executions should be hold.
	 * @param InMaxExecsEnqueue - how many extra executions can be enqueued (must be at least 1).
	 * @param InstanceId - the id of the instance of this action.
	 */
	static FECFHandle DoNoMoreThanXTime(UObject* InOwner, TUniqueFunction<void()>&& InExecFunc, float InTime, int32 InMaxExecsEnqueue, FECFInstanceId& InstanceId, const FECFActionSettings& Settings = {});

	/**
	 * Stops DoNoMoreThanXTimes.
	 * @param InOwner [optional] - if defined it will remove time locks only from the given owner.
	 *                             Otherwise it will remove all time locks from everywhere.
	 */
	static void RemoveAllDoNoMoreThanXTimes(const UObject* WorldContextObject, UObject* InOwner = nullptr);
};

using FFlow = FEnhancedCodeFlow;