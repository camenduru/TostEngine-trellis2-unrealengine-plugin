#include "TostEngineTrellis2Module.h"
#include "TostEngineHttpClient.h"
#include "TostEngineServices.h"
#include "TostEngineWebhookServer.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

void FTostEngineTrellis2Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LogTemp, Log, TEXT("TostEngineTrellis2 Module Started"));
	TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("TostEngine_Trellis2_UE"));
	if (Plugin.IsValid())
	{
		FString PluginPath = Plugin->GetBaseDir();
		UE_LOG(LogTemp, Log, TEXT("Plugin Path: %s"), *PluginPath);
	}
}

void FTostEngineTrellis2Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Log, TEXT("TostEngineTrellis2 Module Shutdown"));
}

IMPLEMENT_MODULE(FTostEngineTrellis2Module, TostEngineTrellis2)
