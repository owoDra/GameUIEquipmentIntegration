// Copyright (C) 2024 owoDra

#include "EquipmentWidget.h"

#include "EquipmentFunctionLibrary.h"
#include "EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentWidget)


UEquipmentWidget::UEquipmentWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentWidget::NativeDestruct()
{
	UnlistenEquipmentSlotEvent();

	Super::NativeDestruct();
}

void UEquipmentWidget::OnPawnChanged_Implementation(APawn* OldPawn, APawn* NewPawn)
{
	if (NewPawn)
	{
		CacheEquipmentManager();
	}
}


void UEquipmentWidget::CacheEquipmentManager()
{
	// If OwningPawn is not cached, cache it first

	if (!OwningPawn.IsValid())
	{
		GetCachedPawn();
		return;
	}

	if (auto* NewEMC{ UEquipmentFunctionLibrary::GetEquipmentManagerComponentFromActor(OwningPawn.Get()) })
	{
		// Suspend if Old and New are same

		if (EquipmentManagerComponent == NewEMC)
		{
			return;
		}

		// If there is already a cache, unlisten it.

		UnlistenEquipmentSlotEvent();

		EquipmentManagerComponent = NewEMC;

		ListenEquipmentSlotEvent();

		// Update slot information

		{
			FEquipmentSlotChangedMessage OutActiveSlotInfo;
			if (EquipmentManagerComponent->GetActiveSlotInfo(OutActiveSlotInfo))
			{
				HandleActiveSlotChange(OutActiveSlotInfo);
			}
		}
		
		{
			FEquipmentSlotChangedMessage OutSlotInfo;
			if (EquipmentManagerComponent->GetSlotInfo(AssociateSlotTag, OutSlotInfo))
			{
				HandleAssociateSlotChange(OutSlotInfo);
			}
		}
	}
}

void UEquipmentWidget::ListenEquipmentSlotEvent()
{
	if (EquipmentManagerComponent.IsValid())
	{
		EquipmentManagerComponent->OnActiveEquipmentSlotChange.AddDynamic(this, &ThisClass::HandleActiveSlotChange);
		EquipmentManagerComponent->OnEquipmentSlotChange.AddDynamic(this, &ThisClass::HandleAssociateSlotChange);
	}
}

void UEquipmentWidget::UnlistenEquipmentSlotEvent()
{
	if (EquipmentManagerComponent.IsValid())
	{
		EquipmentManagerComponent->OnActiveEquipmentSlotChange.RemoveDynamic(this, &ThisClass::HandleActiveSlotChange);
		EquipmentManagerComponent->OnEquipmentSlotChange.RemoveDynamic(this, &ThisClass::HandleAssociateSlotChange);
	}
}

void UEquipmentWidget::HandleActiveSlotChange(FEquipmentSlotChangedMessage Param)
{
	const auto bNewEquiped{ Param.SlotTag == AssociateSlotTag };

	if ((EquipedState == EAssociateEquipmentEquipedState::Unknown) || (HasEquiped() != bNewEquiped))
	{
		EquipedState = bNewEquiped ? EAssociateEquipmentEquipedState::Equiped : EAssociateEquipmentEquipedState::NotEquiped;

		OnActiveSlotChange(bNewEquiped);
	}
}

void UEquipmentWidget::HandleAssociateSlotChange(FEquipmentSlotChangedMessage Param)
{
	if (Param.SlotTag == AssociateSlotTag)
	{
		ItemData = Param.Data;
		Equipment = Param.Instance;

		OnSlotItemChanged(Param.Data.Get(), Param.Instance.Get());
	}
}


UEquipment* UEquipmentWidget::GetCachedEquipment()
{
	if (!Equipment.IsValid())
	{
		CacheEquipmentManager();
	}

	return Equipment.Get();
}

const UItemData* UEquipmentWidget::GetCachedItemData()
{
	if (!ItemData.IsValid())
	{
		CacheEquipmentManager();
	}

	return ItemData.Get();
}

bool UEquipmentWidget::HasEquiped() const
{
	return EquipedState == EAssociateEquipmentEquipedState::Equiped;
}
