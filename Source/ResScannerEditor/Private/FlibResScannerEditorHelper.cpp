// Fill out your copyright notice in the Description page of Project Settings.


#include "FlibResScannerEditorHelper.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Widgets/Notifications/SNotificationList.h"
#include "Framework/Notifications/NotificationManager.h"
#define LOCTEXT_NAMESPACE "UFlibResScannerEditorHelper"

TArray<FString> UFlibResScannerEditorHelper::OpenFileDialog()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	TArray<FString> SelectedFiles;
	
	if (DesktopPlatform)
	{
		const bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr,
			LOCTEXT("OpenHotPatchConfigDialog", "Open .json").ToString(),
			FString(TEXT("")),
			TEXT(""),
			TEXT("HotPatcher json (*.json)|*.json"),
			EFileDialogFlags::None,
			SelectedFiles
		);
	}
	return SelectedFiles;
}

TArray<FString> UFlibResScannerEditorHelper::SaveFileDialog()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();

	TArray<FString> SaveFilenames;
	if (DesktopPlatform)
	{
		
		const bool bOpened = DesktopPlatform->SaveFileDialog(
			nullptr,
			LOCTEXT("SvedHotPatcherConfig", "Save .json").ToString(),
			FString(TEXT("")),
			TEXT(""),
			TEXT("HotPatcher json (*.json)|*.json"),
			EFileDialogFlags::None,
			SaveFilenames
		);
	}
	return SaveFilenames;
}

#undef LOCTEXT_NAMESPACE