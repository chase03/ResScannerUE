#include "ResScannerProxy.h"

void UResScannerProxy::Init()
{
	MatchedResult.Assets.Empty();
	if(!GetScannerConfig().IsValid())
	{
		ScannerConfig = MakeShareable(new FScannerConfig);
	}
	MatchOperators.Add(TEXT("NameMatchRule"),MakeShareable(new NameMatchOperator));
	MatchOperators.Add(TEXT("PathMatchRule"),MakeShareable(new PathMatchOperator));
	MatchOperators.Add(TEXT("PropertyMatchRule"),MakeShareable(new PropertyMatchOperator));
	MatchOperators.Add(TEXT("ExternalMatchRule"),MakeShareable(new ExternalMatchOperator));
}

void UResScannerProxy::Shutdown()
{
	MatchedResult.Assets.Empty();
}

void UResScannerProxy::DoScan()
{
	MatchedResult.Assets.Empty();
	for(const auto& ScannerRule:GetScannerConfig()->ScannerRules)
	{
		TArray<FAssetData> FilterAssets = UFlibAssetParseHelper::GetAssetsByFilters(ScannerRule.ScanAssetTypes,ScannerRule.ScanFilters);
		FMatchedInfo MatchedInfo;
		MatchedInfo.RuleName = ScannerRule.RuleName;
		for(const auto& Asset:FilterAssets)
		{
			if(!UFlibAssetParseHelper::IsIgnoreAsset(Asset,GetScannerConfig()->GlobalIgnoreRules) &&
			   !UFlibAssetParseHelper::IsIgnoreAsset(Asset,ScannerRule.IgnoreRules)
			)
			{
				bool bMatchAllRules = true;
				for(const auto& Operator:GetMatchOperators())
				{
					bMatchAllRules = Operator.Value->Match(Asset,ScannerRule);
					if(!bMatchAllRules)
					{
						break;
					}
				}
				if(bMatchAllRules)
				{
					MatchedInfo.Assets.AddUnique(Asset);
					MatchedInfo.AssetPackageNames.AddUnique(Asset.PackageName.ToString());
				}
			}
		}
		if(!!MatchedInfo.Assets.Num())
		{
			MatchedResult.Assets.Add(MatchedInfo);
		}
	}
	FString OutString;
	TemplateHelper::TSerializeStructAsJsonString(MatchedResult,OutString);
	UE_LOG(LogTemp,Log,TEXT("%s"),*OutString);
}

void UResScannerProxy::SetScannerConfig(FScannerConfig InConfig)
{
	if(!ScannerConfig.IsValid())
	{
		ScannerConfig = MakeShareable(new FScannerConfig);
	}
	*ScannerConfig = InConfig;
}
