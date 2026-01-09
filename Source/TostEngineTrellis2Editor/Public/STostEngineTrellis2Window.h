#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "TostEngineServices.h"

class STostEngineTrellis2Window : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(STostEngineTrellis2Window)
	{}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

private:
	FString SelectedImagePath;
	FString GeneratedModelUrl;
	FString JobStatus;
	float DelayTime = 0.0f;
	float ExecutionTime = 0.0f;
	bool bIsGenerating = false;

	// Parameters
	FString AuthToken;
	FString WebhookUrl;
	FString WorkerId;
	bool bUseLocal = true;
	FString LocalUploadUrl = "http://localhost:9000";
	FString LocalApiUrl = "http://localhost:8000";
	int32 Seed = 0;
	int32 WebhookPort = 8080;
	bool bWebhookServerRunning = false;
	FString Resolution = "1024";
	bool bRemoveBackground = true;
	float SS_GuidanceStrength = 7.5f;
	float SS_GuidanceRescale = 0.7f;
	int32 SS_SamplingSteps = 12;
	float SS_RescaleT = 5.0f;
	float ShapeSlat_GuidanceStrength = 7.5f;
	float ShapeSlat_GuidanceRescale = 0.5f;
	int32 ShapeSlat_SamplingSteps = 12;
	float ShapeSlat_RescaleT = 3.0f;
	float TexSlat_GuidanceStrength = 1.0f;
	float TexSlat_GuidanceRescale = 0.0f;
	int32 TexSlat_SamplingSteps = 12;
	float TexSlat_RescaleT = 3.0f;

	TSharedPtr<class SEditableTextBox> ImagePathTextBox;
	TSharedPtr<class SButton> GenerateButton;
	TSharedPtr<class STextBlock> StatusTextBlock;
	TSharedPtr<class SProgressBar> ProgressBar;

	FReply OnSelectImage();
	FReply OnGenerate();
	void UpdateStatus(const FString& Status, float InDelayTime, float InExecutionTime);
	void OnGenerationCompleted(const FString& ResultUrl);
	void OnGLBDownloaded(const FString& DownloadedPath);
	void OnGLBImported(const FString& ImportedPath);
	void OnGenerationFailed(const FString& Error);

	// Getters and setters for SLATE bindings
	FText GetAuthToken() const { return FText::FromString(AuthToken); }
	void SetAuthToken(const FText& InValue) { AuthToken = InValue.ToString(); }

	FText GetWebhookUrl() const { return FText::FromString(WebhookUrl); }
	void SetWebhookUrl(const FText& InValue) { WebhookUrl = InValue.ToString(); }

	FText GetWorkerId() const { return FText::FromString(WorkerId); }
	void SetWorkerId(const FText& InValue) { WorkerId = InValue.ToString(); }

	int32 GetSeed() const { return Seed; }
	void SetSeed(int32 InValue) { Seed = InValue; }

	ECheckBoxState GetRemoveBackground() const { return bRemoveBackground ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }
	void SetRemoveBackground(ECheckBoxState InValue) { bRemoveBackground = (InValue == ECheckBoxState::Checked); }

	ECheckBoxState GetUseLocal() const { return bUseLocal ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }
	void SetUseLocal(ECheckBoxState InValue) { bUseLocal = (InValue == ECheckBoxState::Checked); }

	FText GetLocalUploadUrl() const { return FText::FromString(LocalUploadUrl); }
	void SetLocalUploadUrl(const FText& InValue) { LocalUploadUrl = InValue.ToString(); }

	FText GetLocalApiUrl() const { return FText::FromString(LocalApiUrl); }
	void SetLocalApiUrl(const FText& InValue) { LocalApiUrl = InValue.ToString(); }

	EVisibility GetProgressVisibility() const { return bIsGenerating ? EVisibility::Visible : EVisibility::Collapsed; }

	bool CanGenerate() const { return !SelectedImagePath.IsEmpty() && !bIsGenerating; }

	bool IsRemoteMode() const { return !bUseLocal; }

	ECheckBoxState GetWebhookServerRunning() const { return bWebhookServerRunning ? ECheckBoxState::Checked : ECheckBoxState::Unchecked; }
	void SetWebhookServerRunning(ECheckBoxState InValue);

	int32 GetWebhookPort() const { return WebhookPort; }
	void SetWebhookPort(int32 InValue) { WebhookPort = InValue; }
};
