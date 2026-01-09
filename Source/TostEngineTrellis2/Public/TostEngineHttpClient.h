#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"
#include "Interfaces/IHttpRequest.h"
#include "TostEngineHttpClient.generated.h"

DECLARE_DELEGATE_OneParam(FOnAIRequestCompleted, const FString& /*Response*/);
DECLARE_DELEGATE_OneParam(FOnAIRequestFailed, const FString& /*Error*/);
DECLARE_DELEGATE_OneParam(FOnUploadCompleted, const FString& /*Url*/);
DECLARE_DELEGATE_OneParam(FOnUploadFailed, const FString& /*Error*/);

UCLASS()
class TOSTENGINETRELLIS2_API UTostEngineHttpClient : public UObject
{
	GENERATED_BODY()

public:
	UTostEngineHttpClient();

	void CallAIService(const FString& WorkerId, const FString& WebhookUrl, const TSharedPtr<FJsonObject>& InputData, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, FOnAIRequestCompleted OnCompleted, FOnAIRequestFailed OnFailed);

	void UploadImage(const TArray<uint8>& ImageData, const FString& FileName, bool bUseLocal, const FString& LocalUploadUrl, FOnUploadCompleted OnCompleted, FOnUploadFailed OnFailed);

private:
	void OnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
	void OnUploadRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	FOnAIRequestCompleted CurrentOnCompleted;
	FOnAIRequestFailed CurrentOnFailed;
	FOnUploadCompleted CurrentUploadOnCompleted;
	FOnUploadFailed CurrentUploadOnFailed;
};
