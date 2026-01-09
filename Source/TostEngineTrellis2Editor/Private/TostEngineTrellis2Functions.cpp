#include "TostEngineTrellis2Functions.h"
#include "TostEngineHttpClient.h"
#include "TostEngineWebhookServer.h"
#include "HttpModule.h"
#include "HttpServerModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/World.h"
#include "HAL/PlatformProcess.h"
#include "HAL/FileManager.h"
#include "AssetToolsModule.h"
#include "AssetImportTask.h"
#include "Editor.h"
#include "LevelEditor.h"
#include "Engine/StaticMeshActor.h"
#include "Misc/FileHelper.h"
#include <atomic>

bool UTostEngineTrellis2Functions::CallTrellis2(
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
	FString& OutError)
{
	TSharedPtr<FJsonObject> InputData = MakeShareable(new FJsonObject());

	// Create the nested "input" object
	TSharedPtr<FJsonObject> InputObject = MakeShareable(new FJsonObject());
	if (!JobId.IsEmpty()) InputObject->SetStringField(TEXT("job_id"), JobId);
	InputObject->SetStringField(TEXT("input_image"), InputImage);
	InputObject->SetNumberField(TEXT("seed"), Seed);
	InputObject->SetStringField(TEXT("resolution"), Resolution);
	InputObject->SetBoolField(TEXT("remove_bg"), bRemoveBackground);
	InputObject->SetNumberField(TEXT("ss_guidance_strength"), SS_GuidanceStrength);
	InputObject->SetNumberField(TEXT("ss_guidance_rescale"), SS_GuidanceRescale);
	InputObject->SetNumberField(TEXT("ss_sampling_steps"), SS_SamplingSteps);
	InputObject->SetNumberField(TEXT("ss_rescale_t"), SS_RescaleT);
	InputObject->SetNumberField(TEXT("shape_slat_guidance_strength"), ShapeSlat_GuidanceStrength);
	InputObject->SetNumberField(TEXT("shape_slat_guidance_rescale"), ShapeSlat_GuidanceRescale);
	InputObject->SetNumberField(TEXT("shape_slat_sampling_steps"), ShapeSlat_SamplingSteps);
	InputObject->SetNumberField(TEXT("shape_slat_rescale_t"), ShapeSlat_RescaleT);
	InputObject->SetNumberField(TEXT("tex_slat_guidance_strength"), TexSlat_GuidanceStrength);
	InputObject->SetNumberField(TEXT("tex_slat_guidance_rescale"), TexSlat_GuidanceRescale);
	InputObject->SetNumberField(TEXT("tex_slat_sampling_steps"), TexSlat_SamplingSteps);
	InputObject->SetNumberField(TEXT("tex_slat_rescale_t"), TexSlat_RescaleT);

	// Set the input object and webhook
	InputData->SetObjectField(TEXT("input"), InputObject);
	if (!WebhookUrl.IsEmpty())
	{
		InputData->SetStringField(TEXT("webhook"), WebhookUrl);
	}

	return ExecuteServiceCallAsync(TEXT("trellis-2"), InputData, WebhookUrl, WorkerId, AuthToken, bUseLocal, LocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed, OutJobId, OutError);
}

bool UTostEngineTrellis2Functions::ExecuteServiceCall(const FString& ServiceId, const TSharedPtr<FJsonObject>& InputData, const FString& WebhookUrl, const FString& WorkerId, const FString& AuthToken, FString& OutJobId, FString& OutError)
{
	TArray<FTostEngineService> Services = UTostEngineServices::GetAIServices();
	const FTostEngineService* Service = Services.FindByPredicate([&ServiceId](const FTostEngineService& S) { return S.Id == ServiceId; });

	if (!Service)
	{
		OutError = FString::Printf(TEXT("Service %s not found"), *ServiceId);
		return false;
	}

	UTostEngineHttpClient* HttpClient = NewObject<UTostEngineHttpClient>();

	OutJobId = UTostEngineServices::GenerateJobId();

	const TSharedPtr<FJsonObject>* InputObject;
	if (InputData->TryGetObjectField(TEXT("input"), InputObject) && InputObject)
	{
		if (!(*InputObject)->HasField(TEXT("job_id")))
		{
			(*InputObject)->SetStringField(TEXT("job_id"), OutJobId);
		}
	}

	FOnAIRequestCompleted OnCompleted = FOnAIRequestCompleted::CreateLambda([OutJobId](const FString& Response) {
		UE_LOG(LogTemp, Log, TEXT("AI Service request sent successfully for job %s"), *OutJobId);
	});

	FOnAIRequestFailed OnFailed = FOnAIRequestFailed::CreateLambda([OutError](const FString& Error) mutable {
		OutError = Error;
		UE_LOG(LogTemp, Error, TEXT("AI Service request failed: %s"), *Error);
	});

	HttpClient->CallAIService(WorkerId, WebhookUrl, InputData, AuthToken, false, TEXT(""), OnCompleted, OnFailed);

	return true;
}

