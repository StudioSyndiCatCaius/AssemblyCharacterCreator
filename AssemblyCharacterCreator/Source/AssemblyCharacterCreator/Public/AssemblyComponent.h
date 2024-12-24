// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataAsset.h"
#include "Styling/SlateBrush.h"
#include "Components/SkeletalMeshComponent.h"
#include "AssemblyComponent.generated.h"


USTRUCT()
struct FAssemblyAppearanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category="Assembly")
	int32 Seed;

	UPROPERTY(Instanced, EditAnywhere, Category="Assembly")
	UAssemblyMethod* AssemblyMethod;

	UPROPERTY(Instanced, EditAnywhere, Category="Assembly")
	UAssemblyModifier* AssemblyModifier;
	
	UPROPERTY()
	TMap<FString,FString> AssemblyProperties;
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ASSEMBLYCHARACTERCREATOR_API UAssemblyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAssemblyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	                           
	UPROPERTY(EditAnywhere, Category="Assembly")
	TSubclassOf<AAssemblySkin> Skin;

	UFUNCTION()
	void Update_Skin();

	UPROPERTY()
	UChildActorComponent* skinComponent;

	UFUNCTION(BlueprintCallable, Category="Assembly")
	AAssemblySkin* SetSkin(TSubclassOf<AAssemblySkin> SkinClass);

	UFUNCTION(BlueprintPure,Category="Assembly")
	AAssemblySkin* GetSkin();

	UPROPERTY(EditAnywhere, Category="Assembly")
	FAssemblyAppearanceData AppearanceData;

	UFUNCTION(BlueprintCallable, Category="Assembly", meta=(AdvancedDisplay="Preset"))
	void Assemble();

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category="Assembly")
	UAssemblyPreset* Preset;
	
	UPROPERTY()
	TArray<USkeletalMeshComponent*> MeshComponents;

	// Properties
	UFUNCTION(BlueprintCallable, Category="Assembly")
	void SetAssemblyProperty_String(FString Property, FString Value);

	UFUNCTION(BlueprintCallable, Category="Assembly")
	void SetAssemblyProperty_int(FString Property, int32 Value);

	UFUNCTION(BlueprintCallable, Category="Assembly")
	void SetAssemblyProperty_float(FString Property, float Value);
	
};

UCLASS(EditInlineNew, Blueprintable, BlueprintType, Abstract, CollapseCategories)
class ASSEMBLYCHARACTERCREATOR_API UAssemblyMethod : public UObject
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, Category="Assembly")
	TArray<USkeletalMeshComponent*> OnAssembled(UAssemblyComponent* Component);

	UFUNCTION(BlueprintNativeEvent, Category="Assembly")
	bool OnCleanup(UAssemblyComponent* Component);
};

UCLASS(EditInlineNew, Blueprintable, BlueprintType, Abstract, CollapseCategories)
class ASSEMBLYCHARACTERCREATOR_API UAssemblyModifier : public UObject
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category="Assembly")
	bool OnModify(UAssemblyComponent* Component, const TArray<USkeletalMeshComponent*>& MeshComponents);
	
};

UCLASS(EditInlineNew, Blueprintable, BlueprintType, Abstract, CollapseCategories)
class ASSEMBLYCHARACTERCREATOR_API UAssemblyOption : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assembly")
	FString OptionID;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assembly")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Assembly")
	FSlateBrush DisplayIcon;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Assembly")
	void Updated(UAssemblyComponent* Component, ACharacter* Character);
};

UCLASS()
class ASSEMBLYCHARACTERCREATOR_API UAssemblyBodyType : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, Instanced, EditAnywhere,Category="Assembly")
	TArray<UAssemblyOption*> Options;
	
	UFUNCTION(BlueprintPure,Category="Assembly")
	UAssemblyOption* GetOptionFromID(const FString& ID) const
	{
		for(auto* tempOption : Options)
		{
			if(tempOption && tempOption->OptionID==ID)
			{
				return tempOption;
			}
		}
		return nullptr;
	}
	
};



UCLASS()
class ASSEMBLYCHARACTERCREATOR_API UAssemblyPreset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category="Assembly")
	FAssemblyAppearanceData AppearanceData;
	
};

UCLASS()
class ASSEMBLYCHARACTERCREATOR_API UAssemblyFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category="Assembly", DisplayName="Merge Meshes (Assembly)")
	static USkeletalMesh* MergeMeshesAssembly(TArray<USkeletalMesh*> Meshes, USkeletalMesh* BaseMesh);

	UFUNCTION(BlueprintCallable, Category="Assembly", DisplayName="Merge Meshe Components (Assembly)")
	static USkeletalMesh* MergeComponentMeshesAssembly(TArray<USkeletalMeshComponent*> Meshes, USkeletalMesh* BaseMesh);
};


UCLASS()
class ASSEMBLYCHARACTERCREATOR_API AAssemblySkin : public AActor
{
	GENERATED_BODY()
public:
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assembly")
	USkeletalMesh* MasterSkeleton;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assembly")
	TSubclassOf<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assembly")
	bool bMerge;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assembly")
	bool bForceFollowMasterComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Assembly")
	bool bHideBaseMesh;
	
	UFUNCTION(BlueprintNativeEvent, Category="Assembly")
	TArray<USkeletalMeshComponent*> GetMeshMergeComponents();

	UFUNCTION(BlueprintCallable,Category="Assembly")
	void BuildSkin();
};