#pragma once

#include "Commandlets/Commandlet.h"
#include "ResScannerCommandlet.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogResScannerCommandlet, Log, All);

UCLASS()
class UResScannerCommandlet :public UCommandlet
{
	GENERATED_BODY()

public:

	virtual int32 Main(const FString& Params)override;
};