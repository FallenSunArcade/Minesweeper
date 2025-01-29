// Copyright Epic Games, Inc. All Rights Reserved.

#include "MinesweeperGame.h"

#include "MinesweeperBoardWidget.h"
#include "MinesweeperEditorSubsystem.h"
#include "MinesweeperGameStyle.h"
#include "MinesweeperGameCommands.h"
#include "Misc/MessageDialog.h"
#include "ToolMenus.h"

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
        .ClientSize(FVector2D(800, 400))
        .SupportsMaximize(false)
        .SupportsMinimize(false);
	
    MinesweeperWindow->SetContent(
        SNew(SBorder)
        .Padding(10)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
	        .HAlign(HAlign_Center)
	        .VAlign(VAlign_Center)
            .Padding(5)
            [
                SAssignNew(MinesweeperBoard, SMinesweeperBoardWidget)
            ]
            + SVerticalBox::Slot()
            .HAlign(HAlign_Center)
            .VAlign(VAlign_Bottom)
            .Padding(5)
            [
                SNew(SEditableTextBox)
                .HintText(FText::FromString("Type something here..."))
                .OnTextCommitted_Lambda([](const FText& NewText, ETextCommit::Type CommitType) {
                    if (CommitType == ETextCommit::OnEnter)
                    {
                    	if(UMinesweeperEditorSubsystem* EditorSubsystem = GEditor->GetEditorSubsystem<UMinesweeperEditorSubsystem>())
                    	{
                    		EditorSubsystem->SendMessageToLLM(NewText.ToString());
                    	}
                        UE_LOG(LogTemp, Log, TEXT("User pressed Enter: %s"), *NewText.ToString());
                    }
                })
            ]
        ]
    );

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
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FMinesweeperGameCommands::Get().PluginAction));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FMinesweeperGameModule, MinesweeperGame)