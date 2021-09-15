#pragma once

#include "ResScannerEditor.h"
#include "CoreMinimal.h"
#include "IDetailsView.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Input/Reply.h"
#include "Widgets/SCompoundWidget.h"
class SResScanner : public SCompoundWidget
{

public:
	SLATE_BEGIN_ARGS(SResScanner)
	{}
	SLATE_END_ARGS()

public:
	/**
	* Construct the widget
	*
	* @param	InArgs			A declaration from which to construct the widget
	*/
	void Construct(const FArguments& InArgs);
};

