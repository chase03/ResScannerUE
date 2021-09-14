// Fill out your copyright notice in the Description page of Project Settings.


#include "FlibAssetParseHelper.h"
#include "TemplateHelper.hpp"

// engine header
#include "AssetRegistryModule.h"
#include "ARFilter.h"

FProperty* UFlibAssetParseHelper::GetPropertyByName(UObject* Obj, const FString& PropertyName)
{
	FProperty* Result = nullptr;
	for(TFieldIterator<FProperty> PropertyIter(Obj->GetClass());PropertyIter;++PropertyIter)
	{
		if(PropertyIter->GetName().Equals(PropertyName))
		{
			Result = *PropertyIter;
		}
		// UE_LOG(LogTemp,Log,TEXT("Property Name: %s"),*PropertyIter->GetName());
	}
	return Result;
}

FString UFlibAssetParseHelper::GetPropertyValueByName(UObject* Obj, const FString& PropertyName)
{
	FString Result;
	FProperty* Property = UFlibAssetParseHelper::GetPropertyByName(Obj, PropertyName);
	if(Property)
	{
		Property->ExportTextItem(Result,Property->ContainerPtrToValuePtr<void*>(Obj),TEXT(""),NULL,NULL);
	}
	return Result;
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByFilters(const TArray<FString> AssetTypes,
	const TArray<FDirectoryPath>& FilterDirectorys)
{
	TArray<FString> FilterPaths;
	for(const auto& Directory:FilterDirectorys)
	{
		FilterPaths.AddUnique(Directory.Path);
	}
	return UFlibAssetParseHelper::GetAssetsByFilters(AssetTypes,FilterPaths);
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByFilters(const TArray<FString> AssetTypes,
                                                             const TArray<FString>& FilterPaths)
{
	TArray<FAssetData> Result;
	FARFilter Filter;
	Filter.PackagePaths.Append(FilterPaths);
	Filter.ClassNames.Append(AssetTypes);
	Filter.bRecursivePaths = true;
	UFlibAssetParseHelper::GetAssetRegistry().GetAssets(Filter, Result);
	return Result;
}

IAssetRegistry& UFlibAssetParseHelper::GetAssetRegistry(bool bSearchAllAssets)
{
	
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	if(bSearchAllAssets)
	{
		AssetRegistryModule.Get().SearchAllAssets(true);
	}
	return AssetRegistryModule.Get();
}

bool UFlibAssetParseHelper::IsIgnoreAsset(const FAssetData& AssetData, const TArray<FIgnoreRule>& IgnoreRules)
{
	bool bIsIgnored = false;
	for(const auto& IgnoreRule:IgnoreRules)
	{
		for(const auto& Filter: IgnoreRule.IgnoreFilters)
		{
			if(AssetData.PackagePath.ToString().StartsWith(Filter.Path))
			{
				bIsIgnored = true;
				break;
			}
		}
		if(!bIsIgnored)
		{
			for(const auto& Asset: IgnoreRule.IgnoreAssets)
			{
				if(AssetData.PackagePath.ToString().Equals(Asset.GetAssetPathString()))
				{
					bIsIgnored = true;
				}
			}
		}
		else
		{
			break;
		}
	}
	return bIsIgnored;
}

