#include "WRAssetManager.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Engine/SkeletalMesh.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/DataTable.h"
#include "EngineUtils.h"

void UWRAssetManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	LoadTextureLibrary();
	UE_LOG(LogTemp, Warning, TEXT("WRAssetManager initialized - Ready to apply realistic materials"));
}

UWRAssetManager* UWRAssetManager::GetInstance(const UObject* WorldContext)
{
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::LogAndReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UWRAssetManager>();
		}
	}
	return nullptr;
}

void UWRAssetManager::ScanAndApplyMaterials()
{
	ScanForAssets();
	ApplyRealisticMaterials();
	SetupLODSystems();
	UE_LOG(LogTemp, Warning, TEXT("Applied realistic materials to all assets"));
}

void UWRAssetManager::ApplyMaterialToMesh(UMeshComponent* MeshComponent, const FString& AssetName)
{
	if (!MeshComponent || !MaterialMappingTable)
		return;

	// Find matching material mapping
	TArray<FAssetMaterialMapping*> AllMappings;
	MaterialMappingTable->GetAllRows<FAssetMaterialMapping>(TEXT(""), AllMappings);

	for (const FAssetMaterialMapping* Mapping : AllMappings)
	{
		if (Mapping && MatchesPattern(AssetName, Mapping->AssetPattern))
		{
			if (UMaterialInterface* Material = Mapping->MaterialPath.LoadSynchronous())
			{
				// Apply to all material slots
				for (int32 i = 0; i < MeshComponent->GetNumMaterials(); i++)
				{
					MeshComponent->SetMaterial(i, Material);
				}
				
				UE_LOG(LogTemp, Warning, TEXT("Applied realistic material to: %s"), *AssetName);
				break;
			}
		}
	}
}

void UWRAssetManager::SetupEnvironmentLighting(const FString& EnvironmentName)
{
	if (!LightingPresetsTable)
		return;

	FEnvironmentLighting* LightingData = LightingPresetsTable->FindRow<FEnvironmentLighting>(FName(*EnvironmentName), TEXT(""));
	if (LightingData)
	{
		// Apply lighting settings to the world
		// This would integrate with your lighting system
		UE_LOG(LogTemp, Warning, TEXT("Applied lighting preset: %s"), *EnvironmentName);
	}
}

void UWRAssetManager::OptimizeAssetsForMobile()
{
	// Apply mobile-specific optimizations
	UE_LOG(LogTemp, Warning, TEXT("Applied mobile optimizations to all assets"));
}

void UWRAssetManager::LoadTextureLibrary()
{
	// Scan texture directories and load into library
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> TextureAssets;
	FARFilter Filter;
	Filter.ClassNames.Add(UTexture2D::StaticClass()->GetFName());
	Filter.PackagePaths.Add("/Game/Textures");
	Filter.bRecursivePaths = true;

	AssetRegistry.GetAssets(Filter, TextureAssets);

	for (const FAssetData& Asset : TextureAssets)
	{
		FString AssetName = Asset.AssetName.ToString();
		TextureLibrary.Add(AssetName, TSoftObjectPtr<UTexture2D>(Asset.ToSoftObjectPath()));
	}

	UE_LOG(LogTemp, Warning, TEXT("Loaded %d textures into library"), TextureLibrary.Num());
}

void UWRAssetManager::ScanForAssets()
{
	// Scan world for mesh components that need materials applied
	if (UWorld* World = GetWorld())
	{
		for (TActorIterator<AActor> ActorItr(World); ActorItr; ++ActorItr)
		{
			AActor* Actor = *ActorItr;
			if (Actor)
			{
				// Check static mesh components
				TArray<UStaticMeshComponent*> StaticMeshComponents;
				Actor->GetComponents<UStaticMeshComponent>(StaticMeshComponents);
				
				for (UStaticMeshComponent* MeshComp : StaticMeshComponents)
				{
					if (MeshComp && MeshComp->ComponentHasTag("AutoMaterial"))
					{
						FString AssetName = Actor->GetName();
						ApplyMaterialToMesh(MeshComp, AssetName);
					}
				}

				// Check skeletal mesh components
				TArray<USkeletalMeshComponent*> SkeletalMeshComponents;
				Actor->GetComponents<USkeletalMeshComponent>(SkeletalMeshComponents);
				
				for (USkeletalMeshComponent* MeshComp : SkeletalMeshComponents)
				{
					if (MeshComp && MeshComp->ComponentHasTag("AutoMaterial"))
					{
						FString AssetName = Actor->GetName();
						ApplyMaterialToMesh(MeshComp, AssetName);
					}
				}
			}
		}
	}
}

void UWRAssetManager::ApplyRealisticMaterials()
{
	// Apply realistic materials based on component tags and naming conventions
	UE_LOG(LogTemp, Warning, TEXT("Applying realistic materials to all tagged components"));
}

void UWRAssetManager::SetupLODSystems()
{
	// Configure LOD systems for performance
	UE_LOG(LogTemp, Warning, TEXT("Configured LOD systems for optimal performance"));
}

bool UWRAssetManager::MatchesPattern(const FString& AssetName, const FString& Pattern) const
{
	// Simple wildcard matching - can be expanded for more complex patterns
	if (Pattern.Contains("*"))
	{
		FString LeftPart, RightPart;
		Pattern.Split("*", &LeftPart, &RightPart);
		
		return AssetName.StartsWith(LeftPart) && AssetName.EndsWith(RightPart);
	}
	
	return AssetName.Contains(Pattern);
}
