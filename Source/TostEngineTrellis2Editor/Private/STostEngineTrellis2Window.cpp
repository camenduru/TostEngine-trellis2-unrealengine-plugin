#include "STostEngineTrellis2Window.h"
#include "TostEngineTrellis2Functions.h"
#include "TostEngineHttpClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SComboBox.h"
#include "Widgets/Input/SSpinBox.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SScrollBox.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "Editor.h"
#include "DesktopPlatformModule.h"
#include "Misc/Paths.h"
#include "Modules/ModuleManager.h"
#include "LevelEditor.h"
#include "Misc/Guid.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Sockets.h"
#include "SocketSubsystem.h"

void STostEngineTrellis2Window::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("TRELLIS 2: Native and Compact Structured Latents for 3D Generation")))
			.Font(FCoreStyle::Get().GetFontStyle("HeadingMedium"))
			.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Generate 3D models from input images using TRELLIS 2 AI model")))
			.AutoWrapText(true)
		]
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SAssignNew(ImagePathTextBox, SEditableTextBox)
				.HintText(FText::FromString(TEXT("Select an input image...")))
				.IsReadOnly(true)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SNew(SButton)
				.Text(FText::FromString(TEXT("Select Image")))
				.OnClicked(this, &STostEngineTrellis2Window::OnSelectImage)
			]
		]
		// Auth Token (only for remote)
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Auth Token: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &STostEngineTrellis2Window::GetAuthToken)
				.OnTextChanged(this, &STostEngineTrellis2Window::SetAuthToken)
				.HintText(FText::FromString(TEXT("Enter your authentication token...")))
				.IsEnabled(this, &STostEngineTrellis2Window::IsRemoteMode)
			]
		]
		// Webhook URL
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Webhook URL: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &STostEngineTrellis2Window::GetWebhookUrl)
				.OnTextChanged(this, &STostEngineTrellis2Window::SetWebhookUrl)
				.HintText(FText::FromString(TEXT("Enter webhook URL for notifications...")))
			]
		]
		// Worker ID
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Worker ID: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &STostEngineTrellis2Window::GetWorkerId)
				.OnTextChanged(this, &STostEngineTrellis2Window::SetWorkerId)
				.HintText(FText::FromString(TEXT("Enter worker ID...")))
				.IsEnabled(this, &STostEngineTrellis2Window::IsRemoteMode)
			]
		]
		// Use Local
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Use Local: ")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(this, &STostEngineTrellis2Window::GetUseLocal)
				.OnCheckStateChanged(this, &STostEngineTrellis2Window::SetUseLocal)
			]
		]
		// Local Upload URL
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Local Upload URL: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &STostEngineTrellis2Window::GetLocalUploadUrl)
				.OnTextChanged(this, &STostEngineTrellis2Window::SetLocalUploadUrl)
				.HintText(FText::FromString(TEXT("http://localhost:9000")))
			]
		]
		// Local API URL
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Local API URL: ")))
			]
			+ SHorizontalBox::Slot()
			.FillWidth(1.0f)
			[
				SNew(SEditableTextBox)
				.Text(this, &STostEngineTrellis2Window::GetLocalApiUrl)
				.OnTextChanged(this, &STostEngineTrellis2Window::SetLocalApiUrl)
				.HintText(FText::FromString(TEXT("http://localhost:8000")))
			]
		]
		// Webhook Server
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Enable Webhook Server: ")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(this, &STostEngineTrellis2Window::GetWebhookServerRunning)
				.OnCheckStateChanged(this, &STostEngineTrellis2Window::SetWebhookServerRunning)
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(10, 0, 0, 0)
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Port:")))
				.ColorAndOpacity(FSlateColor(FLinearColor(0.5f, 0.5f, 0.5f)))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.Padding(5, 0, 0, 0)
			[
				SNew(SSpinBox<int32>)
				.Value(this, &STostEngineTrellis2Window::GetWebhookPort)
				.OnValueChanged(this, &STostEngineTrellis2Window::SetWebhookPort)
				.MinValue(1)
				.MaxValue(65535)
				.MinDesiredWidth(80)
			]
		]
		// Seed
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Seed: ")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SSpinBox<int32>)
				.Value(this, &STostEngineTrellis2Window::GetSeed)
				.OnValueChanged(this, &STostEngineTrellis2Window::SetSeed)
				.MinValue(0)
				.MaxValue(999999)
			]
		]
		// Remove Background
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(STextBlock)
				.Text(FText::FromString(TEXT("Remove Background: ")))
			]
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				SNew(SCheckBox)
				.IsChecked(this, &STostEngineTrellis2Window::GetRemoveBackground)
				.OnCheckStateChanged(this, &STostEngineTrellis2Window::SetRemoveBackground)
			]
		]
		// Generate Button
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		.HAlign(EHorizontalAlignment::HAlign_Center)
		[
			SAssignNew(GenerateButton, SButton)
			.Text(FText::FromString(TEXT("Generate 3D Model")))
			.IsEnabled(this, &STostEngineTrellis2Window::CanGenerate)
			.OnClicked(this, &STostEngineTrellis2Window::OnGenerate)
		]
		// Status
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SAssignNew(StatusTextBlock, STextBlock)
			.Text(FText::FromString(TEXT("Ready")))
		]
		// Progress Bar
		+ SVerticalBox::Slot()
		.Padding(10)
		.AutoHeight()
		[
			SAssignNew(ProgressBar, SProgressBar)
			.Visibility(this, &STostEngineTrellis2Window::GetProgressVisibility)
		]
	];
}

