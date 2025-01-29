// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

/**
 * 
 */
class MINESWEEPERGAME_API SMinesweeperBoardWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperBoardWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	void GenerateBoard(int32 NewRows, int32 NewColumns, int32 NewMines);

	FReply OnCellClicked(int32 Row, int32 Column);

	FReply OnStartNewGame();

	void ShowBombs();

private:
	TSharedPtr<SGridPanel> GridPanel;

	TSharedPtr<SButton> StartNewGameButton;

	TArray<FIntPoint> MineLocations;
	
	TArray<TArray<int32>> AdjacentBombGrid;
	
	TMap<FIntPoint, TSharedPtr<SButton>> CellButtons;
	
	TMap<FIntPoint, TSharedPtr<STextBlock>> CellTexts;

	int32 BoardRows = 4;

	int32 BoardCols = 4;

	int32 BoardMines = 2;
};
