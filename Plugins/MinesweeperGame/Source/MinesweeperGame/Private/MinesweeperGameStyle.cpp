// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperGameStyle.h"
#include "MinesweeperGame.h"
#include "Framework/Application/SlateApplication.h"
#include "Styling/SlateStyleRegistry.h"
#include "Slate/SlateGameResources.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateStyleMacros.h"

#define RootToContentDir Style->RootToContentDir

TSharedPtr<FSlateStyleSet> FMinesweeperGameStyle::StyleInstance = nullptr;

void FMinesweeperGameStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FMinesweeperGameStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FMinesweeperGameStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("MinesweeperGameStyle"));
	return StyleSetName;
}


const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);

TSharedRef< FSlateStyleSet > FMinesweeperGameStyle::Create()
{
	TSharedRef< FSlateStyleSet > Style = MakeShareable(new FSlateStyleSet("MinesweeperGameStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("MinesweeperGame")->GetBaseDir() / TEXT("Resources"));

	Style->Set("MinesweeperGame.PluginAction", new IMAGE_BRUSH(TEXT("T_Minesweeper"), Icon20x20));
	return Style;
}

void FMinesweeperGameStyle::ReloadTextures()
{
	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FMinesweeperGameStyle::Get()
{
	return *StyleInstance;
}
