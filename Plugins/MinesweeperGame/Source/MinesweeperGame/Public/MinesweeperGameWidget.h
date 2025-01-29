// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SMinesweeperBoardWidget;
/**
 * 
 */
class MINESWEEPERGAME_API SMinesweeperGameWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperGameWidget) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

protected:
	FReply OnSumitClicked();

private:
	TSharedPtr<SMinesweeperBoardWidget> MinesweeperBoard;

	FText CurrentText;
};
