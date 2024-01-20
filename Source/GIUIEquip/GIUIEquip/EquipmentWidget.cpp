// Copyright (C) 2024 owoDra

#include "EquipmentWidget.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentWidget)


UEquipmentWidget::UEquipmentWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}


void UEquipmentWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ListenPawnChange();
}

void UEquipmentWidget::NativeDestruct()
{
	UnlistenPawnChange();

	Super::NativeDestruct();
}


APawn* UEquipmentWidget::GetCachedPawn()
{
	if (!OwningPawn.IsValid())
	{
		HandlePawnChange(nullptr, GetOwningPlayerPawn());
	}

	return OwningPawn.Get();
}

void UEquipmentWidget::ListenPawnChange()
{
	auto* PC{ GetOwningPlayer() };
	check(PC);

	PC->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::HandlePawnChange);
}

void UEquipmentWidget::UnlistenPawnChange()
{
	if (auto* PC{ GetOwningPlayer() })
	{
		PC->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::HandlePawnChange);
	}
}

void UEquipmentWidget::HandlePawnChange(APawn* OldPawn, APawn* NewPawn)
{
	OwningPawn = NewPawn;

	OnPawnChanged(OldPawn, NewPawn);
}
