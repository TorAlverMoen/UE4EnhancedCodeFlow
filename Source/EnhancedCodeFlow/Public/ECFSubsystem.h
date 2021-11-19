// Copyright (c) 2021 Damian Nowakowski. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Tickable.h"
#include "ECFHandle.h"
#include "ECFActionSettings.h"
#include "ECFSubsystem.generated.h"

class UECFActionBase;

UCLASS()
class ENHANCEDCODEFLOW_API UECFSubsystem : public UWorldSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	friend class FEnhancedCodeFlow;

protected:

	/** UGameInstanceSubsystem interface implementation */
	void Initialize(FSubsystemCollectionBase& Collection) override;
	void Deinitialize() override;

	/** FTickableGameObject interface implementation */
	void Tick(float DeltaTime) override;
	TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(ECFSubsystem, STATGROUP_Tickables); }
	bool IsTickable() const override { return true; }
	bool IsTickableWhenPaused() const override { return true; }

	// Add Action to list. Returns the Action id.
	template<typename T, typename ... Ts>
	FECFHandle AddAction(UObject* InOwner, const FECFActionSettings& Settings, int64 InstanceId, Ts&& ... Args)
	{
		FECFHandle PossibleInstancedActionHandle = GetInstancedAction(InstanceId);
		if (PossibleInstancedActionHandle.IsValid())
		{
			return PossibleInstancedActionHandle;
		}

		T* NewAction = NewObject<T>(this);
		NewAction->SetAction(InOwner, ++LastHandleId, InstanceId, Settings);
		if (NewAction->Setup(Forward<Ts>(Args)...))
		{
			NewAction->Init();
			PendingAddActions.Add(NewAction);
			return NewAction->GetHandleId();
		}

		return FECFHandle();
	}

	// Remove Action of given HandleId from list. 
	void RemoveAction(FECFHandle& HandleId, bool bComplete);

	// Remove all Actions of async tasks of given class from list.
	void RemoveActionsOfClass(TSubclassOf<UECFActionBase> ActionClass, bool bComplete, UObject* InOwner);

	// Template version of RemoveActionsOfClass.
	template<typename T>
	void RemoveActionsOfClass(bool bComplete, UObject* InOwner)
	{
		RemoveActionsOfClass(T::StaticClass(), bComplete, InOwner);
	}

	// Remove all Actions of async tasks.
	void RemoveAllActions(bool bComplete, UObject* InOwner);

	// Check if the action is running
	bool HasAction(const FECFHandle& HandleId) const;

	//
	FECFHandle GetInstancedAction(int64 InstanceId);
	
	// List of active actions.
	UPROPERTY()
	TArray<UECFActionBase*> Actions;

	// List of nodes to be add in the future.
	UPROPERTY()
	TArray<UECFActionBase*> PendingAddActions;

	// Id of the last created node.
	FECFHandle LastHandleId;
	
	/** Getter handling */
	static UECFSubsystem* Get(const UObject* WorldContextObject);

	/** Helper function to finish action */
	void FinishAction(UECFActionBase* Action, bool bComplete);

	/** Utility function to check action validity. */
	static bool IsActionValid(UECFActionBase* Action);
};
