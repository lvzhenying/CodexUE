#include "CodexAssistantModule.h"

#include "SCodexAssistantWindow.h"

#include "Framework/Docking/TabManager.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FCodexAssistantModule"

namespace
{
    static const FName CodexAssistantTabName("CodexAssistantTab");
}

void FCodexAssistantModule::StartupModule()
{
    FGlobalTabmanager::Get()->RegisterNomadTabSpawner(
        CodexAssistantTabName,
        FOnSpawnTab::CreateRaw(this, &FCodexAssistantModule::SpawnAssistantTab))
        .SetDisplayName(LOCTEXT("CodexAssistantTabTitle", "Codex Assistant"))
        .SetTooltipText(LOCTEXT("CodexAssistantTooltip", "Open Codex assistant window"))
        .SetMenuType(ETabSpawnerMenuType::Hidden);

    UToolMenus::RegisterStartupCallback(
        FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FCodexAssistantModule::RegisterMenus));
}

void FCodexAssistantModule::ShutdownModule()
{
    UToolMenus::UnRegisterStartupCallback(this);
    UToolMenus::UnregisterOwner(this);
    FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(CodexAssistantTabName);
}

TSharedRef<SDockTab> FCodexAssistantModule::SpawnAssistantTab(const FSpawnTabArgs& SpawnTabArgs)
{
    return SNew(SDockTab)
        .TabRole(ETabRole::NomadTab)
        [
            SNew(SCodexAssistantWindow)
        ];
}

void FCodexAssistantModule::OpenAssistantTab()
{
    FGlobalTabmanager::Get()->TryInvokeTab(CodexAssistantTabName);
}

void FCodexAssistantModule::RegisterMenus()
{
    FToolMenuOwnerScoped OwnerScoped(this);

    UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
    FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
    Section.AddMenuEntry(
        "OpenCodexAssistant",
        LOCTEXT("OpenCodexAssistantLabel", "Codex Assistant"),
        LOCTEXT("OpenCodexAssistantTooltip", "Open the Codex assistant panel."),
        FSlateIcon(),
        FUIAction(FExecuteAction::CreateRaw(this, &FCodexAssistantModule::OpenAssistantTab)));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FCodexAssistantModule, CodexAssistant)
