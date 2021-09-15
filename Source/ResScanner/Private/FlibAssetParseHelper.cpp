// Fill out your copyright notice in the Description page of Project Settings.


#include "FlibAssetParseHelper.h"
#include "TemplateHelper.hpp"

// engine header
#include "Kismet/KismetStringLibrary.h"
#include "AssetRegistryModule.h"
#include "ARFilter.h"
#include "Engine/AssetManager.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

DEFINE_LOG_CATEGORY(LogFlibAssetParseHelper);

void UFlibAssetParseHelper::CreateSaveFileNotify(const FText& InMsg, const FString& InSavedFile,SNotificationItem::ECompletionState NotifyType)
{
	AsyncTask(ENamedThreads::GameThread,[InMsg,InSavedFile,NotifyType]()
	{
		auto Message = InMsg;
		FNotificationInfo Info(Message);
		Info.bFireAndForget = true;
		Info.ExpireDuration = 5.0f;
		Info.bUseSuccessFailIcons = false;
		Info.bUseLargeFont = false;

		const FString HyperLinkText = InSavedFile;
		Info.Hyperlink = FSimpleDelegate::CreateLambda(
			[](FString SourceFilePath)
			{
				FPlatformProcess::ExploreFolder(*SourceFilePath);
			},
			HyperLinkText
		);
		Info.HyperlinkText = FText::FromString(HyperLinkText);
		FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(NotifyType);
	});
}

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

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByFiltersByClass(const TArray<UClass*> AssetTypes,
	const TArray<FDirectoryPath>& FilterDirectorys)
{
	TArray<FString> Types;
	for(auto& Type:AssetTypes)
	{
		if(IsValid(Type))
		{
			Types.AddUnique(Type->GetName());
		}
	}
	return UFlibAssetParseHelper::GetAssetsByFilters(Types,FilterDirectorys);
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

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByObjectPath(const TArray<FSoftObjectPath> SoftObjectPaths)
{
	TArray<FAssetData> result;
	UAssetManager& AssetManager = UAssetManager::Get();
	for(const auto& ObjectPath:SoftObjectPaths)
	{
		FAssetData OutAssetData;
		if (AssetManager.GetAssetDataForPath(ObjectPath, OutAssetData) && OutAssetData.IsValid())
		{
			result.AddUnique(OutAssetData);
		}
	}
	return result;
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsWithCachedByTypes(const TArray<FAssetData>& CachedAssets,
	const TArray<UClass*> AssetTypes)
{
	TArray<FString> Types;
	for(auto& Type:AssetTypes)
	{
		if(IsValid(Type))
		{
			Types.AddUnique(Type->GetName());
		}
	}
	return UFlibAssetParseHelper::GetAssetsWithCachedByTypes(CachedAssets,Types);
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsWithCachedByTypes(const TArray<FAssetData>& CachedAssets,
                                                                     const TArray<FString> AssetTypes)
{
	TArray<FAssetData> result;
	for(const auto& CachedAsset:CachedAssets)
	{
		for(const auto& Type:AssetTypes)
		{
			if(CachedAsset.AssetClass.ToString().Equals(Type))
			{
				result.AddUnique(CachedAsset);
				break;
			}
		}
	}
	return result;
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

bool UFlibAssetParseHelper::IsIgnoreAsset(const FAssetData& AssetData, const TArray<FAssetFilters>& IgnoreRules)
{
	bool bIsIgnored = false;
	for(const auto& IgnoreRule:IgnoreRules)
	{
		for(const auto& Filter: IgnoreRule.Filters)
		{
			if(AssetData.PackagePath.ToString().StartsWith(Filter.Path))
			{
				bIsIgnored = true;
				break;
			}
		}
		if(!bIsIgnored)
		{
			for(const auto& Asset: IgnoreRule.Assets)
			{
				if(AssetData.ObjectPath.ToString().Equals(Asset.GetAssetPathString()))
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



bool NameMatchOperator::Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
{
	bool bIsMatched = true;
	FString AssetName = AssetData.AssetName.ToString();
	for(const auto& MatchRule:Rule.NameMatchRules)
	{
		switch (MatchRule.MatchMode)
		{
		case ENameMatchMode::StartWith:
			{
				bIsMatched = AssetName.StartsWith(MatchRule.Rule);
				break;
			}
		case ENameMatchMode::EndWith:
			{
				bIsMatched = AssetName.EndsWith(MatchRule.Rule);
				break;
			}
		case ENameMatchMode::Wildcard:
			{
				bIsMatched = AssetName.MatchesWildcard(MatchRule.Rule,ESearchCase::IgnoreCase);
				break;
			}
		}
		bIsMatched = MatchRule.bReverseCheck ? !bIsMatched : bIsMatched;
		if(!bIsMatched)
		{
			break;
		}
	}
	return bIsMatched;
}
bool PathMatchOperator::Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)

{
	bool bIsMatched = true;
	FString AssetPath = AssetData.ObjectPath.ToString();
	for(const auto& MatchRule:Rule.PathMatchRules)
	{
		switch (MatchRule.MatchMode)
		{
		case EPathMatchMode::WithIn:
			{
				bIsMatched = AssetPath.StartsWith(MatchRule.Rule);
				break;
			}
		case EPathMatchMode::Wildcard:
			{
				bIsMatched = AssetPath.MatchesWildcard(MatchRule.Rule,ESearchCase::IgnoreCase);
				break;
			}
		}
		bIsMatched = MatchRule.bReverseCheck ? !bIsMatched : bIsMatched;
		if(!bIsMatched)
		{
			break;
		}
	}
	return bIsMatched;
}

bool PropertyMatchOperator::Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
{
	bool bIsMatched = true;
	UObject* Asset = AssetData.GetAsset();

	auto IsFloatLambda = [](UObject* Object,const FString& PropertyName)->bool
	{
		FProperty* Property = UFlibAssetParseHelper::GetPropertyByName(Object, PropertyName);
		return Property->IsA(FFloatProperty::StaticClass());
	};
	auto IsFloatEqualLambda = [](const FString& L,const FString& R)->bool
	{
		float LValue = UKismetStringLibrary::Conv_StringToFloat(L);
		float RValue = UKismetStringLibrary::Conv_StringToFloat(R);
		return (LValue == RValue);
	};
		
	for(const auto& MatchRule:Rule.PropertyMatchRules)
	{
		FString Value = UFlibAssetParseHelper::GetPropertyValueByName(Asset,MatchRule.PropertyName);
		if(!Value.IsEmpty())
		{
			bool bIsFloatType = IsFloatLambda(Asset,MatchRule.PropertyName);
			if(!bIsFloatType)
			{
				bIsMatched = Value.Equals(MatchRule.MatchValue);
			}
			else
			{
				bIsMatched = IsFloatEqualLambda(Value,MatchRule.MatchValue);
			}
			bIsMatched = (MatchRule.MatchRule == EPropertyMatchRule::NotEqual) ? !bIsMatched : bIsMatched;
		}
		if(!bIsMatched)
		{
			break;
		}
	}
	return bIsMatched;
}
bool CustomMatchOperator::Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
{
	bool bIsMatched = true;
	for(auto ExOperator:Rule.CustomRules)
	{
		if(IsValid(ExOperator))
		{
			UOperatorBase* Operator = Cast<UOperatorBase>(ExOperator->GetDefaultObject());
			if(Operator)
			{
				bIsMatched = Operator->Match(AssetData.GetAsset(),AssetData.AssetClass.ToString());
				if(!bIsMatched)
				{
					break;
				}
			}
			else
			{
				UE_LOG(LogFlibAssetParseHelper,Log,TEXT("% is Invalid UOperatorBase class!"),*ExOperator->GetName());
			}
		}
	}
	return bIsMatched;
}
