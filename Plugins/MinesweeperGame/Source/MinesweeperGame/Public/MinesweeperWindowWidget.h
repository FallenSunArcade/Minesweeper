// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"

/**
 * 
 */
class MINESWEEPERGAME_API SMinesweeperWindowWidget : public SWindow
{
public:
	SLATE_BEGIN_ARGS(SMinesweeperWindowWidget)
		{
		}

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);
};
