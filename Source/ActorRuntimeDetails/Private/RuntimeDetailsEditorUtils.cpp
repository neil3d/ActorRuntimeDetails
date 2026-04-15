// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "RuntimeDetailsEditorUtils.h"

#define LOCTEXT_NAMESPACE "FRuntimeDetailsEditorUtilsEditorUtils"

int32 FRuntimeDetailsEditorUtils::DeleteComponents(const TArray<UActorComponent*>& ComponentsToDelete, UActorComponent*& OutComponentToSelect)
{
	int32 NumDeletedComponents = 0;

	for (UActorComponent* ComponentToDelete : ComponentsToDelete)
	{
		AActor* Owner = ComponentToDelete->GetOwner();
		check(Owner != nullptr);

		// If necessary, determine the component that should be selected following the deletion of the indicated component
		if (!OutComponentToSelect || ComponentToDelete == OutComponentToSelect)
		{
			USceneComponent* RootComponent = Owner->GetRootComponent();
			if (RootComponent != ComponentToDelete)
			{
				// Worst-case, the root can be selected
				OutComponentToSelect = RootComponent;

				if (USceneComponent* ComponentToDeleteAsSceneComp = Cast<USceneComponent>(ComponentToDelete))
				{
					if (USceneComponent* ParentComponent = ComponentToDeleteAsSceneComp->GetAttachParent())
					{
						// The component to delete has a parent, so we select that in the absence of an appropriate sibling
						OutComponentToSelect = ParentComponent;

						// Try to select the sibling that immediately precedes the component to delete
						TArray<USceneComponent*> Siblings;
						ParentComponent->GetChildrenComponents(false, Siblings);
						for (int32 i = 0; i < Siblings.Num() && ComponentToDelete != Siblings[i]; ++i)
						{
							if (IsValid(Siblings[i]))
							{
								OutComponentToSelect = Siblings[i];
							}
						}
					}
				}
				else
				{
					// For a non-scene component, try to select the preceding non-scene component
					for (UActorComponent* Component : Owner->GetComponents())
					{
						if (Component != nullptr)
						{
							if (Component == ComponentToDelete)
							{
								break;
							}
							else if (!Component->IsA<USceneComponent>())
							{
								OutComponentToSelect = Component;
							}
						}
					}
				}
			}
			else
			{
				OutComponentToSelect = nullptr;
			}
		}

		// Actually delete the component
		ComponentToDelete->Modify();
		ComponentToDelete->DestroyComponent(true);
		NumDeletedComponents++;
	}

	// Non-native components will be reinstanced, so we have to update the ptr after reconstruction
	// in order to avoid pointing at an invalid (trash) instance after re-running construction scripts.
	FName ComponentToSelectName;
	const AActor* ComponentToSelectOwner = nullptr;
	if (OutComponentToSelect && OutComponentToSelect->CreationMethod != EComponentCreationMethod::Native)
	{
		// Keep track of the pending selection's name and owner
		ComponentToSelectName = OutComponentToSelect->GetFName();
		ComponentToSelectOwner = OutComponentToSelect->GetOwner();

		// Reset the ptr value - we'll reassign it after reconstruction
		OutComponentToSelect = nullptr;
	}

	return NumDeletedComponents;
}

void FRuntimeDetailsEditorUtils::RenameComponentTemplate(UActorComponent* ComponentTemplate, const FName& NewName)
{
	if (ComponentTemplate != nullptr)
	{
		// Rename the component template (archetype) - note that this can be called during compile-on-load, so we include the flag not to reset the BPGC's package loader.
		const FString NewComponentName = NewName.ToString();
		ComponentTemplate->Rename(*(NewComponentName), nullptr, REN_DontCreateRedirectors | REN_ForceNoResetLoaders);
	}
}

bool FRuntimeDetailsEditorUtils::IsComponentNameAvailable(const FString& InString, UObject* ComponentOwner, const UActorComponent* ComponentToIgnore)
{
	UObject* Object = FindObjectFast<UObject>(ComponentOwner, *InString);

	bool bNameIsAvailable = Object == nullptr || Object == ComponentToIgnore;

	return bNameIsAvailable;
}

FVector& FRuntimeDetailsEditorUtils::GetRelativeLocation_DirectMutable(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeLocation_DirectMutable();
}

FVector FRuntimeDetailsEditorUtils::GetRelativeLocation(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeLocation();
}

void FRuntimeDetailsEditorUtils::SetRelativeLocation(USceneComponent* SceneComponent, FVector RelativeLocation)
{
	SceneComponent->SetRelativeLocation(RelativeLocation);
}

FRotator&  FRuntimeDetailsEditorUtils::GetRelativeRotation_DirectMutable(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeRotation_DirectMutable();
}

FRotator FRuntimeDetailsEditorUtils::GetRelativeRotation(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeRotation();
}

void FRuntimeDetailsEditorUtils::SetRelativeRotation(USceneComponent* SceneComponent, FRotator RelativeRotation)
{
	SceneComponent->SetRelativeRotation(RelativeRotation);
}

FVector& FRuntimeDetailsEditorUtils::GetRelativeScale3D_DirectMutable(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeScale3D_DirectMutable();
}

FVector FRuntimeDetailsEditorUtils::GetRelativeScale3D(USceneComponent* SceneComponent)
{
	return SceneComponent->GetRelativeScale3D();
}

void FRuntimeDetailsEditorUtils::SetRelativeScale3D(USceneComponent* SceneComponent, FVector RelativeScale3D)
{
	SceneComponent->SetRelativeScale3D(RelativeScale3D);
}

bool FRuntimeDetailsEditorUtils::IsUsingAbsoluteLocation(USceneComponent* SceneComponent)
{
	return SceneComponent->IsUsingAbsoluteLocation();
}

bool FRuntimeDetailsEditorUtils::IsUsingAbsoluteRotation(USceneComponent* SceneComponent)
{
	return SceneComponent->IsUsingAbsoluteRotation();
}

bool FRuntimeDetailsEditorUtils::IsUsingAbsoluteScale(USceneComponent* SceneComponent)
{
	return SceneComponent->IsUsingAbsoluteScale();
}

#undef LOCTEXT_NAMESPACE