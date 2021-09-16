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
	return std::move(UFlibAssetParseHelper::GetAssetsByFilters(Types,FilterDirectorys));
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByFilters(const TArray<FString> AssetTypes,
                                                             const TArray<FDirectoryPath>& FilterDirectorys)
{
	TArray<FString> FilterPaths;
	for(const auto& Directory:FilterDirectorys)
	{
		FilterPaths.AddUnique(Directory.Path);
	}
	return std::move(UFlibAssetParseHelper::GetAssetsByFilters(AssetTypes,FilterPaths));
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsByFilters(const TArray<FString> AssetTypes,
                                                             const TArray<FString>& FilterPaths)
{
	TArray<FAssetData> result;
	FARFilter Filter;
	Filter.PackagePaths.Append(FilterPaths);
	Filter.ClassNames.Append(AssetTypes);
	Filter.bRecursivePaths = true;
	UFlibAssetParseHelper::GetAssetRegistry().GetAssets(Filter, result);
	return std::move(result);
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
	return std::move(result);
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsWithCachedByTypes(const TArray<FAssetData>& CachedAssets,
	const TArray<UClass*>& AssetTypes)
{
	TArray<FString> Types;
	for(auto& Type:AssetTypes)
	{
		if(IsValid(Type))
		{
			Types.AddUnique(Type->GetName());
		}
	}
	if(!Types.Num())
	{
		UE_LOG(LogFlibAssetParseHelper,Error,TEXT("GetAssetsWithCachedByTypes Types is Emppty,Search all asset types."));
	}
	return std::move(UFlibAssetParseHelper::GetAssetsWithCachedByTypes(CachedAssets,Types));
}

TArray<FAssetData> UFlibAssetParseHelper::GetAssetsWithCachedByTypes(const TArray<FAssetData>& CachedAssets,
                                                                     const TArray<FString>& AssetTypes)
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
	return std::move(result);
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
	for(const auto& MatchRule:Rule.NameMatchRules.Rules)
	{
		int32 OptionalMatchNum = 0;
		for(const auto& RuleItem:MatchRule.Rules)
		{
			bool bMatchResult = false;
			switch (MatchRule.MatchMode)
			{
			case ENameMatchMode::StartWith:
				{
					bMatchResult = AssetName.StartsWith(RuleItem);
					break;
				}
			case ENameMatchMode::EndWith:
				{
					bMatchResult = AssetName.EndsWith(RuleItem);
					break;
				}
			case ENameMatchMode::Wildcard:
				{
					bMatchResult = AssetName.MatchesWildcard(RuleItem,ESearchCase::IgnoreCase);
					break;
				}
			}
			if(bMatchResult)
			{
				OptionalMatchNum++;
			}
		}
		bool bIsMatchAllRules = (OptionalMatchNum == MatchRule.Rules.Num());
		// Optional中匹配成功的数量必须与配置的一致
		bIsMatched = (MatchRule.MatchLogic == EMatchLogic::Necessary) ? bIsMatchAllRules : (MatchRule.OptionalRuleMatchNum == OptionalMatchNum);
		if(!bIsMatched)
		{
			break;
		}
	}
	if(Rule.NameMatchRules.Rules.Num())
	{
		bIsMatched = Rule.NameMatchRules.bReverseCheck ? !bIsMatched : bIsMatched;
	}
	return bIsMatched;
}
bool PathMatchOperator::Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
{
	bool bIsMatched = true;
	FString AssetPath = AssetData.ObjectPath.ToString();
	for(const auto& MatchRule:Rule.PathMatchRules.Rules)
	{
		int32 OptionalMatchNum = 0;
		for(const auto& RuleItem:MatchRule.Rules)
		{
			bool bMatchResult = false;
			switch (MatchRule.MatchMode)
			{
			case EPathMatchMode::WithIn:
				{
					bMatchResult = AssetPath.StartsWith(RuleItem);
					break;
				}
			case EPathMatchMode::Wildcard:
				{
					bMatchResult = AssetPath.MatchesWildcard(RuleItem,ESearchCase::IgnoreCase);
					break;
				}
			}
			if(bMatchResult)
			{
				OptionalMatchNum++;
			}
		}
		bool bIsMatchAllRules = (OptionalMatchNum == MatchRule.Rules.Num());
		bIsMatched = (MatchRule.MatchLogic == EMatchLogic::Necessary) ? bIsMatchAllRules : (MatchRule.OptionalRuleMatchNum == OptionalMatchNum);
		if(!bIsMatched)
		{
			break;
		}
	}
	if(Rule.PathMatchRules.Rules.Num())
	{
		bIsMatched = Rule.PathMatchRules.bReverseCheck ? !bIsMatched : bIsMatched;
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
		int32 OptionalMatchNum = 0;
		for(const auto& PropertyRule:MatchRule.Rules)
		{
			bool bMatchResult = false;
			FString Value = UFlibAssetParseHelper::GetPropertyValueByName(Asset,PropertyRule.PropertyName);
			if(!Value.IsEmpty())
			{
				bool bIsFloatType = IsFloatLambda(Asset,PropertyRule.PropertyName);
				if(!bIsFloatType)
				{
					bMatchResult = Value.Equals(PropertyRule.MatchValue);
				}
				else
				{
					bMatchResult = IsFloatEqualLambda(Value,PropertyRule.MatchValue);
				}
				bMatchResult = (PropertyRule.MatchRule == EPropertyMatchRule::NotEqual) ? !bMatchResult : bMatchResult;
			}
			if(bMatchResult)
			{
				OptionalMatchNum++;
			}
		}
		bool bIsMatchAllRules = (OptionalMatchNum == MatchRule.Rules.Num());
		// Optional中匹配成功的数量必须与配置的一致
		bIsMatched = (MatchRule.MatchLogic == EMatchLogic::Necessary) ? bIsMatchAllRules : (MatchRule.OptionalRuleMatchNum == OptionalMatchNum);
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
				if(!bIsMatched && Operator->GetMatchLogic() == EMatchLogic::Necessary)
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
