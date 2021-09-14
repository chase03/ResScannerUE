// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "FMatchRuleTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetStringLibrary.h"
#include "FlibAssetParseHelper.generated.h"


/**
 * 
 */
UCLASS()
class RESSCANNER_API UFlibAssetParseHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static FProperty* GetPropertyByName(UObject* Obj,const FString& PropertyName);
	UFUNCTION(BlueprintCallable)
	static FString GetPropertyValueByName(UObject* Obj,const FString& PropertyName);
	static TArray<FAssetData> GetAssetsByFilters(const TArray<FString> AssetTypes,const TArray<FDirectoryPath>& FilterDirectorys);
	static TArray<FAssetData> GetAssetsByFilters(const TArray<FString> AssetTypes,const TArray<FString>& FilterPaths);
	static IAssetRegistry& GetAssetRegistry(bool bSearchAllAssets = false);
	
	static bool IsIgnoreAsset(const FAssetData& AssetData,const TArray<FIgnoreRule>& IgnoreRules);
};

struct IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)=0;
	virtual FString GetOperatorName()=0;
};

struct NameMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
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
			case ENameMatchMode::NotStartWith:
				{
					bIsMatched = !AssetName.StartsWith(MatchRule.Rule);
					break;
				}
			case ENameMatchMode::EndWith:
				{
					bIsMatched = AssetName.EndsWith(MatchRule.Rule);
					break;
				}
			case ENameMatchMode::NotEndWith:
				{
					bIsMatched = !AssetName.EndsWith(MatchRule.Rule);
					break;
				}
			case ENameMatchMode::Wildcard:
				{
					bIsMatched = AssetName.MatchesWildcard(MatchRule.Rule,ESearchCase::IgnoreCase);
					break;
				}
			case ENameMatchMode::NotWildcard:
				{
							
					bIsMatched = !AssetName.MatchesWildcard(MatchRule.Rule,ESearchCase::IgnoreCase);
					break;
				}
			}
			if(!bIsMatched)
			{
				break;
			}
		}
		return bIsMatched;
	}
	virtual FString GetOperatorName(){ return TEXT("NameMatchRule");};
};

struct PathMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
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
			case EPathMatchMode::WithOut:
				{
					bIsMatched = !AssetPath.StartsWith(MatchRule.Rule);
					break;
				}
			}
			if(!bIsMatched)
			{
				break;
			}
		}
		return bIsMatched;
	}
	virtual FString GetOperatorName(){ return TEXT("PathMatchRule");};
};

struct PropertyMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
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
				// switch(MatchRule.MatchRule)
				// {
				// case EPropertyMatchRule::Equal:
				// 	{
				// 		if(!bIsFloatType)
				// 		{
				// 			bIsMatched = Value.Equals(MatchRule.MatchValue);
				// 		}
				// 		else
				// 		{
				// 			bIsMatched = IsFloatEqualLambda(Value,MatchRule.MatchValue);
				// 		}
				// 		
				// 		break;
				// 	}
				// case EPropertyMatchRule::NotEqual:
				// 	{
				// 		if(!bIsFloatType)
				// 		{
				// 			bIsMatched = !Value.Equals(MatchRule.MatchValue);
				// 		}
				// 		else
				// 		{
				// 			bIsMatched = !IsFloatEqualLambda(Value,MatchRule.MatchValue);
				// 		}
				// 		break;
				// 	}
				// }
			}
			if(!bIsMatched)
			{
				break;
			}
		}
		return bIsMatched;
	}
	virtual FString GetOperatorName(){ return TEXT("PropertyMatchRule");};
};

struct ExternalMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)
	{
		bool bIsMatched = true;
		for(auto ExOperator:Rule.ExternalRules)
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
					UE_LOG(LogTemp,Log,TEXT("% is Invalid UOperatorBase class!"),*ExOperator->GetName());
				}
			}
		}
		return bIsMatched;
	}
	virtual FString GetOperatorName(){ return TEXT("ExternalMatchRule");};
};
