#pragma once

#include "Modules/ModuleManager.h"

class FTostEngineTrellis2Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
