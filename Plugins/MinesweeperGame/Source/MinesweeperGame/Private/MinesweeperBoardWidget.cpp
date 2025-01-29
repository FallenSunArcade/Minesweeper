// Fill out your copyright notice in the Description page of Project Settings.


#include "MinesweeperBoardWidget.h"
#include "MinesweeperEditorSubsystem.h"
#include "SlateOptMacros.h"


void SMinesweeperBoardWidget::Construct(const FArguments& InArgs)
{
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
		EditorSubsystem->OnGenerateBoard.BindRaw(this, &SMinesweeperBoardWidget::GenerateBoard);
	}
}

void SMinesweeperBoardWidget::GenerateBoard(int32 NewRows, int32 NewColumns, int32 NewMines)
{
	NewMines = FMath::Clamp(NewMines, 0, NewRows * NewColumns);

	BoardRows = NewRows;
	BoardCols = NewColumns;
	BoardMines = NewMines;

	GridPanel->ClearChildren();
	GridPanel->SetEnabled(true);
	
	MineLocations.Empty();
	CellButtons.Empty();

	StartNewGameButton->SetVisibility(EVisibility::Visible);
	
	AdjacentBombGrid.SetNumZeroed(NewRows); 

	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		AdjacentBombGrid[Row].SetNumZeroed(NewColumns); 
	}

	TArray<FIntPoint> AvailableCells;
	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		for (int32 Column = 0; Column < NewColumns; ++Column)
		{
			AvailableCells.Add(FIntPoint(Row, Column));
		}
	}
	
	for (int32 i = 0; i < NewMines; ++i)
	{
		int32 RandomIndex = FMath::RandRange(0, AvailableCells.Num() - 1);
		MineLocations.Add(AvailableCells[RandomIndex]);
		AvailableCells.RemoveAt(RandomIndex);

		for (int32 Row = -1; Row <= 1; ++Row)
        {
            for (int32 Col = -1; Col <= 1; ++Col)
            {
                int32 NeighborRow = MineLocations[i].X + Row;
            	int32 NeighborColumn = MineLocations[i].Y + Col;

                if (NeighborRow >= 0 && NeighborRow < NewRows && NeighborColumn >= 0 && NeighborColumn < NewColumns && !(Row == 0 && Col == 0))
                {
                    AdjacentBombGrid[NeighborRow][NeighborColumn]++;
                }
            }
        }
	}
	
	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		for (int32 Column = 0; Column < NewColumns; ++Column)
		{
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
		TSharedPtr<SButton> ClickedButton = CellButtons[ClickedCell];
		TSharedPtr<STextBlock> ClickedText = CellTexts[ClickedCell];

		if (ClickedButton.IsValid() && ClickedText.IsValid())
		{
			if (MineLocations.Contains(ClickedCell))
			{
				ClickedButton->SetBorderBackgroundColor(FLinearColor::Red);
				ClickedText->SetText(FText::FromString("B"));
				GridPanel->SetEnabled(false);
			}
			else
			{
				int32 BombNeighbors = AdjacentBombGrid[Row][Column];
				ClickedButton->SetBorderBackgroundColor(FLinearColor::Green);
				ClickedText->SetText(FText::FromString(FString::FromInt(BombNeighbors)));
			}
		}
	}

	return FReply::Handled();
}

FReply SMinesweeperBoardWidget::OnStartNewGame()
{
	GenerateBoard(BoardRows, BoardCols, BoardMines);
	return FReply::Handled();
}


