#include "TostEngineTrellis2EditorModule.h"
#include "HttpServerModule.h"
#include "CoreMinimal.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "STostEngineTrellis2Window.h"

#define LOCTEXT_NAMESPACE "FTostEngineTrellis2EditorModule"

const FName FTostEngineTrellis2EditorModule::TostEngineTrellis2TabName(TEXT("TostEngineTrellis2"));

void FTostEngineTrellis2EditorCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "TostEngine TRELLIS 2", "Bring up TostEngine TRELLIS 2 window", EUserInterfaceActionType::Button, FInputChord());
}

void FTostEngineTrellis2EditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FTostEngineTrellis2EditorCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTostEngineTrellis2EditorCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTostEngineTrellis2EditorModule::PluginButtonClicked));

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("WindowLayout", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FTostEngineTrellis2EditorModule::AddMenuExtension));

		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Settings", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FTostEngineTrellis2EditorModule::AddToolbarExtension));

		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TostEngineTrellis2TabName, FOnSpawnTab::CreateRaw(this, &FTostEngineTrellis2EditorModule::OnSpawnPluginTab), FCanSpawnTab())
		.SetDisplayName(LOCTEXT("FTostEngineTrellis2TabTitle", "TostEngine TRELLIS 2"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	// Start the HTTP server module
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	HttpServerModule.StartAllListeners();
}

void FTostEngineTrellis2EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TostEngineTrellis2TabName);

	// Stop the HTTP server module
	FHttpServerModule& HttpServerModule = FHttpServerModule::Get();
	HttpServerModule.StopAllListeners();
}

TSharedRef<SDockTab> FTostEngineTrellis2EditorModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(STostEngineTrellis2Window)
		];
}

void FTostEngineTrellis2EditorModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TostEngineTrellis2TabName);
}

void FTostEngineTrellis2EditorModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FTostEngineTrellis2EditorCommands::Get().OpenPluginWindow);
}

void FTostEngineTrellis2EditorModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FTostEngineTrellis2EditorCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTostEngineTrellis2EditorModule, TostEngineTrellis2Editor)
