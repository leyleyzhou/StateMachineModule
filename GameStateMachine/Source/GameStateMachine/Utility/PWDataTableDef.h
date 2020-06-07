#pragma once
#include "UnrealString.h"

const FString PWDTDir_Root = TEXT("/Game/DataTables");
const FString PWDTDir_StandaloneRoot = TEXT("/Game/StandalonePackage/DataTables");
const FString PWDTDir_Common = PWDTDir_Root + TEXT("/Tables");
const FString PWDTDir_DHDT = PWDTDir_Root + TEXT("/DHDT");

//config DataTable name
const FString PWDTName_TeamInfo = TEXT("TeamInfo");
const FString PWDTName_InteractConfig = TEXT("InteractConfig");

//--zly--Start--2018.04.28,For Temporary,it will be deprecated few days later
const FString PWDTDir_Equipment = PWDTDir_DHDT + TEXT("/Equipment");
//--zly--End--2018.04.28,For Temporary,it will be deprecated few days later.
