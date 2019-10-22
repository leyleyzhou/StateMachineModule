#pragma once
#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include "SMMReplicatedObject.generated.h"

UCLASS(DefaultToInstanced)
class STATEMACHINEMODULE_API USMMRepliactedObject :public UObject
{
	GENERATED_BODY()
public:
	USMMRepliactedObject(const FObjectInitializer& ObjectInitializer);

	bool ReplicateSelf(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags);
// --------------------------------------
//	UObject overrides
// --------------------------------------
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
	virtual bool IsSupportedForNetworking() const override;
};
