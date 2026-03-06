#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CodexAssistantSettings.generated.h"

UCLASS(Config=EditorPerProjectUserSettings, DefaultConfig, meta=(DisplayName="Codex Assistant"))
class CODEXASSISTANT_API UCodexAssistantSettings : public UDeveloperSettings
{
    GENERATED_BODY()

public:
    virtual FName GetCategoryName() const override { return TEXT("Plugins"); }

    UPROPERTY(Config, EditAnywhere, Category="Connection", meta=(DisplayName="API Base URL"))
    FString ApiBaseUrl = TEXT("https://api.openai.com/v1/responses");

    UPROPERTY(Config, EditAnywhere, Category="Connection", meta=(DisplayName="Model"))
    FString Model = TEXT("gpt-5.2-codex");

    UPROPERTY(Config, EditAnywhere, Category="Connection", meta=(DisplayName="API Key", PasswordField=true))
    FString ApiKey;

    UPROPERTY(Config, EditAnywhere, Category="Prompt")
    FString SystemPrompt = TEXT("You are a helpful Unreal Engine 5 coding assistant.");

    UPROPERTY(Config, EditAnywhere, Category="Prompt", meta=(ClampMin="1", ClampMax="64000"))
    int32 MaxOutputTokens = 1200;
};
