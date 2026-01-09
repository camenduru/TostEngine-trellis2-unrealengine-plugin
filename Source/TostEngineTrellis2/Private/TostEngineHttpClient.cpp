#include "TostEngineHttpClient.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/Guid.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

UTostEngineHttpClient::UTostEngineHttpClient()
{
}

void UTostEngineHttpClient::CallAIService(const FString& WorkerId, const FString& WebhookUrl, const TSharedPtr<FJsonObject>& InputData, const FString& AuthToken, bool bUseLocal, const FString& LocalApiUrl, FOnAIRequestCompleted OnCompleted, FOnAIRequestFailed OnFailed)
{
	CurrentOnCompleted = OnCompleted;
	CurrentOnFailed = OnFailed;

	// Create the request URL
	FString BaseUrl = bUseLocal ? LocalApiUrl : TEXT("https://api.runpod.ai");
	FString RequestUrl = bUseLocal ? FString::Printf(TEXT("%s/run"), *BaseUrl) : FString::Printf(TEXT("%s/v2/%s/run"), *BaseUrl, *WorkerId);

	// Serialize input data to JSON string
	FString InputJsonString;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&InputJsonString);
	FJsonSerializer::Serialize(InputData.ToSharedRef(), Writer);

	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(RequestUrl);
	HttpRequest->SetVerb(TEXT("POST"));
	HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	if (!AuthToken.IsEmpty())
	{
		FString AuthHeader = FString::Printf(TEXT("Bearer %s"), *AuthToken);
		HttpRequest->SetHeader(TEXT("Authorization"), AuthHeader);
	}
	HttpRequest->SetContentAsString(InputJsonString);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTostEngineHttpClient::OnHttpRequestCompleted);

	if (!HttpRequest->ProcessRequest())
	{
		CurrentOnFailed.ExecuteIfBound(TEXT("Failed to process request"));
	}
}

void UTostEngineHttpClient::UploadImage(const TArray<uint8>& ImageData, const FString& FileName, bool bUseLocal, const FString& LocalUploadUrl, FOnUploadCompleted OnCompleted, FOnUploadFailed OnFailed)
{
	CurrentUploadOnCompleted = OnCompleted;
	CurrentUploadOnFailed = OnFailed;

	// Generate a unique filename
	FString Extension = FPaths::GetExtension(FileName);
	FString UniqueId = FGuid::NewGuid().ToString(EGuidFormats::DigitsLower);
	FString UniqueFileName = FString::Printf(TEXT("%s.%s"), *UniqueId, *Extension);

	// Determine content type
	FString ContentType = TEXT("application/octet-stream");
	if (Extension.Equals(TEXT("jpg"), ESearchCase::IgnoreCase) || Extension.Equals(TEXT("jpeg"), ESearchCase::IgnoreCase))
	{
		ContentType = TEXT("image/jpeg");
	}
	else if (Extension.Equals(TEXT("png"), ESearchCase::IgnoreCase))
	{
		ContentType = TEXT("image/png");
	}
	else if (Extension.Equals(TEXT("gif"), ESearchCase::IgnoreCase))
	{
		ContentType = TEXT("image/gif");
	}
	else if (Extension.Equals(TEXT("webp"), ESearchCase::IgnoreCase))
	{
		ContentType = TEXT("image/webp");
	}

	FString UploadUrl;
	if (bUseLocal)
	{
		// Upload to local MinIO
		FString BaseUrl = LocalUploadUrl.IsEmpty() ? TEXT("http://localhost:9000") : LocalUploadUrl;
		UploadUrl = FString::Printf(TEXT("%s/tost/%s"), *BaseUrl, *UniqueFileName);
	}
	else
	{
		// Upload to TostAI S3
		FString BaseUrl = TEXT("https://s3.tost.ai/tost");
		UploadUrl = FString::Printf(TEXT("%s/%s"), *BaseUrl, *UniqueFileName);
	}

	// Create HTTP request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->SetURL(UploadUrl);
	HttpRequest->SetVerb(TEXT("PUT"));
	HttpRequest->SetHeader(TEXT("Content-Type"), ContentType);
	HttpRequest->SetContent(ImageData);

	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UTostEngineHttpClient::OnUploadRequestCompleted);

	if (!HttpRequest->ProcessRequest())
	{
		CurrentUploadOnFailed.ExecuteIfBound(TEXT("Failed to process upload request"));
	}
}

void UTostEngineHttpClient::OnUploadRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid() && Response->GetResponseCode() == 200)
	{
		FString Url = Request->GetURL();
		CurrentUploadOnCompleted.ExecuteIfBound(Url);
	}
	else
	{
		FString ErrorMsg = TEXT("Upload failed");
		if (Response.IsValid())
		{
			ErrorMsg += FString::Printf(TEXT(" - Status: %d"), Response->GetResponseCode());
		}
		if (Request.IsValid())
		{
			ErrorMsg += FString::Printf(TEXT(" - URL: %s"), *Request->GetURL());
		}
		CurrentUploadOnFailed.ExecuteIfBound(ErrorMsg);
	}
}

void UTostEngineHttpClient::OnHttpRequestCompleted(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (bWasSuccessful && Response.IsValid())
	{
		FString ResponseContent = Response->GetContentAsString();
		CurrentOnCompleted.ExecuteIfBound(ResponseContent);
	}
	else
	{
		FString ErrorMsg = TEXT("HTTP Request failed");
		if (Request.IsValid())
		{
			ErrorMsg += FString::Printf(TEXT(" - URL: %s"), *Request->GetURL());
		}
		CurrentOnFailed.ExecuteIfBound(ErrorMsg);
	}
}
