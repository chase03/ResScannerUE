#pragma once
#include "TemplateHelper.hpp"
// engine header
#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"
#include "FMatchRuleTypes.generated.h"

UENUM(BlueprintType)
enum class ENameMatchMode : uint8
{
	StartWith,
	EndWith,
	Wildcard
};

UENUM(BlueprintType)
enum class EPathMatchMode : uint8
{
	WithIn,
	Wildcard,
};

USTRUCT(BlueprintType)
struct FNameMatchRule
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ENameMatchMode MatchMode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bReverseCheck;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Rule;

};

USTRUCT(BlueprintType)
struct FPathMatchRule
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPathMatchMode MatchMode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bReverseCheck;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString Rule;
};

UENUM(BlueprintType)
enum class EPropertyMatchRule:uint8
{
	Equal,
	NotEqual
};

USTRUCT(BlueprintType)
struct FPropertyMatchRule
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	EPropertyMatchRule MatchRule;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString PropertyName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString MatchValue;
};

USTRUCT(BlueprintType)
struct FAssetFilters
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite,meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> Filters;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FSoftObjectPath> Assets;
};

UCLASS(Blueprintable,BlueprintType)
class UOperatorBase : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	bool Match(UObject* Object,const FString& AssetType);
};

USTRUCT(BlueprintType)
struct FScannerMatchRule
{
	GENERATED_USTRUCT_BODY()
public:
	// 规则名
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	FString RuleName;
	// 规则描述
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	FString RuleDescribe;
	// 是否启用当前规则
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	bool bEnableRule = true;
	// 扫描资源路径
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter",meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> ScanFilters;
	// 资源类型
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<UClass*> ScanAssetTypes;
	// 命名匹配规则
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FNameMatchRule> NameMatchRules;
	// 路径匹配规则
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FPathMatchRule> PathMatchRules;
	// 属性匹配规则
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FPropertyMatchRule> PropertyMatchRules;
	// 自定义匹配规则（派生自UOperatorBase的类）
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<TSubclassOf<UOperatorBase>> CustomRules;
	// 忽略本规则的路径、资源列表
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	FAssetFilters IgnoreFilters;

	bool HasValidRules()const
	{
		return (NameMatchRules.Num() || PathMatchRules.Num() || PropertyMatchRules.Num() || CustomRules.Num());
	}
};


USTRUCT(BlueprintType)
struct FScannerConfig
{
	GENERATED_USTRUCT_BODY()
public:
	FORCEINLINE static FScannerConfig* Get()
	{
		static FScannerConfig StaticIns;
		return &StaticIns;
	}
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString ConfigName;
	// if true,only scan the GlobalScanFilters assets
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bByGlobalScanFilters = false;
	// if bByGlobalFilters is true,all rule using the filter assets
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditCondition="bByGlobalScanFilters"))
	FAssetFilters GlobalScanFilters;
	// force ignore assets,don't match any rule
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FAssetFilters> GlobalIgnoreFilters;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FScannerMatchRule> ScannerRules;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bSaveConfig = true;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	bool bSaveResult = true;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FDirectoryPath SavePath;
};

USTRUCT(BlueprintType)
struct FMatchedRuleAsset
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString OperatorRuleName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, transient)
	TArray<FAssetData> Assets;
};


USTRUCT(BlueprintType)
struct FMatchedInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString RuleName;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FString RuleDescribe;
	// 该规则在配置数组中的下标
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 RuleID;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, transient)
	TArray<FAssetData> Assets;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FString> AssetPackageNames;
};

USTRUCT(BlueprintType)
struct FMatchedResult
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FMatchedInfo> MatchedAssets;
};
