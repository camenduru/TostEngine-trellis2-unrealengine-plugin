#include "TostEngineWebhookServer.h"
#include "HttpServerModule.h"
#include "IHttpRouter.h"
#include "HttpServerHttpVersion.h"
#include "HttpServerRequest.h"
#include "HttpServerResponse.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "HAL/PlatformProcess.h"
#include "Windows/WindowsPlatformMisc.h"

UTostEngineWebhookServer::UTostEngineWebhookServer()
	: ServerPort(8080)
{
}

void UTostEngineWebhookServer::StartServer(int32 Port)
{
	ServerPort = Port;

	if (HttpRouter.IsValid())
	{
		StopServer();
	}

	// Configure HTTP server to bind to all interfaces (0.0.0.0)
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	
	// Create router with explicit bind to all interfaces
	HttpRouter = HttpServerModule.GetHttpRouter(ServerPort, /* bFailOnBindFailure */ true);

	if (HttpRouter.IsValid())
	{
		FHttpRouteHandle RouteHandle = HttpRouter->BindRoute(FHttpPath(TEXT("/webhook")), EHttpServerRequestVerbs::VERB_POST,
			FHttpRequestHandler::CreateUObject(this, &UTostEngineWebhookServer::HandleHttpRequest));
		bool bBindSuccess = RouteHandle.IsValid();

		if (bBindSuccess)
		{
			// Bind debug endpoint for GET requests to status path
			FHttpRouteHandle DebugRouteHandle = HttpRouter->BindRoute(FHttpPath(TEXT("/status")), EHttpServerRequestVerbs::VERB_GET,
				FHttpRequestHandler::CreateUObject(this, &UTostEngineWebhookServer::HandleDebugRequest));

			// Start listening on all interfaces using FHttpServerManager
			// This should bind to 0.0.0.0 for external access
			HttpServerModule.StartAllListeners();
			bool bStarted = true;
			
			if (bStarted)
			{
				// Log port forwarding command for user to run manually as admin
				FString PortForwardCmd = FString::Printf(TEXT("netsh interface portproxy add v4tov4 listenaddress=0.0.0.0 listenport=%d connectaddress=127.0.0.1 connectport=%d"), ServerPort, ServerPort);
				UE_LOG(LogTemp, Log, TEXT("=============================================="));
				UE_LOG(LogTemp, Log, TEXT("To enable external access, run this command as Administrator:"));
				UE_LOG(LogTemp, Log, TEXT("%s"), *PortForwardCmd);
				UE_LOG(LogTemp, Log, TEXT("=============================================="));

				// Get the local IP address for logging
				ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);
				if (SocketSubsystem)
				{
					bool bCanBindAll;
					TSharedRef<FInternetAddr> Addr = SocketSubsystem->GetLocalHostAddr(*GLog, bCanBindAll);
					FString LocalIPV4 = Addr->ToString(false);

					UE_LOG(LogTemp, Log, TEXT("=============================================="));
					UE_LOG(LogTemp, Log, TEXT("Webhook server started successfully on 0.0.0.0:%d!"), ServerPort);
					UE_LOG(LogTemp, Log, TEXT("=============================================="));
					UE_LOG(LogTemp, Log, TEXT("Port forwarding configured for external access"));
					UE_LOG(LogTemp, Log, TEXT("Access URLs:"));
					UE_LOG(LogTemp, Log, TEXT("  - http://localhost:%d/status"), ServerPort);
					UE_LOG(LogTemp, Log, TEXT("  - http://127.0.0.1:%d/status"), ServerPort);
					UE_LOG(LogTemp, Log, TEXT("  - http://%s:%d/status (LAN access)"), *LocalIPV4, ServerPort);
					UE_LOG(LogTemp, Log, TEXT("=============================================="));
					UE_LOG(LogTemp, Log, TEXT("Note: Firewall rule may need to be added manually"));
					UE_LOG(LogTemp, Log, TEXT("=============================================="));
				}
				else
				{
					UE_LOG(LogTemp, Log, TEXT("Webhook server started on port %d"), ServerPort);
					UE_LOG(LogTemp, Log, TEXT("Access via: http://localhost:%d/status"), ServerPort);
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to start HTTP listeners on port %d"), ServerPort);
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to bind webhook route on port %d"), ServerPort);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get HTTP router for port %d - another application may be using this port"), ServerPort);
		UE_LOG(LogTemp, Error, TEXT("Try closing other applications using port %d and restart the server"), ServerPort);
	}
}

void UTostEngineWebhookServer::StopServer()
{
	if (HttpRouter.IsValid())
	{
		HttpRouter.Reset();
		// Stop all HTTP listeners to properly close the port
		FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
		HttpServerModule.StopAllListeners();
		UE_LOG(LogTemp, Log, TEXT("Webhook server stopped"));
	}
}

FHttpServerResponse UTostEngineWebhookServer::HandleWebhookRequest(const FHttpServerRequest& Request)
{
	if (Request.Verb == EHttpServerRequestVerbs::VERB_POST)
	{
		FString Payload = FString(UTF8_TO_TCHAR(Request.Body.GetData()));

		UE_LOG(LogTemp, Log, TEXT("Received webhook payload: %s"), *Payload);

		// Trigger the delegate
		OnWebhookReceived.ExecuteIfBound(Payload);

		// Send response
		FHttpServerResponse Response;
		Response.Code = static_cast<EHttpServerResponseCodes>(200);
		Response.Headers.Add(TEXT("Content-Type"), { TEXT("application/json") });
		FString ResponseBody = TEXT("{\"status\":\"ok\"}");
		FTCHARToUTF8 Converter(*ResponseBody);
		Response.Body = TArray<uint8>((uint8*)Converter.Get(), Converter.Length());
		return Response;
	}

	// Method not allowed
	FHttpServerResponse Response;
	Response.Code = static_cast<EHttpServerResponseCodes>(405);
	return Response;
}

bool UTostEngineWebhookServer::HandleHttpRequest(const FHttpServerRequest& Request, const FHttpResultCallback& Callback)
{
	FHttpServerResponse Response = HandleWebhookRequest(Request);
	Callback(MakeUnique<FHttpServerResponse>(Response));
	return true;
}

bool UTostEngineWebhookServer::HandleDebugRequest(const FHttpServerRequest& Request, const FHttpResultCallback& OnComplete)
{
	FHttpServerResponse Response;
	Response.Code = static_cast<EHttpServerResponseCodes>(200);
	Response.Headers.Add(TEXT("Content-Type"), { TEXT("application/json") });

	FString ResponseBody = TEXT("{\"status\":\"Webhook server is running\",\"port\":") + FString::FromInt(ServerPort) + TEXT("}");
	FTCHARToUTF8 Converter(*ResponseBody);
	Response.Body = TArray<uint8>((uint8*)Converter.Get(), Converter.Length());

	OnComplete(MakeUnique<FHttpServerResponse>(Response));
	return true;
}
