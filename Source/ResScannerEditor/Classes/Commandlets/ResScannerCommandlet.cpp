#include "ResScannerCommandlet.h"
#include "FMatchRuleTypes.h"
#include "FlibAssetParseHelper.h"
#include "TemplateHelper.hpp"
// engine header
#include "CoreMinimal.h"

#include "ResScannerProxy.h"
#include "Misc/FileHelper.h"
#include "Misc/CommandLine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/Paths.h"

#define COOKER_CONFIG_PARAM_NAME TEXT("-config=")

DEFINE_LOG_CATEGORY(LogResScannerCommandlet);

int32 UResScannerCommandlet::Main(const FString& Params)
{
	UE_LOG(LogResScannerCommandlet, Display, TEXT("UResScannerCommandlet::Main"));
	FString config_path;
	bool bStatus = FParse::Value(*Params, *FString(COOKER_CONFIG_PARAM_NAME).ToLower(), config_path);
	if (!bStatus)
	{
		UE_LOG(LogResScannerCommandlet, Error, TEXT("not -config=xxxx.json params."));
		return -1;
	}

	if (!FPaths::FileExists(config_path))
	{
		UE_LOG(LogResScannerCommandlet, Error, TEXT("cofnig file %s not exists."), *config_path);
		return -1;
	}

	FString JsonContent;
	if (FFileHelper::LoadFileToString(JsonContent, *config_path))
	{
		UE_LOG(LogResScannerCommandlet, Display, TEXT("%s"), *JsonContent);
		FScannerConfig ScannerConfig;
		TemplateHelper::TDeserializeJsonStringAsStruct(JsonContent,ScannerConfig);
		UResScannerProxy* ScannerProxy = NewObject<UResScannerProxy>();
		ScannerProxy->Init();
		ScannerProxy->SetScannerConfig(ScannerConfig);
		ScannerProxy->DoScan();
		const FMatchedResult& Result = ScannerProxy->GetScanResult();
		FString OutString;
		TemplateHelper::TSerializeStructAsJsonString(Result,OutString);
		UE_LOG(LogResScannerCommandlet, Error, TEXT("Asset Scan Result:\n%s"), *OutString);
	}
	if(FParse::Param(FCommandLine::Get(), TEXT("wait")))
	{
		system("pause");
	}
	return 0;
}