bool UTostEngineTrellis2Functions::ExecuteServiceCallAsync(const FString& ServiceId, const TSharedPtr<FJsonObject>& InputData, const FString& WebhookUrl, const FString& WorkerId, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, const FOnJobStatusUpdate& OnStatusUpdate, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed, FString& OutJobId, FString& OutError)
{
	TArray<FTostEngineService> Services = UTostEngineServices::GetAIServices();
	const FTostEngineService* Service = Services.FindByPredicate([&ServiceId](const FTostEngineService& S) { return S.Id == ServiceId; });

	if (!Service)
	{
		OutError = FString::Printf(TEXT("Service %s not found"), *ServiceId);
		return false;
	}

	UTostEngineHttpClient* HttpClient = NewObject<UTostEngineHttpClient>();

	FString InitialJobId = UTostEngineServices::GenerateJobId();
	OutJobId = InitialJobId;

	const TSharedPtr<FJsonObject>* InputObject;
	if (InputData->TryGetObjectField(TEXT("input"), InputObject) && InputObject)
	{
		if (!(*InputObject)->HasField(TEXT("job_id")))
		{
			(*InputObject)->SetStringField(TEXT("job_id"), InitialJobId);
		}
	}

	FString CapturedWorkerId = WorkerId;
	FString CapturedAuthToken = AuthToken;
	FString CapturedLocalApiUrl = LocalApiUrl;
	FOnAIRequestCompleted OnRequestCompleted = FOnAIRequestCompleted::CreateLambda([InitialJobId, CapturedWorkerId, CapturedAuthToken, bUseLocal, CapturedLocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed](const FString& Response) {
		UE_LOG(LogTemp, Log, TEXT("AI Service request sent successfully for job %s"), *InitialJobId);

		TSharedPtr<FJsonObject> ResponseJson;
		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response);

		FString ActualJobId = InitialJobId;
		if (FJsonSerializer::Deserialize(Reader, ResponseJson) && ResponseJson.IsValid())
		{
			if (ResponseJson->HasField(TEXT("id")))
			{
				ActualJobId = ResponseJson->GetStringField(TEXT("id"));
				UE_LOG(LogTemp, Log, TEXT("Actual job ID: %s"), *ActualJobId);
			}

			// Check if the job completed immediately
			if (ResponseJson->HasField(TEXT("status")) && ResponseJson->GetStringField(TEXT("status")) == TEXT("COMPLETED"))
			{
				UE_LOG(LogTemp, Log, TEXT("Job completed immediately"));

				// Extract result URL from output
				if (ResponseJson->HasField(TEXT("output")))
				{
					const TSharedPtr<FJsonObject>* OutputObject;
					if (ResponseJson->TryGetObjectField(TEXT("output"), OutputObject) && OutputObject)
					{
						if ((*OutputObject)->HasField(TEXT("result")))
						{
							FString ResultUrl = (*OutputObject)->GetStringField(TEXT("result"));
							UE_LOG(LogTemp, Log, TEXT("Result URL: %s"), *ResultUrl);
							OnCompleted.ExecuteIfBound(ResultUrl);
							return;
						}
					}
				}

				// Fallback
				OnCompleted.ExecuteIfBound(ResponseJson->GetStringField(TEXT("output")));
				return;
			}
		}

		PollJobStatus(CapturedWorkerId, ActualJobId, CapturedAuthToken, bUseLocal, CapturedLocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed);
	});

	FOnAIRequestFailed OnRequestFailed = FOnAIRequestFailed::CreateLambda([OutError, OnFailed](const FString& Error) mutable {
		OutError = Error;
		UE_LOG(LogTemp, Error, TEXT("AI Service request failed: %s"), *Error);
		OnFailed.ExecuteIfBound(Error);
	});

	HttpClient->CallAIService(WorkerId, WebhookUrl, InputData, AuthToken, bUseLocal, LocalApiUrl, OnRequestCompleted, OnRequestFailed);

	return true;
}

