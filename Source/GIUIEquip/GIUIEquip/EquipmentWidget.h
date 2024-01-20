// Copyright (C) 2024 owoDra

#pragma once

#include "Foundation/PawnWidget.h"

#include "Type/EquipmentMessageTypes.h"

#include "GameplayTagContainer.h"

#include "EquipmentWidget.generated.h"

class UEquipmentManagerComponent;
class UEquipment;
class UItemData;


/**
 * Equipment status of equipment being tracked
 */
UENUM()
enum class EAssociateEquipmentEquipedState : uint8
{
	Unknown = 0,
	NotEquiped = 1,
	Equiped = 2
};


/**
 * Widget with the ability to track the equipment of the Pawn that owns this widget
 * 
 * Tips:
 *	Can be used to create widgets that display equipment status
 */
UCLASS(Abstract, Blueprintable)
class GIUIEQUIP_API UEquipmentWidget : public UPawnWidget
{
	GENERATED_BODY()
public:
	UEquipmentWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void NativeDestruct() override;

	virtual void OnPawnChanged_Implementation(APawn* OldPawn, APawn* NewPawn) override;


	/////////////////////////////////////////////////////////////////////////////////
	// Equipment Manager
protected:
	//
	// EquipmentManager of the Pawn that owns this widget
	//
	UPROPERTY(Transient)
	TWeakObjectPtr<UEquipmentManagerComponent> EquipmentManagerComponent{ nullptr };

protected:
	/**
	 * Cache the EquipmentManager of the current Pawn
	 */
	void CacheEquipmentManager();

	/**
	 * Listen for equipment slot change event
	 */
	void ListenEquipmentSlotEvent();

	/**
	 * Unlisten for equipment slot change event
	 */
	void UnlistenEquipmentSlotEvent();

	/**
	 * Notifies that active slot changed
	 * 
	 * Tips:
	 *	Also executed when a new equipment manager is cached by ChacheEquipmentManager()
	 */
	UFUNCTION()
	virtual void HandleActiveSlotChange(FEquipmentSlotChangedMessage Param);

	/**
	 * Notifies that associate slot item changed
	 *
	 * Tips:
	 *	Also executed when a new equipment manager is cached by ChacheEquipmentManager()
	 */
	UFUNCTION()
	virtual void HandleAssociateSlotChange(FEquipmentSlotChangedMessage Param);


	/////////////////////////////////////////////////////////////////////////////////
	// Equipment
protected:
	//
	// Equipment slots for the equipment tracked in this widget
	//
	UPROPERTY(EditDefaultsOnly, Category = "Equipment", meta = (Categories = "Equipment.Slot"))
	FGameplayTag AssociateSlotTag;

protected:
	//
	// Insutance of the equipment tracked by this widget
	//
	UPROPERTY(Transient)
	TWeakObjectPtr<UEquipment> Equipment{ nullptr };

	//
	// ItemData of the equipment tracked by this widget
	//
	UPROPERTY(Transient)
	TWeakObjectPtr<const UItemData> ItemData{ nullptr };

	//
	// Whether tracking equipment is equipped with
	//
	UPROPERTY(Transient)
	EAssociateEquipmentEquipedState EquipedState{ EAssociateEquipmentEquipedState::Unknown };

public:
	/**
	 * Get the Insutance of the equipment tracked by this widget
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DisplayName = "GetEquipmentInstance"))
	UEquipment* GetCachedEquipment();

	/**
	 * Get the ItemData of the equipment tracked by this widget
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DisplayName = "GetItemData"))
	const UItemData* GetCachedItemData();

	/**
	 * Return whether tracking equipment is equipped with
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment", meta = (DisplayName = "GetItemData"))
	bool HasEquiped() const;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnActiveSlotChange(bool bEquiped);
	virtual void OnActiveSlotChange_Implementation(bool bEquiped) {}

	UFUNCTION(BlueprintNativeEvent, Category = "Equipment")
	void OnSlotItemChanged(const UItemData* Data, UEquipment* Instance);
	virtual void OnSlotItemChanged_Implementation(const UItemData* Data, UEquipment* Instance) {}

};
