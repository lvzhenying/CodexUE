#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FCodexAssistantModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    void RegisterMenus();
    void OpenAssistantTab();
    TSharedRef<class SDockTab> SpawnAssistantTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
    TSharedPtr<class FUICommandList> PluginCommands;
};