void UTostEngineTrellis2Functions::PollJobStatus(const FString& WorkerId, const FString& JobId, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, const FOnJobStatusUpdate& OnStatusUpdate, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed)
{
	UE_LOG(LogTemp, Log, TEXT("=== Starting PollJobStatus ==="));
	UE_LOG(LogTemp, Log, TEXT("WorkerId: %s"), *WorkerId);
	UE_LOG(LogTemp, Log, TEXT("JobId: %s"), *JobId);
	UE_LOG(LogTemp, Log, TEXT("bUseLocal: %s"), bUseLocal ? TEXT("true") : TEXT("false"));

	if (bUseLocal)
	{
		// For local API, send a single status request and do not poll
		FString StatusUrl = FString::Printf(TEXT("%s/status/%s"), *LocalApiUrl, *JobId);
		UE_LOG(LogTemp, Log, TEXT("Local Status URL: %s"), *StatusUrl);

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetURL(StatusUrl);
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetTimeout(300.0f); // 5 minute timeout

		HttpRequest->OnProcessRequestComplete().BindLambda([JobId, OnStatusUpdate, OnCompleted, OnFailed](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			UE_LOG(LogTemp, Log, TEXT("Local status request completed for job %s"), *JobId);
			UE_LOG(LogTemp, Log, TEXT("bWasSuccessful: %s"), bWasSuccessful ? TEXT("true") : TEXT("false"));
			if (bWasSuccessful && Response.IsValid())
			{
				UE_LOG(LogTemp, Log, TEXT("Response code: %d"), Response->GetResponseCode());
				UE_LOG(LogTemp, Log, TEXT("Response content: %s"), *Response->GetContentAsString());
				if (Response->GetResponseCode() == 200)
				{
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

					if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
					{
						FString Status = JsonObject->GetStringField(TEXT("status"));
						UE_LOG(LogTemp, Log, TEXT("Local job status: %s"), *Status);

						if (Status == TEXT("complete"))
						{
							UE_LOG(LogTemp, Log, TEXT("=== LOCAL JOB COMPLETED ==="));

							if (JsonObject->HasField(TEXT("output")))
							{
								FString ResultUrl = JsonObject->GetStringField(TEXT("output"));
								UE_LOG(LogTemp, Log, TEXT("Result URL: %s"), *ResultUrl);

								if (!ResultUrl.IsEmpty())
								{
									if (ResultUrl.EndsWith(TEXT(".txt")))
									{
										DownloadTxtFileContents(ResultUrl, OnCompleted, OnFailed);
										return;
									}
									else
									{
										OnCompleted.ExecuteIfBound(ResultUrl);
										return;
									}
								}
							}
							OnCompleted.ExecuteIfBound(JsonObject->GetStringField(TEXT("output")));
						}
						else if (Status == TEXT("in progress"))
						{
							// For local API, do not poll - just update status
							OnStatusUpdate.ExecuteIfBound(TEXT("IN_PROGRESS"), 0.0f, 0.0f);
							// No further action
						}
						else
						{
							OnFailed.ExecuteIfBound(FString::Printf(TEXT("Unknown status: %s"), *Status));
						}
					}
					else
					{
						OnFailed.ExecuteIfBound(TEXT("Failed to parse status response"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to deserialize JSON for local job %s"), *JobId);
					FString ErrorMessage = FString::Printf(TEXT("HTTP Error: %d - %s"), Response->GetResponseCode(), *Response->GetContentAsString());
					OnFailed.ExecuteIfBound(ErrorMessage);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("HTTP Request failed for local job %s"), *JobId);
				if (Request.IsValid())
				{
					UE_LOG(LogTemp, Error, TEXT("URL: %s"), *Request->GetURL());
				}
				FString ErrorMessage = TEXT("HTTP Request failed");
				if (Request.IsValid())
				{
					ErrorMessage += FString::Printf(TEXT(" - %s"), *Request->GetURL());
				}
				OnFailed.ExecuteIfBound(ErrorMessage);
			}
		});

		if (!HttpRequest->ProcessRequest())
		{
			OnFailed.ExecuteIfBound(TEXT("Failed to process local status request"));
		}
	}
	else
	{
		// Remote API polling
		FString BaseUrl = TEXT("https://api.runpod.ai");
		FString StatusUrl = FString::Printf(TEXT("%s/v2/%s/status/%s"), *BaseUrl, *WorkerId, *JobId);
		UE_LOG(LogTemp, Log, TEXT("Status URL: %s"), *StatusUrl);

		TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
		HttpRequest->SetURL(StatusUrl);
		HttpRequest->SetVerb(TEXT("POST"));
		HttpRequest->SetTimeout(300.0f); // 5 minute timeout
		if (!AuthToken.IsEmpty())
		{
			FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *AuthToken);
			HttpRequest->SetHeader(TEXT("Authorization"), AuthHeader);
		}

		HttpRequest->OnProcessRequestComplete().BindLambda([WorkerId, JobId, AuthToken, bUseLocal, LocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
			if (bWasSuccessful && Response.IsValid())
			{
				if (Response->GetResponseCode() == 200)
				{
					TSharedPtr<FJsonObject> JsonObject;
					TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

					if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
					{
						FString Status = JsonObject->GetStringField(TEXT("status"));

						// Remote status parsing
						float DelayTime = 0.0f;
						if (JsonObject->HasField(TEXT("delayTime")) && !JsonObject->GetField<EJson::Null>(TEXT("delayTime")))
						{
							DelayTime = JsonObject->GetNumberField(TEXT("delayTime"));
						}

						float ExecutionTime = 0.0f;
						if (JsonObject->HasField(TEXT("executionTime")) && !JsonObject->GetField<EJson::Null>(TEXT("executionTime")))
						{
							ExecutionTime = JsonObject->GetNumberField(TEXT("executionTime"));
						}

						OnStatusUpdate.ExecuteIfBound(Status, DelayTime, ExecutionTime);
						UE_LOG(LogTemp, Log, TEXT("Job %s status: %s, delay: %.1fs, exec: %.1fs"), *JobId, *Status, DelayTime / 1000.0f, ExecutionTime / 1000.0f);

						if (Status == TEXT("COMPLETED"))
						{
							UE_LOG(LogTemp, Log, TEXT("=== JOB COMPLETED ==="));

							if (JsonObject->HasField(TEXT("output")))
							{
								const TSharedPtr<FJsonObject>* OutputObject;
								if (JsonObject->TryGetObjectField(TEXT("output"), OutputObject) && OutputObject)
								{
									if ((*OutputObject)->HasField(TEXT("result")))
									{
										const TSharedPtr<FJsonObject>* ResultObject;
										if ((*OutputObject)->TryGetObjectField(TEXT("result"), ResultObject) && ResultObject)
										{
											OnCompleted.ExecuteIfBound((*ResultObject)->GetStringField(TEXT("output")));
											return;
										}
										else
										{
											FString ResultUrl = (*OutputObject)->GetStringField(TEXT("result"));
											UE_LOG(LogTemp, Log, TEXT("Result URL: %s"), *ResultUrl);

											if (!ResultUrl.IsEmpty())
											{
												if (ResultUrl.EndsWith(TEXT(".txt")))
												{
													DownloadTxtFileContents(ResultUrl, OnCompleted, OnFailed);
													return;
												}
												else
												{
													OnCompleted.ExecuteIfBound(ResultUrl);
													return;
												}
											}
										}
									}
								}
							}
							OnCompleted.ExecuteIfBound(JsonObject->GetStringField(TEXT("output")));
						}
						else if (Status == TEXT("FAILED"))
						{
							OnFailed.ExecuteIfBound(TEXT("Job failed"));
						}
						else
						{
							FTimerHandle TimerHandle;
							if (GWorld)
							{
								GWorld->GetTimerManager().SetTimer(TimerHandle, [WorkerId, JobId, AuthToken, bUseLocal, LocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed]() {
									PollJobStatus(WorkerId, JobId, AuthToken, bUseLocal, LocalApiUrl, OnStatusUpdate, OnCompleted, OnFailed);
								}, 2.0f, false);
							}
						}
					}
					else
					{
						OnFailed.ExecuteIfBound(TEXT("Failed to parse status response"));
					}
				}
				else
				{
					FString ErrorMessage = FString::Printf(TEXT("HTTP Error: %d - %s"), Response->GetResponseCode(), *Response->GetContentAsString());
					OnFailed.ExecuteIfBound(ErrorMessage);
				}
			}
			else
			{
				FString ErrorMessage = TEXT("HTTP Request failed");
				if (Request.IsValid())
				{
					ErrorMessage += FString::Printf(TEXT(" - %s"), *Request->GetURL());
				}
				OnFailed.ExecuteIfBound(ErrorMessage);
			}
		});

		if (!HttpRequest->ProcessRequest())
		{
			OnFailed.ExecuteIfBound(TEXT("Failed to process status request"));
		}
	}
}

void UTostEngineTrellis2Functions::DownloadTxtFileContents(const FString& TxtFileUrl, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(TxtFileUrl);
	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->OnProcessRequestComplete().BindLambda([TxtFileUrl, OnCompleted, OnFailed](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			if (Response->GetResponseCode() == 200)
			{
				FString FileContents = Response->GetContentAsString();
				OnCompleted.ExecuteIfBound(FileContents);
			}
			else
			{
				FString ErrorMessage = FString::Printf(TEXT("HTTP Error downloading txt file: %d"), Response->GetResponseCode());
				OnFailed.ExecuteIfBound(ErrorMessage);
			}
		}
		else
		{
			FString ErrorMessage = TEXT("Failed to download txt file");
			OnFailed.ExecuteIfBound(ErrorMessage);
		}
	});

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.Execute(TEXT("Failed to process txt file download request"));
	}
}

