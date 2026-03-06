#include "SCodexAssistantWindow.h"

#include "CodexAssistantSettings.h"

#include "Dom/JsonObject.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SMultiLineEditableTextBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SVerticalBox.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SCodexAssistantWindow"

void SCodexAssistantWindow::Construct(const FArguments& InArgs)
{
    ChildSlot
    [
        SNew(SBorder)
        .Padding(8.0f)
        [
            SNew(SVerticalBox)
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 0.f, 0.f, 6.f)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("InputPrompt", "Prompt"))
            ]
            + SVerticalBox::Slot()
            .FillHeight(0.35f)
            [
                SAssignNew(InputTextBox, SMultiLineEditableTextBox)
                .HintText(LOCTEXT("InputHint", "Describe your UE task..."))
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 8.f)
            [
                SNew(SButton)
                .Text(LOCTEXT("SendButton", "Send to Codex"))
                .OnClicked(this, &SCodexAssistantWindow::OnSendClicked)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 6.f)
            [
                SNew(SSeparator)
            ]
            + SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0.f, 0.f, 0.f, 6.f)
            [
                SNew(STextBlock)
                .Text(LOCTEXT("OutputLabel", "Assistant Output"))
            ]
            + SVerticalBox::Slot()
            .FillHeight(0.65f)
            [
                SAssignNew(OutputTextBox, SMultiLineEditableTextBox)
                .IsReadOnly(true)
            ]
        ]
    ];
}

FReply SCodexAssistantWindow::OnSendClicked()
{
    if (!InputTextBox.IsValid())
    {
        return FReply::Handled();
    }

    const FString Prompt = InputTextBox->GetText().ToString().TrimStartAndEnd();
    if (Prompt.IsEmpty())
    {
        AppendOutput(TEXT("[Codex] Please enter a prompt."));
        return FReply::Handled();
    }

    SendPrompt(Prompt);
    return FReply::Handled();
}

void SCodexAssistantWindow::SendPrompt(const FString& PromptText)
{
    const UCodexAssistantSettings* Settings = GetDefault<UCodexAssistantSettings>();
    if (!Settings)
    {
        AppendOutput(TEXT("[Codex] Missing plugin settings."));
        return;
    }

    if (Settings->ApiKey.IsEmpty())
    {
        AppendOutput(TEXT("[Codex] API key is empty. Set it in Project Settings > Plugins > Codex Assistant."));
        return;
    }

    TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
    Root->SetStringField(TEXT("model"), Settings->Model);

    TArray<TSharedPtr<FJsonValue>> InputArray;
    TSharedRef<FJsonObject> UserMessage = MakeShared<FJsonObject>();
    UserMessage->SetStringField(TEXT("role"), TEXT("user"));

    TArray<TSharedPtr<FJsonValue>> ContentArray;
    TSharedRef<FJsonObject> Content = MakeShared<FJsonObject>();
    Content->SetStringField(TEXT("type"), TEXT("input_text"));

    const FString FinalPrompt = Settings->SystemPrompt.IsEmpty()
        ? PromptText
        : FString::Printf(TEXT("%s\n\nUser request:\n%s"), *Settings->SystemPrompt, *PromptText);

    Content->SetStringField(TEXT("text"), FinalPrompt);
    ContentArray.Add(MakeShared<FJsonValueObject>(Content));
    UserMessage->SetArrayField(TEXT("content"), ContentArray);
    InputArray.Add(MakeShared<FJsonValueObject>(UserMessage));
    Root->SetArrayField(TEXT("input"), InputArray);
    Root->SetNumberField(TEXT("max_output_tokens"), Settings->MaxOutputTokens);

    FString Body;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Body);
    FJsonSerializer::Serialize(Root, Writer);

    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = FHttpModule::Get().CreateRequest();
    Request->SetURL(Settings->ApiBaseUrl);
    Request->SetVerb(TEXT("POST"));
    Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    Request->SetHeader(TEXT("Authorization"), FString::Printf(TEXT("Bearer %s"), *Settings->ApiKey));
    Request->SetContentAsString(Body);
    Request->OnProcessRequestComplete().BindSP(this, &SCodexAssistantWindow::HandleResponse);

    AppendOutput(TEXT("[You] ") + PromptText);
    AppendOutput(TEXT("[Codex] Thinking..."));

    Request->ProcessRequest();
}

void SCodexAssistantWindow::HandleResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful || !Response.IsValid())
    {
        AppendOutput(TEXT("[Codex] Request failed. Check network or endpoint."));
        return;
    }

    const FString ResponseBody = Response->GetContentAsString();
    TSharedPtr<FJsonObject> RootObject;
    const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseBody);

    if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
    {
        AppendOutput(TEXT("[Codex] Received non-JSON response:"));
        AppendOutput(ResponseBody);
        return;
    }

    if (Response->GetResponseCode() >= 400)
    {
        AppendOutput(FString::Printf(TEXT("[Codex] HTTP %d"), Response->GetResponseCode()));
        AppendOutput(ResponseBody);
        return;
    }

    const FString AssistantText = ExtractAssistantText(RootObject);
    if (AssistantText.IsEmpty())
    {
        AppendOutput(TEXT("[Codex] No text output in response payload."));
        AppendOutput(ResponseBody);
        return;
    }

    AppendOutput(TEXT("[Codex] ") + AssistantText);
}

FString SCodexAssistantWindow::ExtractAssistantText(const TSharedPtr<FJsonObject>& RootObject) const
{
    FString Flattened;

    const TArray<TSharedPtr<FJsonValue>>* OutputArray = nullptr;
    if (RootObject->TryGetArrayField(TEXT("output"), OutputArray))
    {
        for (const TSharedPtr<FJsonValue>& Item : *OutputArray)
        {
            const TSharedPtr<FJsonObject>* MessageObj = nullptr;
            if (!Item.IsValid() || !Item->TryGetObject(MessageObj) || !MessageObj || !MessageObj->IsValid())
            {
                continue;
            }

            const TArray<TSharedPtr<FJsonValue>>* ContentItems = nullptr;
            if (!(*MessageObj)->TryGetArrayField(TEXT("content"), ContentItems))
            {
                continue;
            }

            for (const TSharedPtr<FJsonValue>& ContentValue : *ContentItems)
            {
                const TSharedPtr<FJsonObject>* ContentObj = nullptr;
                if (!ContentValue.IsValid() || !ContentValue->TryGetObject(ContentObj) || !ContentObj || !ContentObj->IsValid())
                {
                    continue;
                }

                FString Text;
                if ((*ContentObj)->TryGetStringField(TEXT("text"), Text) && !Text.IsEmpty())
                {
                    if (!Flattened.IsEmpty())
                    {
                        Flattened += TEXT("\n");
                    }
                    Flattened += Text;
                }
            }
        }
    }

    if (Flattened.IsEmpty())
    {
        RootObject->TryGetStringField(TEXT("output_text"), Flattened);
    }

    return Flattened;
}

void SCodexAssistantWindow::AppendOutput(const FString& Text)
{
    if (!OutputTextBox.IsValid())
    {
        return;
    }

    FString Existing = OutputTextBox->GetText().ToString();
    if (!Existing.IsEmpty())
    {
        Existing += TEXT("\n\n");
    }
    Existing += Text;

    OutputTextBox->SetText(FText::FromString(Existing));
}

#undef LOCTEXT_NAMESPACE
