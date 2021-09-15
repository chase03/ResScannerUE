// Copyright 2019 Lipeng Zha, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "ResScannerStyle.h"

class FResScannerCommands : public TCommands<FResScannerCommands>
{
public:

	FResScannerCommands()
		: TCommands<FResScannerCommands>(TEXT("ResScanner"), NSLOCTEXT("Contexts", "ResScanner", "ResScanner Plugin"), NAME_None, FResScannerStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;

	TSharedPtr<FUICommandInfo> CookSelectedAction;
	TSharedPtr<FUICommandInfo> CookAndPakSelectedAction;
	TSharedPtr<FUICommandInfo> AddToPakSettingsAction;
};

#define LOCTEXT_NAMESPACE "FResScannerCommands"

void FResScannerCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "Res Scanner", "Res Scanner Manager", EUserInterfaceActionType::Button, FInputGesture());
}
#undef LOCTEXT_NAMESPACE