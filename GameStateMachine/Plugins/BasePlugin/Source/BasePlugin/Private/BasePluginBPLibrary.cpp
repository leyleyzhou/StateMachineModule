// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "BasePluginBPLibrary.h"
#include "BasePlugin.h"

UBasePluginBPLibrary::UBasePluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float UBasePluginBPLibrary::BasePluginSampleFunction(float Param)
{
	return -1;
}

