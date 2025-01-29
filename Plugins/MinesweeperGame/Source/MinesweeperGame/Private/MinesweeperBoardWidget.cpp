// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperBoardWidget.h"

#include "MinesweeperEditorSubsystem.h"
#include "SlateOptMacros.h"


void SMinesweeperBoardWidget::Construct(const FArguments& InArgs)
{
	ChildSlot
 [
 	SAssignNew(GridPanel, SGridPanel)
 ];

	if(UMinesweeperEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UMinesweeperEditorSubsystem>())
	{
		EditorSubsystem->OnGenerateBoard.BindRaw(this, &SMinesweeperBoardWidget::GenerateBoard);
	}
}

void SMinesweeperBoardWidget::GenerateBoard(int32 NewRows, int32 NewColumns, int32 NewMines)
{
	NewMines = FMath::Clamp(NewMines, 0, NewRows * NewColumns);

	GridPanel->ClearChildren();
	
	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		for (int32 Column = 0; Column < NewColumns; ++Column)
		{
			TSharedPtr<SButton> CellButton;

			GridPanel->AddSlot(Column, Row)
			[
				SAssignNew(CellButton, SButton)
				[
					SNew(STextBlock)
					.Text(FText::FromString("")) 
				]
			];
		}
	}
}