FReply STostEngineTrellis2Window::OnSelectImage()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (DesktopPlatform)
	{
		TArray<FString> OutFiles;
		bool bOpened = DesktopPlatform->OpenFileDialog(
			nullptr,
			TEXT("Select Image"),
			TEXT(""),
			TEXT(""),
			TEXT("Image Files (*.png;*.jpg;*.jpeg)|*.png;*.jpg;*.jpeg|All Files (*.*)|*.*"),
			EFileDialogFlags::None,
			OutFiles
		);

		if (bOpened && OutFiles.Num() > 0)
		{
			SelectedImagePath = OutFiles[0];
			ImagePathTextBox->SetText(FText::FromString(SelectedImagePath));
		}
	}

	return FReply::Handled();
}



FReply STostEngineTrellis2Window::OnGenerate()
{
	if (SelectedImagePath.IsEmpty())
	{
		return FReply::Handled();
	}

	bIsGenerating = true;
	GenerateButton->SetEnabled(false);
	StatusTextBlock->SetText(FText::FromString(TEXT("Uploading image...")));

	// Read file content
	TArray<uint8> FileContent;
	if (!FFileHelper::LoadFileToArray(FileContent, *SelectedImagePath))
	{
		OnGenerationFailed(TEXT("Failed to read image file"));
		return FReply::Handled();
	}

	// Get file extension for filename
	FString Extension = FPaths::GetExtension(SelectedImagePath, false);
	FString FileName = FPaths::GetCleanFilename(SelectedImagePath);

	// Create HttpClient
	UTostEngineHttpClient* HttpClient = NewObject<UTostEngineHttpClient>();

	// Upload image
	HttpClient->UploadImage(
		FileContent,
		FileName,
		bUseLocal,
		LocalUploadUrl,
		FOnUploadCompleted::CreateLambda([this](const FString& UploadUrl) {
			// Upload successful, now call TRELLIS 2
			StatusTextBlock->SetText(FText::FromString(TEXT("Submitting job...")));

			FString JobId;
			FString Error;

			// Use webhook server URL if enabled, otherwise use the entered webhook URL
			FString EffectiveWebhookUrl = WebhookUrl;
			if (bWebhookServerRunning)
			{
				bool bCanBindAll;
				TSharedRef<FInternetAddr> Addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, bCanBindAll);
				FString LocalIP = Addr->ToString(false);
				EffectiveWebhookUrl = FString::Printf(TEXT("http://%s:%d/webhook"), *LocalIP, WebhookPort);
			}

			UTostEngineTrellis2Functions::CallTrellis2(
				TEXT(""),
				UploadUrl,
				Seed,
				Resolution,
				bRemoveBackground,
				SS_GuidanceStrength,
				SS_GuidanceRescale,
				SS_SamplingSteps,
				SS_RescaleT,
				ShapeSlat_GuidanceStrength,
				ShapeSlat_GuidanceRescale,
				ShapeSlat_SamplingSteps,
				ShapeSlat_RescaleT,
				TexSlat_GuidanceStrength,
				TexSlat_GuidanceRescale,
				TexSlat_SamplingSteps,
				TexSlat_RescaleT,
				EffectiveWebhookUrl,
				WorkerId,
				bUseLocal ? TEXT("") : AuthToken,
				bUseLocal,
				LocalApiUrl,
				FOnJobStatusUpdate::CreateSP(this, &STostEngineTrellis2Window::UpdateStatus),
				FOnJobCompleted::CreateSP(this, &STostEngineTrellis2Window::OnGenerationCompleted),
				FOnJobFailed::CreateSP(this, &STostEngineTrellis2Window::OnGenerationFailed),
				JobId,
				Error
			);
		}),
		FOnUploadFailed::CreateSP(this, &STostEngineTrellis2Window::OnGenerationFailed)
	);

	return FReply::Handled();
}

