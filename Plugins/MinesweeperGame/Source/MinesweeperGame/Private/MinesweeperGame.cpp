// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperGame.h"

#include "ImageUtils.h"
#include "MinesweeperBoardWidget.h"
#include "MinesweeperEditorSubsystem.h"
#include "MinesweeperGameStyle.h"
#include "MinesweeperGameCommands.h"
#include "MinesweeperGameWidget.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"
#include "Styling/SlateStyleRegistry.h"

static const FName MinesweeperGameTabName("MinesweeperGame");

#define LOCTEXT_NAMESPACE "FMinesweeperGameModule"

void FMinesweeperGameModule::StartupModule()
{
	FMinesweeperGameStyle::Initialize();
	FMinesweeperGameStyle::ReloadTextures();

	FMinesweeperGameCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FMinesweeperGameCommands::Get().PluginAction,
		FExecuteAction::CreateRaw(this, &FMinesweeperGameModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FMinesweeperGameModule::RegisterMenus));
}

void FMinesweeperGameModule::ShutdownModule()
{
	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FMinesweeperGameStyle::Shutdown();

	FMinesweeperGameCommands::Unregister();
}

void FMinesweeperGameModule::PluginButtonClicked()
{
	TSharedRef<SWindow> MinesweeperWindow = SNew(SWindow)
		.Title(FText::FromString("Minesweeper"))
		.ClientSize({500.f, 500.f})
		.SupportsMaximize(false)
		.SupportsMinimize(false);
	
	MinesweeperWindow->SetContent(SNew(SMinesweeperGameWidget));
	
	FSlateApplication::Get().AddWindow(MinesweeperWindow);
}

void FMinesweeperGameModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FMinesweeperGameCommands::Get().PluginAction, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("PluginTools");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(
					FMinesweeperGameCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMinesweeperGameModule, MinesweeperGame)