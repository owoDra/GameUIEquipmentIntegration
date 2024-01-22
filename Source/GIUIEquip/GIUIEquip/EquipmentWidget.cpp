// Copyright (C) 2024 owoDra

#include "EquipmentWidget.h"

// Game Equipment Extension
#include "EquipmentFunctionLibrary.h"
#include "EquipmentManagerComponent.h"
#include "Equipment/Equipment.h"

// Game Framework Core
#include "GameplayTag/GameplayTagStackInterface.h"

#include "GameFramework/PlayerState.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentWidget)


UEquipmentWidget::UEquipmentWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ListenMessageEvents();
}

void UEquipmentWidget::NativeDestruct()
{
	UnlistenMessageEvents();

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


UObject* UEquipmentWidget::GetStatTagTargetObject() const
{
	if (TrackingStatTagTarget == ETrackingStatTagTarget::EquipmentInstance)
	{
		return Equipment.Get();
	}
	else if (TrackingStatTagTarget == ETrackingStatTagTarget::Pawn)
	{
		return OwningPawn.Get();
	}
	else if (TrackingStatTagTarget == ETrackingStatTagTarget::PlayerController)
	{
		return GetOwningPlayer();
	}
	else if (TrackingStatTagTarget == ETrackingStatTagTarget::PlayerState)
	{
		return GetOwningPlayerState();
	}

	return nullptr;
}

void UEquipmentWidget::ListenMessageEvents()
{
	if (const auto* World{ GetWorld() })
	{
		if (bTrackingStatTagMessage && TrackingStatTags.IsValid())
		{
			auto& Subsystem{ UGameplayMessageSubsystem::Get(World) };

			StatTagStockMessageListenerHandle = Subsystem.RegisterListener<FGameplayTagStackCountChangeMessage>(TAG_Message_TagStackCountChange, this, &ThisClass::HandleStatTagStockMessage);
		}
	}
}

void UEquipmentWidget::UnlistenMessageEvents()
{
	if (StatTagStockMessageListenerHandle.IsValid())
	{
		StatTagStockMessageListenerHandle.Unregister();
	}
}

void UEquipmentWidget::HandleStatTagStockMessage(FGameplayTag MessageTag, const FGameplayTagStackCountChangeMessage& Message)
{
	if (TrackingStatTags.HasTag(Message.Tag) && Message.OwningObject == GetStatTagTargetObject())
	{
		OnStatTagChanged(Message);
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

		if (bTrackingStatTagMessage && TrackingStatTags.IsValid())
		{
			auto* TargetObject{ GetStatTagTargetObject() };
			if (!TargetObject)
			{
				return;
			}

			auto* Interface{ Cast<IGameplayTagStackInterface>(TargetObject) };

			if (ensureAlwaysMsgf(Interface, TEXT("(%s) has not implement IGameplayTagStackInterface"), *GetNameSafe(TargetObject)))
			{
				for (const auto& Tag : TrackingStatTags)
				{
					FGameplayTagStackCountChangeMessage Message;
					Message.Tag = Tag;
					Message.MaxCount = Interface->GetMaxStatTagStackCount(Tag);
					Message.Count = Interface->GetStatTagStackCount(Tag);
					Message.OwningObject = TargetObject;

					OnStatTagChanged(Message);
				}
			}
		}

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
