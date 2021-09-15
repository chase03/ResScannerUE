#include "ResScannerProxy.h"

DEFINE_LOG_CATEGORY(LogResScannerProxy);
#define LOCTEXT_NAMESPACE "UResScannerProxy"

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
		if(!ScannerRule.ScanFilters.Num())
		{
			UE_LOG(LogResScannerProxy,Warning,TEXT("rule %s not contain any filters!"),*ScannerRule.RuleName);
			continue;
		}
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
	FString Name = GetScannerConfig()->ConfigName;
	if(Name.IsEmpty())
	{
		Name = FDateTime::UtcNow().ToString();
	}
	if(GetScannerConfig()->bSaveConfig)
	{
		FString SerializedJsonStr;
		TemplateHelper::TSerializeStructAsJsonString(*ScannerConfig,SerializedJsonStr);
		FString SavePath = FPaths::Combine(FPaths::ConvertRelativePathToFull(GetScannerConfig()->SavePath.Path),FString::Printf(TEXT("%s_config.json"),*Name));
		if(FFileHelper::SaveStringToFile(SerializedJsonStr, *SavePath) && !IsRunningCommandlet())
		{
			FText Msg = LOCTEXT("SavedScanConfigMag", "Successd to Export the Config.");
			UFlibAssetParseHelper::CreateSaveFileNotify(Msg,SavePath,SNotificationItem::CS_Success);
		}
	}
	if(GetScannerConfig()->bSaveResult)
	{
		FString SerializedJsonStr;
		TemplateHelper::TSerializeStructAsJsonString(MatchedResult,SerializedJsonStr);
		FString SavePath = FPaths::Combine(FPaths::ConvertRelativePathToFull(GetScannerConfig()->SavePath.Path),FString::Printf(TEXT("%s_result.json"),*Name));
		if(FFileHelper::SaveStringToFile(SerializedJsonStr, *SavePath) && !IsRunningCommandlet())
		{
			FText Msg = LOCTEXT("SavedScanResultMag", "Successd to Export the scan result.");
			UFlibAssetParseHelper::CreateSaveFileNotify(Msg,SavePath,SNotificationItem::CS_Success);
		}
	}
}

void UResScannerProxy::SetScannerConfig(FScannerConfig InConfig)
{
	if(!ScannerConfig.IsValid())
	{
		ScannerConfig = MakeShareable(new FScannerConfig);
	}
	*ScannerConfig = InConfig;
}
#undef LOCTEXT_NAMESPACE