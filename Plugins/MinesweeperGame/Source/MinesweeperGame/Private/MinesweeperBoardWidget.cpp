// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperBoardWidget.h"
#include "MinesweeperEditorSubsystem.h"
#include "SlateOptMacros.h"


void SMinesweeperBoardWidget::Construct(const FArguments& InArgs)
{
	CurrentBoard =
		{
		{"1", "X", "1"},
		{"2", "2", "1"},
		{"X", "1", "0"}
		};
	
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Bottom)
		.Padding(5)
		[
			SAssignNew(StartNewGameButton, SButton)
			.OnClicked(this, &SMinesweeperBoardWidget::OnStartNewGame)
			.Visibility(EVisibility::Visible)
			[
				SNew(STextBlock)
				.Text(FText::FromString("New Game"))
			]
		]
		+ SVerticalBox::Slot()
		.AutoHeight()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(5)
		[
			SAssignNew(GridPanel, SGridPanel)
		]
	]; 

	if(UMinesweeperEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UMinesweeperEditorSubsystem>())
	{
		EditorSubsystem->OnBoardGenerated.BindRaw(this, &SMinesweeperBoardWidget::GenerateBoard);
	}
}

void SMinesweeperBoardWidget::GenerateBoard(const TArray<TArray<FString>>& Board)
{
	CurrentBoard = Board;
	int32 NewRows = Board.Num();
	int32 NewColumns = NewRows > 0 ? Board[0].Num() : 0;
	
	GridPanel->ClearChildren();
	GridPanel->SetEnabled(true);
	
	MineLocations.Empty();
	CellButtons.Empty();
	CellTexts.Empty();

	StartNewGameButton->SetVisibility(EVisibility::Visible);
	
	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		for (int32 Column = 0; Column < NewColumns; ++Column)
		{
			if (Board[Row][Column] == "X")
			{
				MineLocations.Add(FIntPoint(Row, Column));
			}

			TSharedPtr<SButton> CellButton;
			TSharedPtr<STextBlock> CellText;

			GridPanel->AddSlot(Column, Row)
			[
				SNew(SBox)
				.WidthOverride(40.f)
				.HeightOverride(40.f)
				[
					SAssignNew(CellButton, SButton)
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.OnClicked(this, &SMinesweeperBoardWidget::OnCellClicked, Row, Column)
					[
						SAssignNew(CellText, STextBlock)
						.Text(FText::FromString(""))
					]
				]
			];

			CellButtons.Add(FIntPoint(Row, Column), CellButton);
			CellTexts.Add(FIntPoint(Row, Column), CellText);
		}
	}
}

FReply SMinesweeperBoardWidget::OnCellClicked(int32 Row, int32 Column)
{
	FIntPoint ClickedCell(Row, Column);

	if (CellButtons.Contains(ClickedCell) && CellTexts.Contains(ClickedCell))
	{
		if (CellButtons[ClickedCell].IsValid() && CellTexts[ClickedCell].IsValid())
		{
			if (MineLocations.Contains(ClickedCell))
			{
				ShowBombs();
				GridPanel->SetEnabled(false);
			}
			else
			{
				CellButtons[ClickedCell]->SetBorderBackgroundColor(FLinearColor::Green);
				CellTexts[ClickedCell]->SetText(FText::FromString(CurrentBoard[Row][Column]));
			}
		}
	}

	return FReply::Handled();
}

FReply SMinesweeperBoardWidget::OnStartNewGame()
{
	GenerateBoard(CurrentBoard);
	return FReply::Handled();
}

void SMinesweeperBoardWidget::ShowBombs()
{
	for(const auto& BombLocation : MineLocations)
	{
		if (CellButtons.Contains(BombLocation) && CellTexts.Contains(BombLocation))
		{
			if(CellButtons[BombLocation].IsValid() && CellTexts[BombLocation].IsValid())
			{
				CellButtons[BombLocation]->SetBorderBackgroundColor(FLinearColor::Red);
				CellTexts[BombLocation]->SetText(FText::FromString("B"));
			}
		}
	}
}


