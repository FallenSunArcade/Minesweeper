// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Http.h"
#include "MinesweeperEditorSubsystem.generated.h"

DECLARE_DELEGATE_ThreeParams(FOnGenerateBoard, int32, int32, int32);

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

	FOnGenerateBoard OnGenerateBoard;
	
protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
};

