#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "TostEngineServices.h"
#include "Misc/DateTime.h"
#include "TostEngineWebhookServer.h"
#include "TostEngineTrellis2Functions.generated.h"

// Delegate declarations for async callbacks
DECLARE_DELEGATE_ThreeParams(FOnJobStatusUpdate, const FString& /*Status*/, float /*DelayTime*/, float /*ExecutionTime*/);
DECLARE_DELEGATE_OneParam(FOnJobCompleted, const FString& /*ResultUrl*/);
DECLARE_DELEGATE_OneParam(FOnJobFailed, const FString& /*Error*/);

// Job status structure matching UI implementation
struct FJobStatus
{
    FString JobId;
    FString Status; // "IN_QUEUE", "IN_PROGRESS", "COMPLETED", "FAILED"
    float DelayTime;
    float ExecutionTime;
    FDateTime StartTime;
    FString ServiceType;

    FJobStatus()
        : DelayTime(0.0f)
        , ExecutionTime(0.0f)
    {}

    FJobStatus(const FString& InJobId, const FString& InServiceType)
        : JobId(InJobId)
        , Status(TEXT("IN_QUEUE"))
        , DelayTime(0.0f)
        , ExecutionTime(0.0f)
        , StartTime(FDateTime::UtcNow())
        , ServiceType(InServiceType)
    {}
};

UCLASS()
class TOSTENGINETRELLIS2EDITOR_API UTostEngineTrellis2Functions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// TRELLIS 2 function with all parameters
	static bool CallTrellis2(
		const FString& JobId,
		const FString& InputImage,
		int32 Seed,
		const FString& Resolution,
		bool bRemoveBackground,
		float SS_GuidanceStrength,
		float SS_GuidanceRescale,
		int32 SS_SamplingSteps,
		float SS_RescaleT,
		float ShapeSlat_GuidanceStrength,
		float ShapeSlat_GuidanceRescale,
		int32 ShapeSlat_SamplingSteps,
		float ShapeSlat_RescaleT,
		float TexSlat_GuidanceStrength,
		float TexSlat_GuidanceRescale,
		int32 TexSlat_SamplingSteps,
		float TexSlat_RescaleT,
		const FString& WebhookUrl,
		const FString& WorkerId,
		const FString& AuthToken,
		bool bUseLocal,
		const FString& LocalApiUrl,
		const FOnJobStatusUpdate& OnStatusUpdate,
		const FOnJobCompleted& OnCompleted,
		const FOnJobFailed& OnFailed,
		FString& OutJobId,
		FString& OutError
	);

	static void StartWebhookServer(int32 Port = 8080);
	static void StopWebhookServer();
	static bool IsWebhookServerRunning();

	static void PollJobStatus(const FString& WorkerId, const FString& JobId, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, const FOnJobStatusUpdate& OnStatusUpdate, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed);
	static void DownloadGLBFile(const FString& GlbUrl, const FString& LocalPath, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed);
	static void ImportGLBToScene(const FString& GlbPath, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed);

private:
	static UPROPERTY()
	UTostEngineWebhookServer* WebhookServer;
	static void HandleWebhookReceived(const FString& Payload);

	static bool ExecuteServiceCall(const FString& ServiceId, const TSharedPtr<FJsonObject>& InputData, const FString& WebhookUrl, const FString& WorkerId, const FString& AuthToken, FString& OutJobId, FString& OutError);
	static bool ExecuteServiceCallAsync(const FString& ServiceId, const TSharedPtr<FJsonObject>& InputData, const FString& WebhookUrl, const FString& WorkerId, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, const FOnJobStatusUpdate& OnStatusUpdate, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed, FString& OutJobId, FString& OutError);
	static void DownloadTxtFileContents(const FString& TxtFileUrl, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed);
};
