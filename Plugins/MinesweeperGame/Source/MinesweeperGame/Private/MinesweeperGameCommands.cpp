// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperGameCommands.h"

#define LOCTEXT_NAMESPACE "FMinesweeperGameModule"

void FMinesweeperGameCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "MinesweeperGame", "Execute MinesweeperGame action", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
