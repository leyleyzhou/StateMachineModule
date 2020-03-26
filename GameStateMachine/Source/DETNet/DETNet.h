#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FDETNetModule : public IModuleInterface
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	bool LoadRequiredLibraries();
};