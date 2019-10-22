#include "SMMReplicatedObject.h"
#include "Engine/Engine.h"
#include "Engine/NetDriver.h"

USMMRepliactedObject::USMMRepliactedObject(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

bool USMMRepliactedObject::ReplicateSelf(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = false;
	//if (IsRepPropertyDirty(Channel))
	//{
	//	WroteSomething = Channel->ReplicateSubobject(this, *Bunch, *RepFlags);
	//	ClearRepPropertyDirty(Channel);
	//}
	//TODO
	return WroteSomething;
}

int32 USMMRepliactedObject::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject) || !IsSupportedForNetworking())
	{
		return FunctionCallspace::Local;
	}
	check(GetOuter() != nullptr);
	return GetOuter()->GetFunctionCallspace(Function, Stack);
}

bool USMMRepliactedObject::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(GetOuter() != nullptr);

	AActor* Owner = CastChecked<AActor>(GetOuter());

	bool bProcessed = false;

	FWorldContext* const Context = GEngine->GetWorldContextFromWorld(GetWorld());
	if (Context != nullptr)
	{
		for (FNamedNetDriver& Driver : Context->ActiveNetDrivers)
		{
			if (Driver.NetDriver != nullptr && Driver.NetDriver->ShouldReplicateFunction(Owner, Function))
			{
				Driver.NetDriver->ProcessRemoteFunction(Owner, Function, Parameters, OutParms, Stack, this);
				bProcessed = true;
			}
		}
	}

	return bProcessed;
}

bool USMMRepliactedObject::IsSupportedForNetworking() const
{
	return true;
}
