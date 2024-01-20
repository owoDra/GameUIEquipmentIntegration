// Copyright (C) 2024 owoDra

#pragma once

#include "Foundation/PawnWidget.h"

#include "EquipmentWidget.generated.h"

class UEquipmentManagerComponent;
class UEquipment;
class UItemData;


/**
 * Widget with the ability to track Pawn owned by the PlayerController that owns this widget
 * 
 * Tips:
 *	This can be used for widgets that display Pawn information (e.g. health bar, etc.)
 */
UCLASS(Abstract, Blueprintable)
class GUIEXT_API UEquipmentWidget : public UPawnWidget
{
	GENERATED_BODY()
public:
	UEquipmentWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeDestruct() override;


protected:
	//
	// Pawn owned by the PlayerController that owns this widget
	//
	UPROPERTY(Transient)
	TWeakObjectPtr<APawn> OwningPawn{ nullptr };

public:
	/**
	 * Get the Pawn owned by the PlayerController that owns this widget
	 * 
	 * Tips:
	 *	Basically returns the same as GetOwningPlayerPawn(), but caches the previously retrieved Pawn.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Pawn", meta = (DisplayName = "GetPawn"))
	APawn* GetCachedPawn();

protected:
	/**
	 * Listen for Pawn changes
	 */
	void ListenPawnChange();

	/**
	 * Unlisten for Pawn changes
	 */
	void UnlistenPawnChange();

	/**
	 * Notifies that Pawn has been changed
	 * 
	 * Tips:
	 *	Also executed when a new Pawn is cached by GetPawn()
	 */
	UFUNCTION()
	virtual void HandlePawnChange(APawn* OldPawn, APawn* NewPawn);

	UFUNCTION(BlueprintNativeEvent, Category = "Pawn")
	void OnPawnChanged(APawn* OldPawn, APawn* NewPawn);
	virtual void OnPawnChanged_Implementation(APawn* OldPawn, APawn* NewPawn) {}

};
