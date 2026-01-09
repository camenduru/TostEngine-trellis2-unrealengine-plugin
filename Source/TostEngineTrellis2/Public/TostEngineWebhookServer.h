#pragma once

#include "CoreMinimal.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "TostEngineWebhookServer.generated.h"

DECLARE_DELEGATE_OneParam(FOnWebhookReceived, const FString& /*Payload*/);

UCLASS()
class TOSTENGINETRELLIS2_API UTostEngineWebhookServer : public UObject
{
	GENERATED_BODY()

public:
	UTostEngineWebhookServer();

	void StartServer(int32 Port = 8080);
	void StopServer();

	FOnWebhookReceived OnWebhookReceived;

private:
	FHttpServerResponse HandleWebhookRequest(const FHttpServerRequest& Request);
	bool HandleHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& Callback);
	bool HandleDebugRequest(const FHttpServerRequest& Request, const FHttpResultCallback& Callback);

	TSharedPtr<IHttpRouter> HttpRouter;
	int32 ServerPort;
};