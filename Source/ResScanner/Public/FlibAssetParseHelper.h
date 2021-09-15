// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "FMatchRuleTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Widgets/Notifications/SNotificationList.h"

#include "FlibAssetParseHelper.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFlibAssetParseHelper, Log, All);

/**
 * 
 */
UCLASS()
class RESSCANNER_API UFlibAssetParseHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static void CreateSaveFileNotify(const FText& InMsg, const FString& InSavedFile,SNotificationItem::ECompletionState NotifyType);
	static FProperty* GetPropertyByName(UObject* Obj,const FString& PropertyName);
	UFUNCTION(BlueprintCallable)
	static FString GetPropertyValueByName(UObject* Obj,const FString& PropertyName);
	static TArray<FAssetData> GetAssetsByFiltersByClass(const TArray<UClass*> AssetTypes,const TArray<FDirectoryPath>& FilterDirectorys);
	static TArray<FAssetData> GetAssetsByFilters(const TArray<FString> AssetTypes,const TArray<FDirectoryPath>& FilterDirectorys);
	static TArray<FAssetData> GetAssetsByFilters(const TArray<FString> AssetTypes,const TArray<FString>& FilterPaths);
	static IAssetRegistry& GetAssetRegistry(bool bSearchAllAssets = false);
	static bool IsIgnoreAsset(const FAssetData& AssetData,const TArray<FIgnoreRule>& IgnoreRules);
};

struct IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule)=0;
	virtual FString GetOperatorName()=0;
	virtual ~IMatchOperator(){};
};

struct NameMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule);
	virtual FString GetOperatorName(){ return TEXT("NameMatchRule");};
};

struct PathMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule);
	virtual FString GetOperatorName(){ return TEXT("PathMatchRule");};
};

struct PropertyMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule);
	virtual FString GetOperatorName(){ return TEXT("PropertyMatchRule");};
};

struct CustomMatchOperator:public IMatchOperator
{
	virtual bool Match(const FAssetData& AssetData,const FScannerMatchRule& Rule);
	virtual FString GetOperatorName(){ return TEXT("ExternalMatchRule");};
};
