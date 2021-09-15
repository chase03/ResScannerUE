#pragma once 

#include "CoreMinimal.h"

#include "Templates/SharedPointer.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Widgets/Text/SMultiLineEditableText.h"


class SResScannerContents
	: public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SResScannerContents) { }
	SLATE_END_ARGS()

public:

	/**
	 * Constructs the widget.
	 *
	 * @param InArgs The Slate argument list.
	 */
	void Construct(	const FArguments& InArgs);

public:

	void SetExpanded(bool InExpand);

	void SetContent(const FString& InContent);

private:

	TSharedPtr<SExpandableArea> ContentsWidget;
	TSharedPtr<SMultiLineEditableText> MulitText;
};