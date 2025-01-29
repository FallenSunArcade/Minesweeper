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
	
	AdjacentBombGrid.SetNum(NewRows);
	for (int32 Row = 0; Row < NewRows; ++Row)
	{
		AdjacentBombGrid[Row].SetNum(NewColumns);
		for (int32 Col = 0; Col < NewColumns; ++Col)
		{
			AdjacentBombGrid[Row][Col] = 0; 
		}
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
		FIntPoint BombLocation = AvailableCells[RandomIndex];
		MineLocations.Add(BombLocation);
		AvailableCells.RemoveAt(RandomIndex);

		for (int32 Row = -1; Row <= 1; ++Row)
        {
            for (int32 Col = -1; Col <= 1; ++Col)
            {
                int32 NeighborRow = BombLocation.X + Row;
            	int32 NeighborColumn = BombLocation.Y + Col;

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
		if (CellButtons[ClickedCell].IsValid() && CellTexts[ClickedCell].IsValid())
		{
			if (MineLocations.Contains(ClickedCell))
			{
				ShowBombs();
				GridPanel->SetEnabled(false);
			}
			else
			{
				int32 BombNeighbors = AdjacentBombGrid[Row][Column];
				CellButtons[ClickedCell]->SetBorderBackgroundColor(FLinearColor::Green);
				CellTexts[ClickedCell]->SetText(FText::FromString(FString::FromInt(BombNeighbors)));
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


