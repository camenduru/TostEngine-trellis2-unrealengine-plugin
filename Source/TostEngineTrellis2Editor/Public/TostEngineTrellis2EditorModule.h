#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Widgets/Docking/SDockTab.h"

class FUICommandList;

class FTostEngineTrellis2EditorCommands : public TCommands<FTostEngineTrellis2EditorCommands>
{
public:
	FTostEngineTrellis2EditorCommands()
		: TCommands<FTostEngineTrellis2EditorCommands>(TEXT("TostEngineTrellis2Editor"), NSLOCTEXT("Contexts", "TostEngineTrellis2Editor", "TostEngineTrellis2Editor Plugin"), NAME_None, FAppStyle::GetAppStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};

class FTostEngineTrellis2EditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	TSharedPtr<FUICommandList> PluginCommands;
	static const FName TostEngineTrellis2TabName;

	void PluginButtonClicked();
	void AddMenuExtension(FMenuBuilder& Builder);
	void AddToolbarExtension(FToolBarBuilder& Builder);
	TSharedRef<SDockTab> OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs);
};