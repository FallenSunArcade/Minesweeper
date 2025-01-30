﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "MinesweeperEditorSubsystem.generated.h"

DECLARE_DELEGATE_OneParam(FOnGenerateBoard, const TArray<TArray<FString>>& Board);

/**
 * 
 */
UCLASS()
class MINESWEEPERGAME_API UMinesweeperEditorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

public:
	void SendMessageToLLM(const FString& InputMessage);

	void OnLLMResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnGenerateBoard OnBoardGenerated;
	
protected:
	bool HasExpectedMines(const TArray<TArray<FString>>& Grid, int32 ExpectedMines);
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	FString CurrentInputMessage;

	int32 CurrentTryCount = 0;
};

