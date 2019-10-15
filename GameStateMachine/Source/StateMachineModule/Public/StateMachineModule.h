#pragma once
#include "ModuleInterface.h"
#include "Engine.h"
DECLARE_LOG_CATEGORY_EXTERN(StateMachineModuleLog, All, All);

/**  */
class FStateMachineModule : public IModuleInterface
{
public:
  virtual void StartupModule() override;
  
  virtual void ShutdownModule() override;
};