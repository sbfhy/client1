// Copyright Epic Games, Inc. All Rights Reserved.

using System.IO;
using UnrealBuildTool;

public class ThirdPersonMP : ModuleRules
{
    public ThirdPersonMP(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePaths.Add(ModuleDirectory);
        PrivateIncludePaths.Add(Path.Combine(ModuleDirectory, "../../myproto/include"));

        string libPath = Path.Combine(ModuleDirectory, "../../lib");
        PublicAdditionalLibraries.Add(Path.Combine(libPath, "libprotobuf.lib"));
        PublicAdditionalLibraries.Add(Path.Combine(libPath, "myprotolib.lib"));

        PublicDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "HeadMountedDisplay"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Networking",
            "Sockets",
        });

    }
}

