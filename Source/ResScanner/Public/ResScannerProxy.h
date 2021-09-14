#pragma once
#include "FMatchRuleTypes.h"
#include "FlibAssetParseHelper.h"
#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "ResScannerProxy.generated.h"
 
UCLASS(BlueprintType)
class RESSCANNER_API UResScannerProxy:public UObject
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    virtual void Init();
    UFUNCTION(BlueprintCallable)
    virtual void Shutdown();
    UFUNCTION(BlueprintCallable)
    virtual void DoScan();
    UFUNCTION(BlueprintCallable)
    virtual void SetScannerConfig(FScannerConfig InConfig);
    
    virtual TSharedPtr<FScannerConfig> GetScannerConfig(){return ScannerConfig;}
    virtual TMap<FString,TSharedPtr<IMatchOperator>>& GetMatchOperators(){return MatchOperators;}
private:
    FMatchedResult MatchedResult;
    TSharedPtr<FScannerConfig> ScannerConfig;
    TMap<FString,TSharedPtr<IMatchOperator>> MatchOperators;
};