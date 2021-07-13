// Copyright 2018-2020 X.D. Network Inc. All Rights Reserved.

using UnrealBuildTool;

public class AtelierEditor: ModuleRules
{
	public AtelierEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        bLegacyPublicIncludePaths = false;
        //OptimizeCode = CodeOptimization.InShippingBuildsOnly;

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"JsonUtilities",
			"Json",
			"NavigationSystem",
			"AIModule",

			"Atelier",
            "AtelierAnimGraph",
            "GraphEditor",
			"AnimationCore",
			"AnimGraphRuntime",
			"BlueprintGraph",
			"Navmesh",
			"UMGEditor",
			"PropertyEditor",
			"CommonLibs",
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"UnrealEd",

			"Slate",
			"SlateCore",
			"EditorStyle",
			"AnimGraph",
			"Paper2D",
			"UMG",
			"slua_unreal",
			"AkAudio",
			"Persona",
			"ApplicationCore",
			"GameProjectGeneration",
			"Projects",
			"PluginBrowser",
			"DesktopWidgets",
			"DesktopPlatform",
            // Skill Editor
            "EdCommonLibs",
			"ActionSkill",
			"ActionSkillEditor",

            // StageEditor
			"Blutility",
			"FurnitureEditor",
			"Json",
			"JsonUtilities",
			"LevelSequence",
			"MovieScene",
			"WorkspaceMenuStructure",
			
			// Quest Editor
			"DlgSystem",
            "DlgSystemEditor",
			"EditorScriptingUtilities",
			"SourceControl",
            "AssetRegistry",

			"SequenceExtensionEditor",

			"ToolMenus",
			"AssetTools",
			"NavigationSystem",
            "CommonLibs",

			"AppFramework",
			"AtelierMeshWidget"
		});

		if ((Target.Platform == UnrealTargetPlatform.Win64) || (Target.Platform == UnrealTargetPlatform.Win32))
		{
			PrivateDependencyModuleNames.AddRange(new string[]
			{
				"UEChecker",
			});
		}

		PublicIncludePaths.AddRange(new string[] { "AtelierEditor/AtelierEditor/Public" } );
        PrivateIncludePaths.AddRange(new string[] { "AtelierEditor/AtelierEditor/Private", "AtelierEditor/AtelierEditor/Utilities" } );

        if (Target.bCompileRecast)
        {
            PrivateDependencyModuleNames.Add("Navmesh");
            PublicDefinitions.Add("WITH_RECAST=1");
        }
	}
}
