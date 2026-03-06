#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Widgets/SCompoundWidget.h"

class SCodexAssistantWindow : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SCodexAssistantWindow) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArgs);

private:
    FReply OnSendClicked();
    void SendPrompt(const FString& PromptText);
    void HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    FString ExtractAssistantText(const TSharedPtr<class FJsonObject>& RootObject) const;
    void AppendOutput(const FString& Text);

private:
    TSharedPtr<class SMultiLineEditableTextBox> InputTextBox;
    TSharedPtr<class SMultiLineEditableTextBox> OutputTextBox;
};
