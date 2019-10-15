// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class StateMachineModule : ModuleRules
{
    public StateMachineModule(ReadOnlyTargetRules Target) : base(Target)
    {
        //Public module names that this module uses.
        //In case you would like to add various classes that you're going to use in your game
        //you should add the core,coreuobject and engine dependencies.
        //Don't forget to add your project's name as a dependency 
        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "GameStateMachine" });

        //The path for the header files
        PublicIncludePaths.AddRange(new string[] { "StateMachineModule/Public" });

        //The path for the source files
        PrivateIncludePaths.AddRange(new string[] { "StateMachineModule/Private" });
    }
}