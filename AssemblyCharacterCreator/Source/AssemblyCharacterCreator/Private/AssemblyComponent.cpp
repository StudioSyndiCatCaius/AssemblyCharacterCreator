// Fill out your copyright notice in the Description page of Project Settings.

#include "AssemblyComponent.h"
#include "SkeletalMergingLibrary.h"
#include "GameFramework/Character.h"
#include "SkeletalMeshMerge.h"

// Sets default values for this component's properties
UAssemblyComponent::UAssemblyComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAssemblyComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAssemblyComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                       FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAssemblyComponent::Update_Skin()
{
	const ACharacter* charRef = Cast<ACharacter>(GetOwner());
	if(!charRef)
	{
		return;
	}
	const FTransform& temp_trans = charRef->GetMesh()->GetRelativeTransform();
	if(!skinComponent)
	{
		skinComponent = Cast<UChildActorComponent>(GetOwner()->AddComponentByClass(UChildActorComponent::StaticClass(),false,temp_trans,false));
	}
	if(skinComponent)
	{
		skinComponent->SetChildActorClass(Skin);
		skinComponent->SetRelativeTransform(temp_trans);
		if(Cast<AAssemblySkin>(skinComponent->GetChildActor()))
		{
			AAssemblySkin* skin_ref = Cast<AAssemblySkin>(skinComponent->GetChildActor());
		}
	}
}

AAssemblySkin* UAssemblyComponent::SetSkin(TSubclassOf<AAssemblySkin> SkinClass)
{
	Skin=SkinClass;
	Assemble();
	return GetSkin();
}

AAssemblySkin* UAssemblyComponent::GetSkin()
{
	if (skinComponent && skinComponent->GetChildActor())
	{
		return Cast<AAssemblySkin>(skinComponent->GetChildActor());
	}
	return nullptr;
}

void UAssemblyComponent::Assemble()
{
	Update_Skin();
	if(Preset)
	{
		AppearanceData=Preset->AppearanceData;
	}
	if(AppearanceData.AssemblyMethod)
	{
		AppearanceData.AssemblyMethod->OnCleanup(this);
		MeshComponents = AppearanceData.AssemblyMethod->OnAssembled(this);
	}
	if(AppearanceData.AssemblyModifier)
	{
		AppearanceData.AssemblyModifier->OnModify(this, MeshComponents);
	}
}

void UAssemblyComponent::SetAssemblyProperty_String(FString Property, FString Value)
{
	AppearanceData.AssemblyProperties.Add(Property,Value);
	Assemble();
}

void UAssemblyComponent::SetAssemblyProperty_int(FString Property, int32 Value)
{
	SetAssemblyProperty_String(Property,FString::FromInt(Value));
}

void UAssemblyComponent::SetAssemblyProperty_float(FString Property, float Value)
{
	SetAssemblyProperty_String(Property,FString::SanitizeFloat(Value));
}

USkeletalMesh* UAssemblyFunctions::MergeMeshesAssembly(TArray<USkeletalMesh*> Meshes, USkeletalMesh* BaseMesh)
{
	if(BaseMesh)
	{
		FSkeletalMeshMergeParams MergeParams;
		MergeParams.Skeleton = BaseMesh->GetSkeleton();
		for(auto* TempMesh : Meshes)
		{
			if(TempMesh && TempMesh->GetSkeleton()==MergeParams.Skeleton)
			{
				MergeParams.MeshesToMerge.Add(TempMesh);
			}
		}
		return USkeletalMergingLibrary::MergeMeshes(MergeParams);
	}
	return nullptr;
}

USkeletalMesh* UAssemblyFunctions::MergeComponentMeshesAssembly(TArray<USkeletalMeshComponent*> Meshes,
	USkeletalMesh* BaseMesh)
{
	TArray<USkeletalMesh*> IncomingMeshes;
	for(const auto* TempMesh : Meshes)
	{
		if(TempMesh)
		{
			IncomingMeshes.Add(TempMesh->GetSkeletalMeshAsset());
		}
	}
	return MergeMeshesAssembly(IncomingMeshes,BaseMesh);
}

void AAssemblySkin::OnConstruction(const FTransform& Transform)
{
	BuildSkin();
	Super::OnConstruction(Transform);
}

void AAssemblySkin::BuildSkin()
{
	if(Cast<ACharacter>(GetParentActor()))
	{
		ACharacter* charRef = Cast<ACharacter>(GetParentActor());

		// create merged mesh
		if(!MasterSkeleton)
		{
			return;
		}
		FSkeletalMeshMergeParams MergeParams;
		MergeParams.Skeleton=MasterSkeleton->GetSkeleton();
		for(auto* tempComp : GetMeshMergeComponents())
		{
			if (tempComp && tempComp->GetSkeletalMeshAsset() && tempComp->GetSkeletalMeshAsset()->GetSkeleton()==MasterSkeleton->GetSkeleton())
			{
				MergeParams.MeshesToMerge.Add(tempComp->GetSkeletalMeshAsset());
			}
		}

		USkeletalMesh* new_mesh=USkeletalMergingLibrary::MergeMeshes(MergeParams);
		
		// Apply mesh
		if(bMerge && new_mesh)
		{
			
			charRef->GetMesh()->SetSkeletalMeshAsset(new_mesh);

			//Clean Mesh
			for(auto* tempComp : GetMeshMergeComponents())
			{
				tempComp->DestroyComponent();
			}
		}
		else
		{
			if(MasterSkeleton)
			{
				charRef->GetMesh()->SetSkeletalMeshAsset(MasterSkeleton);
			}
			if(bForceFollowMasterComponent)
			{
				for(auto* tempComp : GetMeshMergeComponents())
				{
					tempComp->SetLeaderPoseComponent(charRef->GetMesh(),true);
				}
			}
			if(bHideBaseMesh)
			{
				//add blank material here
			}
			
		}

		//Set Anim Instance
		if(AnimationClass)
		{
			charRef->GetMesh()->SetAnimInstanceClass(AnimationClass);
		}
	}
	else
	{
		for(auto* tempComp : GetMeshMergeComponents())
		{
			if (tempComp && AnimationClass)
			{
				tempComp->SetAnimInstanceClass(AnimationClass);
			}
		}
	}
}

TArray<USkeletalMeshComponent*> AAssemblySkin::GetMeshMergeComponents_Implementation()
{
	TArray<USkeletalMeshComponent*> out;

	for(auto* TempComp : K2_GetComponentsByClass(USkeletalMeshComponent::StaticClass()))
	{
		out.Add(Cast<USkeletalMeshComponent>(TempComp));
	}
	return out;
}

bool UAssemblyModifier::OnModify_Implementation(UAssemblyComponent* Component,
                                                const TArray<USkeletalMeshComponent*>& MeshComponents)
{
	return false;
}

bool UAssemblyMethod::OnCleanup_Implementation(UAssemblyComponent* Component)
{
	return false;
}

TArray<USkeletalMeshComponent*> UAssemblyMethod::OnAssembled_Implementation(UAssemblyComponent* Component)
{
	TArray<USkeletalMeshComponent*> Emptyness;
	return Emptyness;
}

