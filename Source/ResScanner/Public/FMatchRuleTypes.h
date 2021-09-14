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
	NotStartWith,
	EndWith,
	NotEndWith,
	Wildcard,
	NotWildcard
};

UENUM(BlueprintType)
enum class EPathMatchMode : uint8
{
	WithIn,
	WithOut
};

USTRUCT(BlueprintType)
struct FNameMatchRule
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ENameMatchMode MatchMode;
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
struct FIgnoreRule
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FDirectoryPath> IgnoreFilters;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FSoftObjectPath> IgnoreAssets;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	FString RuleName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter",meta = (RelativeToGameContentDir, LongPackageName))
	TArray<FDirectoryPath> ScanFilters;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FString> ScanAssetTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FNameMatchRule> NameMatchRules;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FPathMatchRule> PathMatchRules;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FPropertyMatchRule> PropertyMatchRules;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<TSubclassOf<UOperatorBase>> ExternalRules;
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Filter")
	TArray<FIgnoreRule> IgnoreRules;
};


USTRUCT(BlueprintType)
struct FScannerConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FScannerMatchRule> ScannerRules;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FIgnoreRule> GlobalIgnoreRules;

	TMap<FString,TFunction<bool(const FAssetData&,const FScannerConfig&)>> MatchOperators;
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
	UPROPERTY(EditAnywhere,BlueprintReadWrite, transient)
	TArray<FAssetData> Assets;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FString> AssetPackageNames;
	// UPROPERTY(EditAnywhere,BlueprintReadWrite)
	// TArray<FMatchedRuleAsset> Assets;
	
};

USTRUCT(BlueprintType)
struct FMatchedResult
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TArray<FMatchedInfo> Assets;
};
