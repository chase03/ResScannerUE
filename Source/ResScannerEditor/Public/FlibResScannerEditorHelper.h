// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Widgets/Notifications/SNotificationList.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FlibResScannerEditorHelper.generated.h"

/**
 * 
 */
UCLASS()
class RESSCANNEREDITOR_API UFlibResScannerEditorHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	static	TArray<FString> OpenFileDialog();
	static TArray<FString> SaveFileDialog();
};
