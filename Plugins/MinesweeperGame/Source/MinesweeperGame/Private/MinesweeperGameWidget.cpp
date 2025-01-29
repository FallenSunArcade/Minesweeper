// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperGameWidget.h"
#include "MinesweeperBoardWidget.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "MinesweeperEditorSubsystem.h"

void SMinesweeperGameWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SBorder)
		.Padding(20)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			.Padding(10)
			[
				SAssignNew(MinesweeperBoard, SMinesweeperBoardWidget)
			]
			+ SVerticalBox::Slot()
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Bottom)
			.Padding(10)
			[
				SNew(SEditableTextBox)
				.HintText(FText::FromString("Generate board with LLM..."))
				.OnTextChanged_Lambda([this](const FText& NewText)
				{
					CurrentText = NewText;
				})
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(10)
			[
				SNew(SButton)
				.Text(FText::FromString("Submit"))
				.OnClicked(this, &SMinesweeperGameWidget::OnSumitClicked)
			]
		]
	];
}

FReply SMinesweeperGameWidget::OnSumitClicked()
{
	if (UMinesweeperEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<
			UMinesweeperEditorSubsystem>())
	{
		EditorSubsystem->SendMessageToLLM(CurrentText.ToString());
	}
	return FReply::Handled();
}

