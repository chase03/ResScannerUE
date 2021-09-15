// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "FMatchRuleTypes.h"
// engine header
#include "Interfaces/ITargetPlatformManagerModule.h"
#include "Interfaces/ITargetPlatform.h"
#include "Templates/SharedPointer.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Text/SMultiLineEditableText.h"
#include "IStructureDetailsView.h"
#include "SResScannerContents.h"

/**
 * Implements the cooked platforms panel.
 */
class SResScannerConfigPage
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SResScannerConfigPage) { }
	SLATE_END_ARGS()

public:

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(	const FArguments& InArgs);

// IPatchableInterface
public:
	virtual void ImportConfig();
	virtual void ExportConfig()const;
	virtual void ResetConfig();
	virtual void DoScanWork()const;
	void CreateExportFilterListView();
	FReply DoScan() const;

protected:
	FReply DoExportConfig()const;
	FReply DoImportConfig()const;
	FReply DoResetConfig()const;
private:

	/** Settings view ui element ptr */
	TSharedPtr<IStructureDetailsView> SettingsView;
	TSharedPtr<SResScannerContents> ContentsWidget;
	TSharedPtr<FScannerConfig> ScannerConfig;
};

