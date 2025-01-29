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

private:
	TSharedPtr<SGridPanel> GridPanel;
};
