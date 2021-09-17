// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"


class FResScannerEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	void PluginButtonClicked();
	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& InSpawnTabArgs);
	void OnTabClosed(TSharedRef<SDockTab> InTab);
	void AddMenuExtension(FMenuBuilder& Builder);
	void AddToolbarExtension(FToolBarBuilder& Builder);
private:
	TSharedPtr<class FUICommandList> PluginCommands;
	TSharedPtr<SDockTab> DockTab;
};
