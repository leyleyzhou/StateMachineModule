// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;
using System;
using System.IO;

public class DETNet : ModuleRules
{
    public DETNet(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
        bEnableShadowVariableWarnings = false;
        bEnableExceptions = true;
        bUseRTTI = true;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
                "CoreUObject",
                "Engine",
                "Json",
                "JsonUtilities",
                "WebSockets",
                "Sockets",
                "Networking",
                "OnlineSubsystem",
                "Http",
                "BasePlugin",
                "GameLiftServerSDK"
            }
        );
    }

}