void UTostEngineTrellis2Functions::DownloadGLBFile(const FString& GlbUrl, const FString& LocalPath, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(GlbUrl);
	HttpRequest->SetVerb(TEXT("GET"));

	HttpRequest->OnProcessRequestComplete().BindLambda([GlbUrl, LocalPath, OnCompleted, OnFailed](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful) {
		if (bWasSuccessful && Response.IsValid())
		{
			if (Response->GetResponseCode() == 200)
			{
				const TArray<uint8>& Content = Response->GetContent();
				if (FFileHelper::SaveArrayToFile(Content, *LocalPath))
				{
					OnCompleted.Execute(LocalPath);
				}
				else
				{
					FString ErrorMessage = TEXT("Failed to save GLB file to disk");
					OnFailed.Execute(ErrorMessage);
				}
			}
			else
			{
				FString ErrorMessage = FString::Printf(TEXT("HTTP Error downloading GLB file: %d"), Response->GetResponseCode());
				OnFailed.Execute(ErrorMessage);
			}
		}
		else
		{
			FString ErrorMessage = TEXT("Failed to download GLB file");
			OnFailed.Execute(ErrorMessage);
		}
	});

	if (!HttpRequest->ProcessRequest())
	{
		OnFailed.Execute(TEXT("Failed to process GLB file download request"));
	}
}