void STostEngineTrellis2Window::UpdateStatus(const FString& Status, float InDelayTime, float InExecutionTime)
{
	DelayTime = InDelayTime;
	ExecutionTime = InExecutionTime;
	JobStatus = Status;

	if (Status == TEXT("IN_QUEUE"))
	{
		StatusTextBlock->SetText(FText::FromString(FString::Printf(TEXT("In Queue... Delay: %.1fs"), DelayTime / 1000.0f)));
	}
	else if (Status == TEXT("IN_PROGRESS"))
	{
		StatusTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Processing... Exec: %.1fs"), ExecutionTime / 1000.0f)));
	}
}

void STostEngineTrellis2Window::OnGenerationCompleted(const FString& ResultUrl)
{
	bIsGenerating = false;
	GenerateButton->SetEnabled(true);
	StatusTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Completed! Result: %s"), *ResultUrl)));
	GeneratedModelUrl = ResultUrl;

	if (ResultUrl.EndsWith(TEXT(".glb")))
	{
		StatusTextBlock->SetText(FText::FromString(TEXT("Downloading GLB file...")));

		FString TempDir = FPaths::ProjectSavedDir() / TEXT("Temp");
		FString GlbFilename = FPaths::GetCleanFilename(ResultUrl);
		FString DownloadedGlbPath = TempDir / GlbFilename;

		UTostEngineTrellis2Functions::DownloadGLBFile(ResultUrl, DownloadedGlbPath,
			FOnJobCompleted::CreateSP(this, &STostEngineTrellis2Window::OnGLBDownloaded),
			FOnJobFailed::CreateSP(this, &STostEngineTrellis2Window::OnGenerationFailed));
	}
}

void STostEngineTrellis2Window::OnGLBDownloaded(const FString& DownloadedPath)
{
	StatusTextBlock->SetText(FText::FromString(TEXT("Importing GLB to scene...")));

	UTostEngineTrellis2Functions::ImportGLBToScene(DownloadedPath,
		FOnJobCompleted::CreateSP(this, &STostEngineTrellis2Window::OnGLBImported),
		FOnJobFailed::CreateSP(this, &STostEngineTrellis2Window::OnGenerationFailed));
}

void STostEngineTrellis2Window::OnGLBImported(const FString& ImportedPath)
{
	StatusTextBlock->SetText(FText::FromString(TEXT("GLB imported to scene successfully!")));
}

void STostEngineTrellis2Window::OnGenerationFailed(const FString& Error)
{
	bIsGenerating = false;
	GenerateButton->SetEnabled(true);
	StatusTextBlock->SetText(FText::FromString(FString::Printf(TEXT("Failed: %s"), *Error)));
}

void STostEngineTrellis2Window::SetWebhookServerRunning(ECheckBoxState InValue)
{
	bool bShouldRun = (InValue == ECheckBoxState::Checked);
	if (bShouldRun != bWebhookServerRunning)
	{
		if (bShouldRun)
		{
			UTostEngineTrellis2Functions::StartWebhookServer(WebhookPort);
			bWebhookServerRunning = true;
		}
		else
		{
			UTostEngineTrellis2Functions::StopWebhookServer();
			bWebhookServerRunning = false;
		}
	}
}
