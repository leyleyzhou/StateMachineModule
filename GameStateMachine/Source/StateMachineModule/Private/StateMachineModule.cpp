#include "StateMachineModule.h"

DEFINE_LOG_CATEGORY(StateMachineModuleLog);

void FStateMachineModule::StartupModule()
{
	UE_LOG(StateMachineModuleLog, Log, TEXT("StateMachineModule module has started!"));
}

void FStateMachineModule::ShutdownModule()
{
	UE_LOG(StateMachineModuleLog, Log, TEXT("StateMachineModule module has shut down!"));
}

IMPLEMENT_GAME_MODULE(FStateMachineModule, StateMachineModule);