void UTostEngineTrellis2Functions::ImportGLBToScene(const FString& GlbPath, const FOnJobCompleted& OnCompleted, const FOnJobFailed& OnFailed)
{
	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");

	TArray<FString> FilesToImport;
	FilesToImport.Add(GlbPath);

	UAutomatedAssetImportData* ImportData = NewObject<UAutomatedAssetImportData>();
	ImportData->bReplaceExisting = true;
	ImportData->bSkipReadOnly = true;
	ImportData->Filenames = FilesToImport;
	ImportData->DestinationPath = TEXT("/Game/TostEngine/Models");

	TArray<UObject*> ImportedAssets = AssetToolsModule.Get().ImportAssetsAutomated(ImportData);

	if (ImportedAssets.Num() > 0)
	{
		// Find the static mesh
		UStaticMesh* StaticMesh = nullptr;
		for (UObject* Asset : ImportedAssets)
		{
			StaticMesh = Cast<UStaticMesh>(Asset);
			if (StaticMesh)
			{
				break;
			}
		}

		if (StaticMesh)
		{
			// Add to scene
			UWorld* World = GEditor->GetEditorWorldContext().World();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				AStaticMeshActor* MeshActor = World->SpawnActor<AStaticMeshActor>(AStaticMeshActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
				if (MeshActor)
				{
					MeshActor->GetStaticMeshComponent()->SetStaticMesh(StaticMesh);
					MeshActor->SetActorLabel(FPaths::GetBaseFilename(GlbPath));
					OnCompleted.Execute(GlbPath);
					return;
				}
			}
		}
	}

	OnFailed.Execute(TEXT("Failed to import GLB to scene"));
}

