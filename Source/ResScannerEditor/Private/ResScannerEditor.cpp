// Copyright Epic Games, Inc. All Rights Reserved.

#include "ResScannerEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "LevelEditor.h"
#include "ResScannerCommands.h"
#if ENGINE_MAJOR_VERSION > 4 || ENGINE_MINOR_VERSION>=26
	#define InvokeTab TryInvokeTab
#endif
#include "SResScanner.h"

static const FName ResScannerTabName("ResScanner");

#define LOCTEXT_NAMESPACE "FResScannerEditorModule"

void FResScannerEditorModule::StartupModule()
{
	FResScannerStyle::Initialize();
	FResScannerStyle::ReloadTextures();
	FResScannerCommands::Register();
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	PluginCommands = MakeShareable(new FUICommandList);
	PluginCommands->MapAction(
		FResScannerCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FResScannerEditorModule::PluginButtonClicked),
		FCanExecuteAction());
	
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FResScannerEditorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);

		// settings
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FResScannerEditorModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}
}

void FResScannerEditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

void FResScannerEditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FResScannerCommands::Get().PluginAction);
}

void FResScannerEditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FResScannerCommands::Get().PluginAction);
}


void FResScannerEditorModule::PluginButtonClicked()
{
	if(!DockTab.IsValid())
	{
		FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ResScannerTabName, FOnSpawnTab::CreateRaw(this, &FResScannerEditorModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FResScannerTabTitle", "ResScanner"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
	}
	FGlobalTabmanager::Get()->InvokeTab(ResScannerTabName);
}

TSharedRef<class SDockTab> FResScannerEditorModule::OnSpawnPluginTab(const class FSpawnTabArgs& InSpawnTabArgs)
{
	return SAssignNew(DockTab,SDockTab)
		.TabRole(ETabRole::NomadTab)
		.Label(LOCTEXT("ResScannerTab", "Res Scanner"))
		.ToolTipText(LOCTEXT("ResScannerTabTextToolTip", "Res Scanner"))
		.OnTabClosed(SDockTab::FOnTabClosedCallback::CreateRaw(this,&FResScannerEditorModule::OnTabClosed))
		.Clipping(EWidgetClipping::ClipToBounds)
		[
			SNew(SResScanner)
		];
}

void FResScannerEditorModule::OnTabClosed(TSharedRef<SDockTab> InTab)
{
	DockTab.Reset();
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FResScannerEditorModule, ResScannerEditor)