// Static member initialization
UTostEngineWebhookServer* UTostEngineTrellis2Functions::WebhookServer = nullptr;

void UTostEngineTrellis2Functions::StartWebhookServer(int32 Port)
{
	// Stop and clean up any existing server first
	if (WebhookServer)
	{
		WebhookServer->StopServer();
		if (WebhookServer->IsRooted())
		{
			WebhookServer->RemoveFromRoot();
		}
		WebhookServer->ConditionalBeginDestroy();
		WebhookServer = nullptr;
	}


	// Create new server instance
	WebhookServer = NewObject<UTostEngineWebhookServer>();
	WebhookServer->AddToRoot();
	WebhookServer->OnWebhookReceived.BindStatic(&UTostEngineTrellis2Functions::HandleWebhookReceived);

	// Start the server
	WebhookServer->StartServer(Port);
}

void UTostEngineTrellis2Functions::StopWebhookServer()
{
	if (WebhookServer)
	{
		WebhookServer->StopServer();
		if (WebhookServer->IsRooted())
		{
			WebhookServer->RemoveFromRoot();
		}
		WebhookServer->ConditionalBeginDestroy();
		WebhookServer = nullptr;
	}

	// Stop the HTTP server module if no other servers are using it
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	if (!UTostEngineTrellis2Functions::IsWebhookServerRunning())
	{
		HttpServerModule.StopAllListeners();
	}
}

bool UTostEngineTrellis2Functions::IsWebhookServerRunning()
{
	return WebhookServer != nullptr;
}

void UTostEngineTrellis2Functions::HandleWebhookReceived(const FString& Payload)
{
	UE_LOG(LogTemp, Log, TEXT("Processing webhook payload: %s"), *Payload);

	// Parse the JSON payload
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Payload);

	if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
	{
		const TSharedPtr<FJsonObject>* OutputObject;
		if (JsonObject->TryGetObjectField(TEXT("output"), OutputObject) && OutputObject)
		{
			FString Status = (*OutputObject)->GetStringField(TEXT("status"));
			UE_LOG(LogTemp, Log, TEXT("Webhook status: %s"), *Status);

			if (Status == TEXT("DONE"))
			{
				FString ResultUrl = (*OutputObject)->GetStringField(TEXT("result"));
				UE_LOG(LogTemp, Log, TEXT("Result URL from webhook: %s"), *ResultUrl);

				if (!ResultUrl.IsEmpty())
				{
					if (ResultUrl.EndsWith(TEXT(".glb")))
					{
						// Download and import the GLB file
						FString TempDir = FPaths::ProjectSavedDir() / TEXT("Temp");
						FString GlbFilename = FPaths::GetCleanFilename(ResultUrl);
						FString DownloadedGlbPath = TempDir / GlbFilename;

						DownloadGLBFile(ResultUrl, DownloadedGlbPath,
							FOnJobCompleted::CreateLambda([](const FString& DownloadedPath) {
								// Import to scene
								ImportGLBToScene(DownloadedPath,
									FOnJobCompleted::CreateLambda([](const FString& ImportedPath) {
										UE_LOG(LogTemp, Log, TEXT("GLB imported to scene from webhook: %s"), *ImportedPath);
									}),
									FOnJobFailed::CreateLambda([](const FString& Error) {
										UE_LOG(LogTemp, Error, TEXT("Failed to import GLB from webhook: %s"), *Error);
									}));
							}),
							FOnJobFailed::CreateLambda([](const FString& Error) {
								UE_LOG(LogTemp, Error, TEXT("Failed to download GLB from webhook: %s"), *Error);
							}));
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse webhook JSON payload"));
	}
}
