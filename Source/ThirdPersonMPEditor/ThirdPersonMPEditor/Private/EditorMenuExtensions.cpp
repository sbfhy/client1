// Copyright 2018-2021 X.D. Network Inc. All Rights Reserved.

#include "EditorMenuExtensions.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "LevelEditor.h"
#include "Modules/ModuleManager.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "EditorStyle.h"
#include "Editor.h"
#include "Atelier/System/Project.h"
#include "EditorModeManager.h"
#include "EditorUtilities.h"
#include "NavigationSystem.h"
#include "AI/NavDataGenerator.h"
#include "HAL/FileManager.h"
#include "Common/GADataTable.h"
#include "Editor/EditorEngine.h"
#include "Atelier/Utilities/Utilities.h"
#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "ObjectTools.h"
#include "AtelierEditor/Utilities/RecastNavMeshExporter.h"
#include "Misc/UObjectToken.h"
#include "NavMesh/RecastNavMesh.h"
#include "NavMesh/RecastNavMeshGenerator.h"
#include "NavMesh/RecastHelpers.h"
#include "WidgetBlueprint.h"
#include "AtelierEditor/Utilities/ContextMenuExtender.h"
#include "Particles/ParticleSystem.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimComposite.h"
#include "Factories/AnimCompositeFactory.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "ObjectTools.h"
#include "Animation/AnimNotifySort.h"
#include "Animation/AnimNotifyData.h"
#include "FileHelpers.h"
#include "ActionSkillEditorModule.h"
#include "ActionSkillEditor.h"
#include "ActionSkill/SkillEditorMiddleware.h"
#include "GameProjectUtils.h"
#include "Interfaces/IPluginManager.h"
#include "Interfaces/IProjectManager.h"
#include "HAL/PlatformFilemanager.h"
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimNotifies/AnimNotify_PlayParticleEffect.h"
#include "Animation/AnimNotifies/AnimNotifyState_TimedParticleEffect.h"
#include "Animation/AnimSequenceBase.h"
#include "Atelier/Anim/AnimNotifies/AnimNotify_WrappedPlayParticleEffect.h"
#include "Atelier/Anim/AnimNotifies/AnimNotifyState_WrappedTimedParticleEffect.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "AtelierEditor/Utilities/EditorStaticLibrary.h"
#include "FurnitureEditorModule.h"
#include "FurnitureEditor/FurnitureEditorMiddleware.h"
#include "EditorExtensionCustomEditorList.h"
#include "LevelSequence.h"
#include "MovieSceneTimeHelpers.h"
#include "DlgManager.h"
#include "Nodes/Summon.h"
#include "Serialization/Csv/CsvParser.h"
#include "EditorAssetLibrary.h"
#include "Json.h"
#include "Atelier/Utilities/FileLibrary.h"
#include "Settings/ProjectPackagingSettings.h"
#include "SourceControlHelpers.h"
#include "NavCollision.h"
#include "EdCommonGenerate.h"
#include "Nodes/DlgNode.h"
#include "DlgDialogue.h"
#include "EDProgrameCommonChecker.h"
#include "Atelier/Kismet/GABlueprintFunctionLibrary.h"

FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors LevelEditorMenuExtenderDelegate;
FDelegateHandle LevelEditorExtenderDelegateHandle;

#define TRANSFER_COORDINATE 0

class FEditorMenuExtensionImpl
{
private:

	static void OpenWorkshopEditor()
	{
		GLevelEditorModeTools().ActivateMode("Atelier_EM_AccessoriesEditor");
		FGlobalTabmanager::Get()->InvokeTab(AccessoriesEditorTabName);
	}

	static void OpenFurnitureEditor()
	{
		FFurnitureEditorModule& furnitureEditorModule = FModuleManager::LoadModuleChecked<FFurnitureEditorModule>("FurnitureEditor");
		FFurnitureEditorMiddleware::PostCreateEditor(furnitureEditorModule.CreateFurnitureEditor());
	}
	static void OpenMainLevel()
	{
		const FString MAIN_LEVEL = "Empty";
		UEditorStaticLibrary::OpenLevelByName(MAIN_LEVEL);
	}


#pragma region Skill Editor

	static void OpenSkillEditor()
	{
		// open skill editor
		if (!FSkillEditorMiddleware::GetSkillEditor().IsValid())
		{
			IActionSkillEditorModule& ActionSKillEditorModule = FModuleManager::LoadModuleChecked<IActionSkillEditorModule>("ActionSkillEditor");
			FSkillEditorMiddleware::PostCreateEditor(ActionSKillEditorModule.CreateActionSkillEditor());
		}
	}

#pragma endregion


	static void OpenAccessoriesEditor()
	{
		UE_LOG(LogAtelierEditor, Warning, TEXT("Open Accessories Editor"));
		GLevelEditorModeTools().ActivateMode("Atelier_EM_AccessoriesEditor");

		FGlobalTabmanager::Get()->InvokeTab(AccessoriesEditorTabName);
	}

	static void OpenInteractiveEditor()
	{
		UE_LOG(LogAtelierEditor, Warning, TEXT("Open Interactive Editor"));
		GLevelEditorModeTools().ActivateMode("Atelier_EM_InteractiveEditor");

		FGlobalTabmanager::Get()->InvokeTab(InteractiveEditorTabName);
	}

	static void OpenAvatarEditor()
	{
		UE_LOG(LogAtelierEditor, Warning, TEXT("Open Avatar Editor"));
		GLevelEditorModeTools().ActivateMode("Atelier_EM_AvatarEditor");

		FGlobalTabmanager::Get()->InvokeTab(FName("AvatarEditor"));
	}

	static void OpenAudioEditor()
	{
		const FString FileToOpen = FPackageName::LongPackageNameToFilename("/Game/EditorUtil/FXBindingTool/AudioLevel", FPackageName::GetMapPackageExtension());
		FEditorFileUtils::LoadMap(FileToOpen, false, true);
	}

	static void AutoGenParticleSystemRefForLua()
	{
		FString log;
		FEdCommonGenerate::AutoGenParticleSystemRefForLua(log);
	}


	static TArray<TArray<FString>> GenStageMonsterArrayData(FString StageType, TArray<UDlgDialogue*> StageDatas)
	{
		TArray<TArray<const TCHAR*>> CharacterTable;
		FString CharacterCsvFile = FPaths::ProjectContentDir() + TEXT("Table/") + TEXT("Character.csv");
		if (FPaths::FileExists(CharacterCsvFile))
		{
			FString FileContent;
			FFileHelper::LoadFileToString(FileContent, *CharacterCsvFile);
			FCsvParser* CsvFiles = new FCsvParser(FileContent);
			CharacterTable = CsvFiles->GetRows();
		}
		TArray<TArray<FString>> OutputDataArray;
		for (auto Dialogue : StageDatas)
		{
			TArray<int> NormalMonster;
			TArray<int> EliteMonster;
			TArray<int> BossMonster;
			for (auto DlgNode : Dialogue->GetNodes())
			{
				if (DlgNode->IsA<USummon>())
				{
					USummon* SummonNode = Cast<USummon>(DlgNode);
					for (int RowIndex = 2; RowIndex < CharacterTable.Num(); ++RowIndex)
					{
						if (SummonNode->npcid == FCString::Atoi(CharacterTable[RowIndex][0]))
						{
							for (int Count = 0; Count < SummonNode->count; ++Count)
							{
								if (FString(CharacterTable[RowIndex][5]) == TEXT("60"))
									NormalMonster.Push(SummonNode->npcid);
								else if (FString(CharacterTable[RowIndex][5]) == TEXT("70"))
									EliteMonster.Push(SummonNode->npcid);
								else if (FString(CharacterTable[RowIndex][5]) == TEXT("80"))
									BossMonster.Push(SummonNode->npcid);
							}
						}
					}
				}
			}
			FString NormalMonsterText;
			FString EliteMonsterText;
			FString BossMonsterText;
			for (int Index = 0; Index < NormalMonster.Num(); ++Index)
			{
				NormalMonsterText.Append(FString::FromInt(NormalMonster[Index]));
				if (Index + 1 < NormalMonster.Num())
					NormalMonsterText.Append(TEXT(","));
			}
			for (int Index = 0; Index < EliteMonster.Num(); ++Index)
			{
				EliteMonsterText.Append(FString::FromInt(EliteMonster[Index]));
				if (Index + 1 < EliteMonster.Num())
					EliteMonsterText.Append(TEXT(","));
			}
			for (int Index = 0; Index < BossMonster.Num(); ++Index)
			{
				BossMonsterText.Append(FString::FromInt(BossMonster[Index]));
				if (Index + 1 < BossMonster.Num())
					BossMonsterText.Append(TEXT(","));
			}
			TArray<FString> StoryData = { Dialogue->GetName() , StageType, NormalMonsterText, EliteMonsterText , BossMonsterText };
			OutputDataArray.Push(StoryData);
		}
		return OutputDataArray;
	}

	static void ExportStageMonsterCsvData()
	{
		TArray<TArray<FString>> OutputFileInfo;
		FString QuestContent = FPaths::ProjectContentDir();
		TArray<FString> TableHead = { TEXT("关卡ID") ,TEXT("关卡标签") ,TEXT("普通怪id"), TEXT("精英怪id"), TEXT("Boss怪id") };
		OutputFileInfo.Push(TableHead);
		TArray<FString> TableHead_Eng = { TEXT("id") ,TEXT("tag"), TEXT("Monster"),TEXT("Elite"), TEXT("Boss") };
		OutputFileInfo.Push(TableHead_Eng);
		TArray<FString> TableHeadType = { TEXT("number") ,TEXT("string"), TEXT("table"), TEXT("table"), TEXT("table") };
		OutputFileInfo.Push(TableHeadType);

		TArray<UDlgDialogue*> AllMonster = UDlgManager::GetAllDialogueFromPath(TEXT("/Game/Quest/Asset/Fight/Monster"));
		TArray<TArray<FString>> AllMonsterText = GenStageMonsterArrayData(TEXT("Monster"), AllMonster);
		OutputFileInfo.Append(AllMonsterText);

		TArray<UDlgDialogue*> AllElite = UDlgManager::GetAllDialogueFromPath(TEXT("/Game/Quest/Asset/Fight/Elite"));
		TArray<TArray<FString>> AllEliteText = GenStageMonsterArrayData(TEXT("Elite"), AllElite);
		OutputFileInfo.Append(AllEliteText);

		TArray<UDlgDialogue*> AllBoss = UDlgManager::GetAllDialogueFromPath(TEXT("/Game/Quest/Asset/Fight/Boss"));
		TArray<TArray<FString>> AllBossText = GenStageMonsterArrayData(TEXT("Boss"), AllBoss);
		OutputFileInfo.Append(AllBossText);

		TArray<UDlgDialogue*> AllStory = UDlgManager::GetAllDialogueFromPath(TEXT("/Game/Quest/Asset/Story"));
		TArray<TArray<FString>> AllStoryText = GenStageMonsterArrayData(TEXT("Story"), AllStory);
		OutputFileInfo.Append(AllStoryText);

		FString SaveFileString;
		for (auto DataRow : OutputFileInfo)
		{
			for (int RowIndex = 0; RowIndex < DataRow.Num(); ++RowIndex)
			{
				SaveFileString.Append(DataRow[RowIndex]);
				if (RowIndex + 1 < DataRow.Num())
					SaveFileString.Append(TEXT("\t"));
			}
			SaveFileString.Append(TEXT("\n"));
		}

		const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		const FString FileTypes = TEXT("Data Table CSV (*.csv)|*.csv");
		TArray<FString> OutFilenames;
		IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
		DesktopPlatform->SaveFileDialog(ParentWindowWindowHandle, TEXT("选择保存路径"), FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir()), TEXT(""),
			FileTypes, EFileDialogFlags::None, OutFilenames);

		if (OutFilenames.Num() > 0)
		{
			FFileHelper::SaveStringToFile(SaveFileString, *OutFilenames[0], FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
		}
	}

	static void AutoGenUIRefForLua()
	{
		FString log;
		FEdCommonGenerate::AutoGenUIRefForLua(log);
	}

	static void GenerateUEnumToProto()
	{
		FString log;
		FEdCommonGenerate::GenerateUEnumToProto(log);
	}

	static void GenQuestClassNameMap() 
	{
		FString log;
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter Filter;
		Filter.ClassNames.Add(UBlueprint::StaticClass()->GetFName());
		Filter.bRecursivePaths = true;
		Filter.bRecursiveClasses = true;
		Filter.PackagePaths.Add("/Game/EditorUtil/Quest");
		TArray<FAssetData> AssetList;
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);
		FString QuestClassNameMapStr = "QuestClassNameMap={";
		for (FAssetData Asset : AssetList)
		{
			UBlueprint* NodeBP = Cast<UBlueprint>(Asset.GetAsset());
			UClass* NodeClass = NodeBP->GeneratedClass;
			const UDlgNode* DialogueNode = NodeClass->GetDefaultObject<UDlgNode>();
			FString NodeClassName = NodeBP->GetName();
			FString NodeCNName = DialogueNode->GetNodeTypeString();
			QuestClassNameMapStr += ("\n" + NodeClassName + "=\"" + NodeCNName + "\",");
		}
		for (TObjectIterator<UClass> It; It; ++It)
		{
			if (It->IsChildOf(UDlgNode::StaticClass()) && !It->HasAnyClassFlags(CLASS_Abstract) && !It->HasAnyClassFlags(CLASS_CompiledFromBlueprint))
			{
				const UDlgNode* DialogueNode = It->GetDefaultObject<UDlgNode>();
				FString NodeClassName = It->GetName();
				FString NodeCNName = DialogueNode->GetNodeTypeString();
				QuestClassNameMapStr += ("\n" + NodeClassName + "=\"" + NodeCNName + "\",");
			}
		}
		QuestClassNameMapStr += "\n}";
		const FString OUT_PATH = FPaths::ProjectContentDir() + "LuaSource/Config/QuestClassNameMap.lua";
		FFileHelper::SaveStringToFile(QuestClassNameMapStr, *OUT_PATH, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	static void AutoGenBlueprintTypeRefForLua()
	{
		FString log;
		FEdCommonGenerate::AutoGenBlueprintTypeRefForLua(log);
		FEdCommonGenerate::AutoGenCurveRefForLua(log);
		FEdCommonGenerate::AutoGenDataTableRefForLua(log);
		FEdCommonGenerate::AutoGenLevelSequenceSettingsForLua(log);
	}

	static void AutoGenAudioRefForLua()
	{
		FString log;
		FEdCommonGenerate::AutoGenAudioRefForLua(log);
	}

	static void AutoGenPersonalityLuaTable()
	{
		FString log;
		FEdCommonGenerate::GenerateUDataTableToLuaTable(log);
	}

	/**
	 * convert *.proto file's enum to lua file
	 *
	 * @param inPBdir Directory for scanning *.proto files
	 * @param inOutLuaFileName output directory of lua's file
	 * @param inExcludePBFileNames *proto files to be excluded
	 */
	static void ConvertPBEnumToLua(const FString& inPBdir, const FString& inOutLuaFileName, const TArray<FString>& inExcludePBFileNames)
	{
		const FString tStrChatProtoDir = FPaths::ConvertRelativePathToFull(inPBdir);
		TArray<FString> tArrayPBFileNames;
		IFileManager::Get().FindFiles(tArrayPBFileNames, *tStrChatProtoDir, *FString(TEXT("*.proto")));
		TArray<FString> tArrayOutLuaLines;
		tArrayOutLuaLines.Add(FString("--This is a cmd desc file auto gen from origin-proto."));
		tArrayOutLuaLines.Add(FString("--Please do no modification."));
		tArrayOutLuaLines.Add(FString("local EMsgId = {"));
		auto tFuncIsExclude = [&](const FString& inFileName)
		{
			for (int32 tIndex = 0; tIndex < inExcludePBFileNames.Num(); ++tIndex)
			{
				if (inFileName.StartsWith(inExcludePBFileNames[tIndex]))
					return true;
			}
			return false;
		};

		for (int tFileIndex = 0; tFileIndex < tArrayPBFileNames.Num(); ++tFileIndex)
		{
			if (tFuncIsExclude(tArrayPBFileNames[tFileIndex]))
				continue;
			TArray<FString> tArrayPBFileLines;
			if (!FFileHelper::LoadFileToStringArray(tArrayPBFileLines, *(tStrChatProtoDir + tArrayPBFileNames[tFileIndex])))
				continue;
			for (int tLineIndex = 0; tLineIndex < tArrayPBFileLines.Num(); ++tLineIndex)
			{
				if (tArrayPBFileLines[tLineIndex].TrimStart().StartsWith("enum"))
				{
					// compatible with mode "enum EXXXXXXX {" and "enum EXXXXXXX"
					FString tOutLine = FString("    ") + tArrayPBFileLines[tLineIndex].TrimStart().Replace(TEXT("enum"), TEXT(""));
					tOutLine = tOutLine.Replace(TEXT("{"), TEXT(""));
					tArrayOutLuaLines.Add(tOutLine + "={");
					tLineIndex += tArrayPBFileLines[tLineIndex].TrimStart().EndsWith("{") ? 1 : 2;

					while (!tArrayPBFileLines[tLineIndex].TrimStart().StartsWith("}"))
					{
						if (tArrayPBFileLines[tLineIndex].TrimStart().StartsWith("/*"))
						{
							while (!tArrayPBFileLines[tLineIndex++].TrimStart().StartsWith("*/"));
							continue;
						}
						// enum sometimes starts with none-1 index, while in most circumstances,
						// we'll use the string-key instead of number-value as a index, so the original
						// format is kept unchanged here.
						FString cc = tArrayPBFileLines[tLineIndex++].Replace(TEXT(";"), TEXT(",")).Replace(TEXT("//"), TEXT("--"));
						tArrayOutLuaLines.Add(FString("        ") + cc);
					}
					tArrayOutLuaLines.Add("    },");
				}
			}
		}
		tArrayOutLuaLines.Add("}");
		tArrayOutLuaLines.Add("return EMsgId");
		FFileHelper::SaveStringArrayToFile(tArrayOutLuaLines, *inOutLuaFileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	/**
	 * generating reflection information of protocol buffers command's enum
	 *
	 * @param inPBFileName Directory for scanning *.proto files
	 * @param inOutLuaFileName output directory of lua's file
	 * @param inCmdLineStart the string command enum line start in pb file
	 * @param inLuaCmdLinePrefix additional prefix in lua's command
	 */
	static void GenPBCmdReflection(const FString& inPBFileName, const FString& inOutLuaFileName, const FString& inCmdLineStart, const FString& inLuaCmdLinePrefix)
	{
		TArray<FString> tArrayOutput;
		TArray<FString> tArrayXDAppChatCmdList;
		FFileHelper::LoadFileToStringArray(tArrayXDAppChatCmdList, *inPBFileName);
		tArrayOutput.Add(FString("--Please do no modification."));
		tArrayOutput.Add(FString("local EPBCmdInfo = {"));
		bool tFindCmd = false;
		FRegexPattern tPattern(TEXT("[a-zA-Z0-9]+"));
		FString tPackageName(TEXT("Unknow"));
		FString tPackageFlag(TEXT("// package:"));
		for (int32 tIndex = 0; tIndex < tArrayXDAppChatCmdList.Num(); ++tIndex)
		{
			if (!tFindCmd)
			{
				tFindCmd = tArrayXDAppChatCmdList[tIndex].TrimStart().StartsWith(inCmdLineStart);
				continue;
			}
			if (tArrayXDAppChatCmdList[tIndex].TrimStart().StartsWith(TEXT("}")))
				break;
			if (tArrayXDAppChatCmdList[tIndex].TrimStart().StartsWith(tPackageFlag))
			{
				tPackageName = tArrayXDAppChatCmdList[tIndex].TrimStart().RightChop(tPackageFlag.Len());
				continue;
			}
			if (tArrayXDAppChatCmdList[tIndex].TrimStart().StartsWith(TEXT("//")))
				continue;
			FString tCmd = tArrayXDAppChatCmdList[tIndex].TrimStart();
			FRegexMatcher tRegexMatcher(tPattern, tCmd);
			if (!tRegexMatcher.FindNext())
				continue;
			FString tCommandName(tRegexMatcher.GetCaptureGroup(0));
			if (!tRegexMatcher.FindNext())
				continue;
			FString tCommandValue(tRegexMatcher.GetCaptureGroup(0));
			FString tStrValueTable(TEXT("{ msgType = '") + tCommandName + TEXT("', cmdValue = ") + tCommandValue + TEXT(", package = '") + tPackageName + TEXT("' }"));
			tArrayOutput.Add(inLuaCmdLinePrefix + tCommandName + TEXT(" = ") + tStrValueTable + TEXT(","));
		}
		tArrayOutput.Add(FString("}"));
		tArrayOutput.Add(FString("return EPBCmdInfo"));
		FFileHelper::SaveStringArrayToFile(tArrayOutput, *inOutLuaFileName, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM);
	}

	/**
	 * generate lua information about xdapp's google protocol buffers
	 */
	static void AutoGenXDAppPbc()
	{
		// generating pb cmd reflection information to lua
		const FString tStrOutputFile = FPaths::ProjectContentDir() + "LuaSource/Network/EXDAppPBInfo.lua";
		const FString tStrXDAppChatProtoFile = FPaths::ProjectContentDir() + "Proto/xdapp/chat/cmd.proto";
		GenPBCmdReflection(tStrXDAppChatProtoFile, tStrOutputFile, TEXT("enum Cmd"), TEXT("    Chat"));
		// export pb's enum to lua
		const FString tStrChatPBDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + "Proto/xdapp/chat/");
		const FString tStrChatPBEnumLuaFile = FPaths::ProjectContentDir() + "LuaSource/Network/EXDAppChatEnum.lua";
		const TArray<FString> tExcludeFiles;
		ConvertPBEnumToLua(tStrChatPBDir, tStrChatPBEnumLuaFile, tExcludeFiles);
		// generating and register pbc file 
		const FString tStrBatDir(FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + TEXT("Proto/xdapp/chat/")) + TEXT(" && pb2pbcForChat.bat "));
		const FString tStrExeDir(FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir() + TEXT("Proto/")));
		FEdCommonGenerate::GenAndRegPBC(tStrBatDir, tStrExeDir);
	}

	static void ConvertAnimEvent()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimSequenceBase::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game");

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		TMap<FName, FName> AssetRegistry;
		for (FAssetData Asset : AssetList)
		{
			UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(Asset.GetAsset());
			if (AnimSequence && AnimSequence->Notifies.Num() > 0)
			{
				bool hasChanged = false;
				bool hasTimedParticleEffect = false;
				for (int32 I = 0; I < AnimSequence->Notifies.Num(); ++I)
				{
					struct FAnimNotifyEvent& OldEvent = (AnimSequence->Notifies[I]);
					if (OldEvent.NotifyStateClass && UKismetMathLibrary::ClassIsChildOf(OldEvent.NotifyStateClass->GetClass(), UAnimNotifyState_TimedParticleEffect::StaticClass()))
					{
						hasTimedParticleEffect = true;
						break;
					}
				}
				if(!hasTimedParticleEffect)
					continue;

				for (int32 I = 0; I < AnimSequence->Notifies.Num(); ++I)
				{
					struct FAnimNotifyEvent OldEvent = (AnimSequence->Notifies[I]);
					if (OldEvent.NotifyStateClass && UKismetMathLibrary::ClassIsChildOf(OldEvent.NotifyStateClass->GetClass(), UAnimNotifyState_TimedParticleEffect::StaticClass()) || OldEvent.Notify && UKismetMathLibrary::ClassIsChildOf(OldEvent.Notify->GetClass(), UAnimNotify_PlayParticleEffect::StaticClass()))
					{
						if (!hasChanged)
							AnimSequence->Modify(true);
						hasChanged = true;

						float BeginTime = OldEvent.GetTime();
						float Length = OldEvent.GetDuration();
						int32 TargetTrackIndex = (AnimSequence->AnimNotifyTracks.Num() - 1) - OldEvent.TrackIndex;
						float TriggerTimeOffset = OldEvent.TriggerTimeOffset;
						float EndTriggerTimeOffset = OldEvent.EndTriggerTimeOffset;
						int32 SlotIndex = OldEvent.GetSlotIndex();
						int32 EndSlotIndex = OldEvent.EndLink.GetSlotIndex();
						int32 SegmentIndex = OldEvent.GetSegmentIndex();
						int32 EndSegmentIndex = OldEvent.GetSegmentIndex();
						EAnimLinkMethod::Type LinkMethod = OldEvent.GetLinkMethod();
						EAnimLinkMethod::Type EndLinkMethod = OldEvent.EndLink.GetLinkMethod();

						FColor OldColor = OldEvent.NotifyColor;
						UAnimNotify* OldEventPayload = OldEvent.Notify;
						UAnimNotifyState* OldEventStatePayload = OldEvent.NotifyStateClass;

						// Delete old one before creating new one to avoid potential array re-allocation when array temporarily increases by 1 in size

						AnimSequence->Notifies.RemoveAt(I);

						FName NotifyName = OldEvent.NotifyName;

						// CreateNewNotify

						// Insert a new notify record and spawn the new notify object
						int32 NewNotifyIndex = AnimSequence->Notifies.Add(FAnimNotifyEvent());
						FAnimNotifyEvent& NewEvent = AnimSequence->Notifies[NewNotifyIndex];
						NewEvent.NotifyName = NotifyName;

						NewEvent.Link(AnimSequence, BeginTime);
						NewEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(AnimSequence->CalculateOffsetForNotify(BeginTime));
						NewEvent.TrackIndex = TargetTrackIndex;

						if (OldEvent.NotifyStateClass && UKismetMathLibrary::ClassIsChildOf(OldEvent.NotifyStateClass->GetClass(), UAnimNotifyState_TimedParticleEffect::StaticClass()))
						{
							class UObject* AnimNotifyClass = NewObject<UObject>(AnimSequence, UAnimNotifyState_WrappedTimedParticleEffect::StaticClass(), NAME_None, RF_Transactional);
							NewEvent.NotifyStateClass = Cast<UAnimNotifyState>(AnimNotifyClass);
							NewEvent.Notify = nullptr;
							if (nullptr != NewEvent.NotifyStateClass)
							{
								NewEvent.NotifyStateClass->OnAnimNotifyCreatedInEditor(NewEvent);
							}
						}
						else if (OldEvent.Notify && UKismetMathLibrary::ClassIsChildOf(OldEvent.Notify->GetClass(), UAnimNotify_PlayParticleEffect::StaticClass()))
						{
							class UObject* Notify = NewObject<UObject>(AnimSequence, UAnimNotify_WrappedPlayParticleEffect::StaticClass(), NAME_None, RF_Transactional);
							//NewEvent.NotifyStateClass = Cast<UAnimNotifyState>(AnimNotifyClass);
							NewEvent.Notify = Cast<UAnimNotify>(Notify);

							// Set default duration to 1 frame for AnimNotifyState.
							if (NewEvent.NotifyStateClass)
							{
								NewEvent.SetDuration(1 / 30.f);
								NewEvent.EndLink.Link(AnimSequence, NewEvent.EndLink.GetTime());
							}

							if (NewEvent.Notify)
							{
								TArray<FAssetData> SelectedAssets;
								AssetSelectionUtils::GetSelectedAssets(SelectedAssets);

								for (TFieldIterator<FObjectProperty> PropIt(NewEvent.Notify->GetClass()); PropIt; ++PropIt)
								{
									if (PropIt->GetBoolMetaData(TEXT("ExposeOnSpawn")))
									{
										FObjectProperty* Property = *PropIt;
										const FAssetData* tAsset = SelectedAssets.FindByPredicate([Property](const FAssetData& Other)
										{
											return Other.GetAsset()->IsA(Property->PropertyClass);
										});

										if (tAsset)
										{
											uint8* Offset = (*PropIt)->ContainerPtrToValuePtr<uint8>(NewEvent.Notify);
											(*PropIt)->ImportText(*tAsset->GetAsset()->GetPathName(), Offset, 0, NewEvent.Notify);
											break;
										}
									}
								}

								NewEvent.Notify->OnAnimNotifyCreatedInEditor(NewEvent);

							}
							else if (NewEvent.NotifyStateClass)
							{
								//NewEvent.NotifyStateClass->OnAnimNotifyCreatedInEditor(NewEvent);
							}
						}

						//FAnimNotifyEvent& NewEvent = AnimSequence->AnimNotifyTracks[TargetTrackIndex]->CreateNewNotify(NotifyName, NewNotifyClass, BeginTime);

						NewEvent.TriggerTimeOffset = TriggerTimeOffset;
						NewEvent.ChangeSlotIndex(SlotIndex);
						NewEvent.SetSegmentIndex(SegmentIndex);
						NewEvent.ChangeLinkMethod(LinkMethod);
						NewEvent.NotifyColor = OldColor;

						// Copy what we can across from the payload
						if ((OldEventPayload != nullptr) && (NewEvent.Notify != nullptr))
						{
							UEngine::FCopyPropertiesForUnrelatedObjectsParams CopyParams;
							UEngine::CopyPropertiesForUnrelatedObjects(OldEventPayload, NewEvent.Notify, CopyParams);
							UAnimNotify_WrappedPlayParticleEffect *PlayParticleEffect = Cast<UAnimNotify_WrappedPlayParticleEffect>(NewEvent.Notify);
							if (PlayParticleEffect && PlayParticleEffect->PSTemplate)
							{
								TSoftObjectPtr<UParticleSystem> PSAssetPtr(PlayParticleEffect->PSTemplate);
								PlayParticleEffect->PSTemplateSoft = PSAssetPtr;
								PlayParticleEffect->PSTemplate = nullptr;
							}
						}
						if ((OldEventStatePayload != nullptr) && (NewEvent.NotifyStateClass != nullptr))
						{
							UEngine::FCopyPropertiesForUnrelatedObjectsParams CopyParams;
							UEngine::CopyPropertiesForUnrelatedObjects(OldEventStatePayload, NewEvent.NotifyStateClass, CopyParams);
							UAnimNotifyState_WrappedTimedParticleEffect *TimedParticleEffect = Cast<UAnimNotifyState_WrappedTimedParticleEffect>(NewEvent.NotifyStateClass);
							if (TimedParticleEffect && TimedParticleEffect->PSTemplate)
							{
								TSoftObjectPtr<UParticleSystem> PSAssetPtr(TimedParticleEffect->PSTemplate);
								TimedParticleEffect->PSTemplateSoft = PSAssetPtr;
								TimedParticleEffect->PSTemplate = nullptr;
							}
						}

						// For Anim Notify States, handle the end time and link
						if (NewEvent.NotifyStateClass != nullptr)
						{
							NewEvent.SetDuration(Length);
							NewEvent.EndTriggerTimeOffset = EndTriggerTimeOffset;
							NewEvent.EndLink.ChangeSlotIndex(EndSlotIndex);
							NewEvent.EndLink.SetSegmentIndex(EndSegmentIndex);
							NewEvent.EndLink.ChangeLinkMethod(EndLinkMethod);
						}

						NewEvent.Update();
					}
				}
				if (hasChanged)
				{
					AnimSequence->PostEditChange();
					AnimSequence->MarkPackageDirty();
					AnimSequence->RefreshCacheData();
					TMap<bool, TArray<UPackage*>> DirtyPackage = AnimSequence->RefreshAndCreateAnimNotifyDataPersistentCache(true);
					TArray<UPackage*> DeletePackageList = DirtyPackage[false];
					TArray<UObject*> DeleteObjectList;
					for (auto iter = DeletePackageList.CreateIterator(); iter; ++iter)
					{
						DeleteObjectList.Add(CastChecked<UObject>(*iter));
					}
					ObjectTools::DeleteObjectsUnchecked(DeleteObjectList);
				}
			}
		}
	}

	static void SetAnimRetargetSource()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		// first set common bkh
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game/Assets/_Art/_Character/_Common/Suit/Ply_Female_H/Animation");
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		for (FAssetData Asset : AssetList)
		{
			UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset.GetAsset());
			if (AnimSequence->GetSkeleton()->GetName() == "Major_Skeleton_BKH")
			{
				AnimSequence->RetargetSource = FName(TEXT("PlayerSkin_Bikini_BKH"));
				AnimSequence->PostEditChange();
				AnimSequence->MarkPackageDirty();
			}
		}

		// second set Heroine and Teammate

		FARFilter FilterAnimSequences2;
		FilterAnimSequences2.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		FilterAnimSequences2.bRecursivePaths = true;
		FilterAnimSequences2.bRecursiveClasses = true;
		FilterAnimSequences2.PackagePaths.Add("/Game/Assets/_Art/_Character/Heroine");
		FilterAnimSequences2.PackagePaths.Add("/Game/Assets/_Art/_Character/Teammate");
		TArray< FAssetData > AssetList2;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences2, AssetList2);

		for (FAssetData Asset : AssetList2)
		{
			UAnimSequence* AnimSequence = Cast<UAnimSequence>(Asset.GetAsset());
			FName Path = Asset.PackagePath;
			FString PathStr = Path.ToString();
			if (PathStr.Right(10) == "/Animation")
			{
				FName ModelPath = FName(*PathStr.Left(PathStr.Len() - 10));

				FARFilter FilterMeshs;
				FilterMeshs.ClassNames.Add(USkeletalMesh::StaticClass()->GetFName());
				FilterMeshs.bRecursivePaths = true;
				FilterMeshs.bRecursiveClasses = true;
				FilterMeshs.PackagePaths.Add(ModelPath);
				TArray< FAssetData > AssetList3;
				AssetRegistryModule.Get().GetAssets(FilterMeshs, AssetList3);
				bool hasMesh = false;
				for (FAssetData Asset3 : AssetList3)
				{
					USkeletalMesh* SkeletalMesh = Cast<USkeletalMesh>(Asset3.GetAsset());
					if (SkeletalMesh->Skeleton == AnimSequence->GetSkeleton() && SkeletalMesh->Skeleton->GetName() == "Major_Skeleton_BKH")
					{
						AnimSequence->RetargetSource = FName(*SkeletalMesh->GetName());
						AnimSequence->PostEditChange();
						AnimSequence->MarkPackageDirty();
						hasMesh = true;
						break;
					}
				}
				if (!hasMesh)
				{
					UE_LOG(LogAtelierEditor, Warning, TEXT("Error Animation Path1  %s"), *Asset.ObjectPath.ToString());
				}
			}
			else
			{
				UE_LOG(LogAtelierEditor, Warning, TEXT("Error Animation Path2  %s"), *Asset.ObjectPath.ToString());
			}
		}
	}

	static void SwitchAnimNotifyData()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimNotifyData::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game");

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		TMap<FName, FName> AssetRegistry;
		for (FAssetData Asset : AssetList)
		{
			if (Asset.AssetName.ToString().Right(14).Equals(TEXT("AnimDataEffect")))
			{

				UAnimNotifyData* NotifyData = Cast<UAnimNotifyData>(Asset.GetAsset());
				if (NotifyData && NotifyData->Notifies.Num() > 0)
				{
					bool Changed = false;
					for (struct FAnimNotifyEvent& Event : NotifyData->Notifies)
					{
						if (Event.Notify)
						{
							if (UKismetMathLibrary::ClassIsChildOf(Event.Notify->GetClass(), UAnimNotify_WrappedPlayParticleEffect::StaticClass()))
							{
								UAnimNotify_WrappedPlayParticleEffect *PlayParticleEffect = Cast<UAnimNotify_WrappedPlayParticleEffect>(Event.Notify);
								if (PlayParticleEffect && PlayParticleEffect->PSTemplate)
								{
									TSoftObjectPtr<UParticleSystem> PSAssetPtr(PlayParticleEffect->PSTemplate);
									PlayParticleEffect->PSTemplateSoft = PSAssetPtr;
									PlayParticleEffect->PSTemplate = nullptr;
									Changed = true;
								}
							}
							else
								UE_LOG(LogAtelierEditor, Warning, TEXT("Another FxNotify is %s in %s"), *Event.Notify->GetClass()->GetName(), *NotifyData->GetFullName());
						}
						if (Event.NotifyStateClass)
						{
							if (UKismetMathLibrary::ClassIsChildOf(Event.NotifyStateClass->GetClass(), UAnimNotifyState_WrappedTimedParticleEffect::StaticClass()))
							{
								UAnimNotifyState_WrappedTimedParticleEffect *PlayParticleEffect = Cast<UAnimNotifyState_WrappedTimedParticleEffect>(Event.NotifyStateClass);
								if (PlayParticleEffect && PlayParticleEffect->PSTemplate)
								{
									TSoftObjectPtr<UParticleSystem> PSAssetPtr(PlayParticleEffect->PSTemplate);
									PlayParticleEffect->PSTemplateSoft = PSAssetPtr;
									PlayParticleEffect->PSTemplate = nullptr;
									Changed = true;
								}
							}
							else
								UE_LOG(LogAtelierEditor, Warning, TEXT("Another FxNotify is %s in %s"), *Event.NotifyStateClass->GetClass()->GetName(), *NotifyData->GetFullName());
						}
					}
					if (Changed)
					{
						NotifyData->PostEditChange();
						NotifyData->MarkPackageDirty();
					}
				}
			}
		}
	}

	static void RefreshAnimNotifyData()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimSequenceBase::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game");

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		TMap<FName, FName> AssetRegistry;
		for (FAssetData Asset : AssetList)
		{
			UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(Asset.GetAsset());
			if (AnimSequence && AnimSequence->Notifies.Num() > 0)
			{
				AnimSequence->FlushAnimationNotify();
				AnimSequence->PostEditChange();
				AnimSequence->MarkPackageDirty();
				AnimSequence->RefreshCacheData();
				TMap<bool, TArray<UPackage*>> DirtyPackage = AnimSequence->RefreshAndCreateAnimNotifyDataPersistentCache(true);
				TArray<UPackage*> DeletePackageList = DirtyPackage[false];
				TArray<UObject*> DeleteObjectList;
				for (auto iter = DeletePackageList.CreateIterator(); iter; ++iter)
				{
					DeleteObjectList.Add(CastChecked<UObject>(*iter));
				}
				ObjectTools::DeleteObjectsUnchecked(DeleteObjectList);
			}
		}
	}

	static void DeduplicationAnimNotifyData()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimNotifyData::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game");

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		TMap<FName, FName> AssetRegistry;
		for (FAssetData Asset : AssetList)
		{
			if (Asset.AssetName.ToString().Right(14).Equals(TEXT("AnimDataEffect")))
			{

				UAnimNotifyData* NotifyData = Cast<UAnimNotifyData>(Asset.GetAsset());
				if (NotifyData && NotifyData->Notifies.Num() > 0)
				{
					const UAnimSequenceBase* LinkedAnimSequence = NotifyData->Notifies[0].GetLinkedSequence();
					UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(StaticLoadObject(UAnimSequenceBase::StaticClass(), NULL, *LinkedAnimSequence->GetPathName(), NULL, LOAD_None, NULL));
					TArray<struct FAnimNotifyEvent> DeduplicationNotifies;
					for (struct FAnimNotifyEvent& Event : NotifyData->Notifies)
					{
						if (!Event.Notify || !Event.NotifyStateClass)
							continue;
						bool hasInArray = false;
						for (struct FAnimNotifyEvent& CEvent : DeduplicationNotifies)
						{
							if (Event.TriggerTimeOffset == CEvent.TriggerTimeOffset)
							{
								if (Event.Notify && CEvent.Notify && Event.Notify->GetClass() == CEvent.Notify->GetClass())
								{
									if (UKismetMathLibrary::ClassIsChildOf(Event.Notify->GetClass(), UAnimNotify_PlayParticleEffect::StaticClass()))
									{
										UAnimNotify_PlayParticleEffect* T_Notify = Cast<UAnimNotify_PlayParticleEffect>(Event.Notify);
										UAnimNotify_PlayParticleEffect* T_CNotify = Cast<UAnimNotify_PlayParticleEffect>(CEvent.Notify);
										if (T_Notify->PSTemplate == T_CNotify->PSTemplate && T_Notify->LocationOffset == T_CNotify->LocationOffset && T_Notify->RotationOffset == T_CNotify->RotationOffset && T_Notify->Scale == T_CNotify->Scale && T_Notify->Attached == T_CNotify->Attached && T_Notify->SocketName == T_CNotify->SocketName)
										{
											hasInArray = true;
											break;
										}
									}
									else if (UKismetMathLibrary::ClassIsChildOf(Event.Notify->GetClass(), UAnimNotify_WrappedPlayParticleEffect::StaticClass()))
									{
										UAnimNotify_WrappedPlayParticleEffect* T_Notify = Cast<UAnimNotify_WrappedPlayParticleEffect>(Event.Notify);
										UAnimNotify_WrappedPlayParticleEffect* T_CNotify = Cast<UAnimNotify_WrappedPlayParticleEffect>(CEvent.Notify);
										if (T_Notify->PSTemplate == T_CNotify->PSTemplate && T_Notify->LocationOffset == T_CNotify->LocationOffset && T_Notify->RotationOffset == T_CNotify->RotationOffset && T_Notify->Scale == T_CNotify->Scale && T_Notify->Attached == T_CNotify->Attached && T_Notify->SocketName == T_CNotify->SocketName)
										{
											hasInArray = true;
											break;
										}
									}
									else
										UE_LOG(LogAtelierEditor, Warning, TEXT("Another FxNotify is  %s"), *Event.Notify->GetClass()->GetName());
								}
								if (Event.NotifyStateClass && CEvent.NotifyStateClass && Event.NotifyStateClass->GetClass() == CEvent.NotifyStateClass->GetClass())
								{
									if (UKismetMathLibrary::ClassIsChildOf(Event.NotifyStateClass->GetClass(), UAnimNotifyState_WrappedTimedParticleEffect::StaticClass()))
									{
										UAnimNotifyState_WrappedTimedParticleEffect* T_Notify = Cast<UAnimNotifyState_WrappedTimedParticleEffect>(Event.NotifyStateClass);
										UAnimNotifyState_WrappedTimedParticleEffect* T_CNotify = Cast<UAnimNotifyState_WrappedTimedParticleEffect>(CEvent.NotifyStateClass);
										if (T_Notify->PSTemplate == T_CNotify->PSTemplate && T_Notify->LocationOffset == T_CNotify->LocationOffset && T_Notify->RotationOffset == T_CNotify->RotationOffset && T_Notify->bDestroyAtEnd == T_CNotify->bDestroyAtEnd && T_Notify->SocketName == T_CNotify->SocketName)
										{
											hasInArray = true;
											break;
										}
									}
								}
							}
						}
						if (!hasInArray)
							DeduplicationNotifies.Push(Event);
					}
					if (NotifyData->Notifies.Num() != DeduplicationNotifies.Num())
					{
						NotifyData->Notifies = DeduplicationNotifies;
						NotifyData->PostEditChange();
						NotifyData->MarkPackageDirty();
						if (AnimSequence)
						{
							AnimSequence->FlushAnimationNotify();
							AnimSequence->PostEditChange();
							AnimSequence->MarkPackageDirty();
						}
					}
				}
			}
		}
	}

#pragma region  导出后端数据

	static void ExportAnimationMotionData()
	{
		FString log;
		FEdCommonGenerate::ExportAnimationMotionData(true, log);
	}


	static void ExportSanData()
	{
		FString log;
		FEdCommonGenerate::ExportSanData(true, log);
	}

	static void ExportAnimationData()
	{
		FString log;
		FEdCommonGenerate::ExportAnimationData(true, log);
	}

	static void ExportAllBackendData()
	{
		ExportAnimationMotionData();
		ExportSanData();
		ExportAnimationData();
	}

	static void GenAnimationTable()
	{
		FString log;
		FEdCommonGenerate::GenerateAnimTable(true, log);
	}

	static void ExportAllAnimationData()
	{
		FString log;
		FEdCommonGenerate::ExportAllAnimationData(true, log);
	}

#pragma endregion

	static void GenerateJumpLinks()
	{
		RecastExporter::GenerateJumpLinks(UEditorStaticLibrary::GetEditorWorld());
	}

	static void ExportNavMeshData()
	{
		FString MapPackagedName = UEditorStaticLibrary::GetEditorWorld()->GetCurrentLevel()->GetPathName();
		FEdCommonGenerate::ExportNavMeshData(false,MapPackagedName);
	}

	static void GenerateAnimComposite()
	{
		UE_LOG(LogAtelierEditor, Warning, TEXT("Generate Animation Composite From AnimSequence Begin"));

		//if (CurrentSelectedPaths.Num() == 0)
		//	return;

		//FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		//FAssetToolsModule& AssetToolsModule = FAssetToolsModule::GetModule();

		//const TArray<FAnimNotifySortItem>& NotifySortItems = FAnimNotifySortConfig::GetInstance()->GetAllSortItem();

		//auto ReflashFloderFunc = [&](const FName& Path)
		//{
		//	FARFilter Filter;
		//	Filter.PackagePaths.Add(Path);
		//	Filter.ClassNames.Emplace(TEXT("ObjectRedirector"));

		//	// Query for a list of assets in the selected paths
		//	TArray<FAssetData> AssetList;
		//	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//	if (AssetList.Num() > 0)
		//	{
		//		TArray<UObject*> Objects;
		//		//TArray<FString> ObjectPaths;
		//		for (const auto& Asset : AssetList)
		//		{
		//			//ObjectPaths.Add(Asset.ObjectPath.ToString());
		//			Objects.Add(CastChecked<UObjectRedirector>(Asset.GetAsset()));
		//		}
		//		TArray<UObjectRedirector*> Redirectors;
		//		for (auto Object : Objects)
		//		{
		//			auto Redirector = CastChecked<UObjectRedirector>(Object);
		//			Redirectors.Add(Redirector);
		//		}
		//		AssetToolsModule.Get().FixupReferencers(Redirectors);
		//	}
		//};

		////记录当前AnimComposite和对应Track数据，转换完成后刷新Track
		//TMap<FName, TArray<FName>> FreshTrackAnimCompositeMap;
		//{
		//	FARFilter Filter;
		//	Filter.ClassNames.Add(UAnimComposite::StaticClass()->GetFName());
		//	for (auto Path : CurrentSelectedPaths)
		//	{
		//		Filter.PackagePaths.Add(FName("/Game/Assets/_Art"));
		//	}
		//	Filter.bRecursivePaths = true;

		//	TArray< FAssetData > AssetList;
		//	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//	for (FAssetData AssetData : AssetList)
		//	{
		//		UAnimComposite* AnimComposite = Cast<UAnimComposite>(AssetData.GetAsset());
		//		FAnimTrack& Track = AnimComposite->AnimationTrack;
		//		TArray<FAnimSegment>& Segments = Track.AnimSegments;
		//		if (Segments.Num() > 0)
		//		{
		//			TArray<FName> Names;
		//			for (auto iter = Segments.CreateIterator(); iter; iter++)
		//			{
		//				FAnimSegment& Seg = *iter;
		//				Names.Add(Seg.AnimReference->GetOutermost()->FileName);
		//			}
		//			FreshTrackAnimCompositeMap.Add(AssetData.PackageName, Names);
		//		}
		//	}
		//}

		////记录当前AnimMontage和对应Track数据，转换完成后刷新Track
		//TMap<FName, TMap<FName, TArray<FName>>> FreshTrackAnimMontageMap;
		//{
		//	FARFilter Filter;
		//	Filter.ClassNames.Add(UAnimMontage::StaticClass()->GetFName());
		//	for (auto Path : CurrentSelectedPaths)
		//	{
		//		Filter.PackagePaths.Add(FName("/Game/Assets/_Art"));
		//	}
		//	Filter.bRecursivePaths = true;

		//	TArray< FAssetData > AssetList;
		//	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//	for (FAssetData AssetData : AssetList)
		//	{
		//		UAnimMontage* AnimMontage = Cast<UAnimMontage>(AssetData.GetAsset());
		//		if (AnimMontage->SlotAnimTracks.Num() > 0)
		//		{
		//			TMap<FName, TArray<FName>> TempMap;
		//			for (auto iter = AnimMontage->SlotAnimTracks.CreateIterator(); iter; iter++)
		//			{
		//				FSlotAnimationTrack& SlotAnimationTrack = *iter;
		//				FAnimTrack& Track = SlotAnimationTrack.AnimTrack;
		//				TArray<FAnimSegment>& Segments = Track.AnimSegments;
		//				if (Segments.Num() > 0)
		//				{
		//					TArray<FName> Names;
		//					for (auto iter = Segments.CreateIterator(); iter; iter++)
		//					{
		//						FAnimSegment& Seg = *iter;
		//						Names.Add(Seg.AnimReference->GetOutermost()->FileName);
		//					}
		//					TempMap.Add(SlotAnimationTrack.SlotName, Names);
		//				}
		//			}
		//			if (TempMap.Num() > 0)
		//			{
		//				FreshTrackAnimMontageMap.Add(AssetData.PackageName, TempMap);
		//			}
		//		}
		//	}
		//}

		//TMap<FName, bool> AllPathMap;

		////生成目录信息
		//{
		//	FARFilter Filter;
		//	Filter.bRecursivePaths = true;
		//	Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());

		//	for (auto Path : CurrentSelectedPaths)
		//	{
		//		Filter.PackagePaths.Add(FName(*Path));
		//	}

		//	TArray< FAssetData > AssetList;
		//	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//	for (FAssetData AssetData : AssetList)
		//	{
		//		AllPathMap.Add(AssetData.PackagePath, true);
		//	}
		//}

		//auto SavePathFunc = [&](const FName& Path)
		//{
		//	FARFilter Filter;
		//	Filter.PackagePaths.Add(Path);

		//	TArray< FAssetData > AssetList;
		//	AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//	TArray<UPackage*> PackagesToSave;
		//	for (FAssetData AssetData : AssetList)
		//	{
		//		PackagesToSave.Add(AssetData.GetAsset()->GetOutermost());
		//	}

		//	TArray<UPackage*> FailedPackages;
		//	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false, &FailedPackages);
		//	if (FailedPackages.Num() > 0)
		//	{
		//		for (auto iter = FailedPackages.CreateIterator(); iter; ++iter)
		//		{
		//			UPackage* Pkg = *iter;
		//			UE_LOG(LogAtelierEditor, Warning, TEXT("Save Faileds: %s"), *(Pkg->FileName.ToString()));
		//		}
		//	}
		//};

		//for (auto iter = AllPathMap.CreateIterator(); iter; ++iter)
		//{
		//	FName CurrentPath = iter->Key;

		//	//检查目录是否处理过
		//	bool Handled = false;
		//	{
		//		FARFilter Filter;
		//		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		//		Filter.PackagePaths.Add(CurrentPath);

		//		TArray< FAssetData > AssetList;
		//		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//		//TArray<FName> AnimNotifyDataPackageNames;

		//		for (FAssetData AssetData : AssetList)
		//		{
		//			if (AssetData.PackageName.ToString().Right(2).Equals("__"))
		//			{
		//				Handled = true;
		//				break;
		//			}
		//		}
		//	}
		//	if (Handled)
		//		continue;

		//	UE_LOG(LogAtelierEditor, Warning, TEXT("Hanlde Path : %s"), *(CurrentPath.ToString()));

		//	TArray<FName> NewAnimSequencePackageNames;
		//	TArray<FName> NewAnimCompositePackageNames;
		//	TMap<FName, UAnimSequenceBase*> FinalAnimCompositePackageNameObjectMap;

		//	TMap<FName, FName> NameCacheMap;

		//	//重命名AnimSequence
		//	{
		//		FARFilter Filter;
		//		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		//		Filter.PackagePaths.Add(CurrentPath);

		//		TArray< FAssetData > AssetList;
		//		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//		//TArray<FName> AnimNotifyDataPackageNames;

		//		for (FAssetData AssetData : AssetList)
		//		{
		//			FString AnimSequencePath = AssetData.PackagePath.ToString();
		//			FString AnimSequenceName = AssetData.AssetName.ToString();
		//			FString AnimSequencePackageName = AssetData.PackageName.ToString();
		//			FString PreFix = "__";

		//			FString NewAnimSequenceName;
		//			NewAnimSequenceName.Append(PreFix).Append(AnimSequenceName);
		//			UAnimSequence* AnimSequenceObject = CastChecked<UAnimSequence>(AssetData.GetAsset());

		//			TArray<FAssetRenameData> AssetsAndNames;
		//			FAssetRenameData RenameData(MakeWeakObjectPtr<UObject>(AnimSequenceObject), AnimSequencePath, NewAnimSequenceName);
		//			AssetsAndNames.Add(RenameData);
		//			AssetToolsModule.Get().RenameAssets(AssetsAndNames);
		//			FAssetRegistryModule::AssetRenamed(AnimSequenceObject, AnimSequencePath);
		//			AnimSequenceObject->MarkPackageDirty();

		//			FString NewAnimSequencePackageName;
		//			NewAnimSequencePackageName.Append(AnimSequencePath).Append("/").Append(NewAnimSequenceName);
		//			NewAnimSequencePackageNames.Add(FName(*NewAnimSequencePackageName));

		//			UE_LOG(LogAtelierEditor, Warning, TEXT("Rename AnimSequence : [%s][%s]"), *(AnimSequencePackageName), *(NewAnimSequencePackageName));

		//			NameCacheMap.Add(FName(*NewAnimSequencePackageName), FName(*AnimSequencePackageName));
		//		}
		//	}

		//	//刷新目录
		//	ReflashFloderFunc(CurrentPath);

		//	SavePathFunc("/Game/Assets/_Art");

		//	//生成AnimComposite
		//	{
		//		FARFilter Filter;
		//		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		//		for (auto iter = NameCacheMap.CreateIterator(); iter; ++iter)
		//		{
		//			Filter.PackageNames.Add(iter->Key);
		//		}
		//		//Filter.PackageNames = NewAnimSequencePackageNames;

		//		TArray< FAssetData > AssetList;
		//		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//		for (FAssetData AssetData : AssetList)
		//		{
		//			UAnimSequence* AnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());
		//			UAnimCompositeFactory* CompositeFactory = NewObject<UAnimCompositeFactory>();
		//			CompositeFactory->SourceAnimation = AnimSequence;

		//			FString AnimCompositeName = AssetData.AssetName.ToString();//AssetData.AssetName.ToString();
		//			AnimCompositeName = AnimCompositeName.Right(AnimCompositeName.Len() - 2);
		//			//AnimCompositeName.Append("Composite");

		//			FString AnimCompositePackageName = NameCacheMap.Find(AssetData.PackageName)->ToString(); //AssetData.PackageName.ToString();
		//			//AnimCompositePackageName.Append("Composite");
		//			NewAnimCompositePackageNames.Add(FName(*AnimCompositePackageName));

		//			UAnimationAsset* NewAsset = Cast<UAnimationAsset>(AssetToolsModule.Get().CreateAsset(AnimCompositeName, FPackageName::GetLongPackagePath(AnimCompositePackageName), UAnimComposite::StaticClass(), CompositeFactory));

		//			if (NewAsset)
		//			{
		//				NewAsset->MarkPackageDirty();
		//			}
		//			else
		//			{
		//				UE_LOG(LogAtelierEditor, Warning, TEXT("Create AnimComposite Failed : [%s]"), *(AnimCompositePackageName));
		//			}
		//		}
		//	}

		//	//刷新目录
		//	ReflashFloderFunc(CurrentPath);

		//	//保存
		//	SavePathFunc(CurrentPath);

		//	//迁移NotifyData
		//	{
		//		FARFilter Filter;

		//		//Filter.PackageNames = ToDeleteAnimNotifyDataPackageNames;
		//		TArray< FAssetData > AssetList;
		//		//AssetRegistryModule.Get().GetAssets(Filter, AssetList);
		//		//ObjectTools::DeleteAssets(AssetList);
		//		//ReflashFloderFunc();

		//		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		//		//Filter.PackageNames = NewAnimSequencePackageNames;
		//		for (auto iter = NameCacheMap.CreateIterator(); iter; ++iter)
		//		{
		//			Filter.PackageNames.Add(iter->Key);
		//		}
		//		AssetList.Empty();
		//		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//		for (FAssetData AssetData : AssetList)
		//		{
		//			UAnimSequence* AnimSequence = Cast<UAnimSequence>(AssetData.GetAsset());

		//			//FString AnimSequencePackageName = AssetData.PackageName.ToString();
		//			FString OriginalPackageName = NameCacheMap.Find(AssetData.PackageName)->ToString();//AnimSequencePackageName.Left(AnimSequencePackageName.Len() - 2);

		//			UPackage* Pkg = LoadPackage(nullptr, *OriginalPackageName, 0);
		//			UAnimComposite* AnimComposite = LoadObject<UAnimComposite>(Pkg, *Pkg->GetPathName());
		//			if (!AnimComposite)
		//			{
		//				UE_LOG(LogAtelierEditor, Warning, TEXT("Load AnimComposite Failed : [%s]"), *(OriginalPackageName));
		//				continue;
		//			}
		//			AnimComposite->DeepCopyNotifies(AnimSequence);
		//			AnimComposite->MarkPackageDirty();

		//			AnimSequence->RemoveAllNotifies();
		//			AnimSequence->MarkPackageDirty();
		//		}
		//	}

		//	//刷新目录
		//	ReflashFloderFunc(CurrentPath);

		//	//保存
		//	SavePathFunc(CurrentPath);
		//}

		////刷新老的AnimComposite
		//{
		//	TArray<UPackage*> PackagesToSave;
		//	for (auto iter = FreshTrackAnimCompositeMap.CreateIterator(); iter; ++iter)
		//	{
		//		FName Key = iter->Key;
		//		TArray<FName> Values = iter->Value;
		//		UPackage* Pkg = LoadPackage(nullptr, *(Key.ToString()), 0);
		//		UAnimComposite* AnimComposite = LoadObject<UAnimComposite>(Pkg, *Pkg->GetPathName());
		//		FAnimTrack& Track = AnimComposite->AnimationTrack;
		//		TArray<FAnimSegment>& Segments = Track.AnimSegments;
		//		for (int i = 0; i < Segments.Num(); ++i)
		//		{
		//			UPackage* Pkg = LoadPackage(nullptr, *(Values[i].ToString()), 0);
		//			UAnimComposite* AnimComposite = LoadObject<UAnimComposite>(Pkg, *Pkg->GetPathName());
		//			FAnimSegment& Seg = Segments[i];
		//			Seg.AnimReference = AnimComposite;
		//		}
		//		PackagesToSave.Add(Pkg);
		//	}

		//	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
		//}

		////刷新老的AnimMontage
		//{
		//	//TMap<FName, TMap<FName, TArray<FName>>> FreshTrackAnimMontageMap;
		//	TArray<UPackage*> PackagesToSave;

		//	for (auto iter = FreshTrackAnimMontageMap.CreateIterator(); iter; ++iter)
		//	{
		//		FName Key = iter->Key;
		//		TMap<FName, TArray<FName>>& SlotAnimMap = iter->Value;
		//		UPackage* Pkg = LoadPackage(nullptr, *(Key.ToString()), 0);
		//		UAnimMontage* AnimMontage = LoadObject<UAnimMontage>(Pkg, *Pkg->GetPathName());
		//		for (auto iter = AnimMontage->SlotAnimTracks.CreateIterator(); iter; ++iter)
		//		{
		//			FSlotAnimationTrack& SlotAnimationTrack = *iter;
		//			FAnimTrack& Track = SlotAnimationTrack.AnimTrack;
		//			TArray<FAnimSegment>& Segments = Track.AnimSegments;
		//			TArray<FName>& Array = *(SlotAnimMap.Find(SlotAnimationTrack.SlotName));
		//			for (int i = 0; i < Segments.Num(); ++i)
		//			{
		//				UPackage* Pkg = LoadPackage(nullptr, *(Array[i].ToString()), 0);
		//				UAnimComposite* AnimComposite = LoadObject<UAnimComposite>(Pkg, *Pkg->GetPathName());
		//				FAnimSegment& Seg = Segments[i];
		//				Seg.AnimReference = AnimComposite;
		//			}
		//		}
		//		PackagesToSave.Add(Pkg);
		//	}

		//	FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);
		//}

		UE_LOG(LogAtelierEditor, Warning, TEXT("Generate Animation Composite From AnimSequence End"));

		//CurrentSelectedPaths.Empty();
	}

	static void PreCheckModel()
	{
#if PLATFORM_WINDOWS
		const FString Command = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() + "/Binaries/Win64/UE4Editor.exe") + " "
			+ FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "Atelier.uproject")
			+ " -run=CheckAssets -c " + FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() + "UEChecker/Settings/test_case_model.json")
			+ " -o " + FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "model_test_result.log");

		_wsystem(*Command);
#endif
	}

	static void PreCheckTexture()
	{
#if PLATFORM_WINDOWS
		const FString Command = FPaths::ConvertRelativePathToFull(FPaths::EngineDir() + "/Binaries/Win64/UE4Editor.exe") + " "
			+ FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "Atelier.uproject")
			+ " -run=CheckAssets -c " + FPaths::ConvertRelativePathToFull(FPaths::ProjectPluginsDir() + "UEChecker/Settings/test_case_texture.json")
			+ " -o " + FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "texture_test_result.log");

		_wsystem(*Command);
#endif
	}

	static void InitExternalArtRes()
	{
#if PLATFORM_WINDOWS
		// clone plugin
		const FString Command = "cd " + FPaths::ProjectDir() + "Tools && InitArtResource.bat";
		_wsystem(*Command);

		static const FString PluginName = "ArtResource";
		const FString PluginFolder = FPaths::ProjectPluginsDir() + "Atelier/" + PluginName;
		const FString PluginFolderFull = FPaths::ConvertRelativePathToFull(PluginFolder);
		IPluginManager::Get().RefreshPluginsList();
		FText FailReason;
		IProjectManager::Get().SetPluginEnabled(PluginName, true, FailReason);
		IPluginManager::Get().MountNewlyCreatedPlugin(PluginName);

		IContentBrowserSingleton& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
		ContentBrowser.ForceShowPluginContent(false /*bIsEnginePlugin*/);
		TArray<FString> SelectedDirectories;
		SelectedDirectories.Add(TEXT("/") + PluginName);
		ContentBrowser.SetSelectedPaths(SelectedDirectories, true);
#endif
	}

	static void CheckFXAssets()
	{
		TMap<FString, FString> FXResMap;

#pragma region 检查技能表中的特效
		FString SkillEffectCSVPath = FPaths::ProjectContentDir() + "Table/Skill";
		TArray<FString> Files;
		IFileManager::Get().FindFilesRecursive(Files, *SkillEffectCSVPath, UTF8_TO_TCHAR("*.csv"), true, false, false);
		for (FString File : Files)
		{
			FString FileName = FPaths::GetCleanFilename(File);
			if (FileName.Contains("Effect"))
			{
				FString EffectContentString;
				if (FFileHelper::LoadFileToString(EffectContentString, *File))
				{
					const FCsvParser Parser(EffectContentString);
					const auto& Rows = Parser.GetRows();
					for (int32 RowIndex = 1; RowIndex < Rows.Num(); RowIndex++)
					{
						const TArray<const TCHAR*>& Cells = Rows[RowIndex];
						FString PropertyString = Cells[2];
						PropertyString = PropertyString.Replace(TEXT("'"), TEXT("\""));
						TArray<FString> strs;
						PropertyString.ParseIntoArray(strs, TEXT(","), true);
						for (int i = 0; i < strs.Num(); i++)
						{
							if (strs[i].Contains("PS="))
							{
								TArray<FString> str;
								strs[i].ParseIntoArray(str, TEXT("="), true);
								FString asset = str[1].Replace(TEXT("\""), TEXT(""));
								if (asset != "None")
								{
									TArray<FString> names;
									asset.ParseIntoArray(names, TEXT("/"), true);
									FString name = names[names.Num() - 1];
									TArray<FString> assetNames;
									name.ParseIntoArray(assetNames, TEXT("."), true);
									FString assetName = assetNames[0];
									FXResMap.Add(assetName, asset);
								}
							}
						}
					}
				}
			}
		}
#pragma endregion

		TMap<FString, FString> FXResourceGenMap;
		FString fxresourcepath = FPaths::ProjectContentDir() + "LuaSource/GamePlay/Gen/FXResourceGen.lua";
		FString fxresourcecontent;
		if (FFileHelper::LoadFileToString(fxresourcecontent, *fxresourcepath))
		{
			fxresourcecontent.ReplaceCharInline('\n', '|');
			fxresourcecontent.ReplaceCharInline('\r', '|');
			fxresourcecontent = fxresourcecontent.Replace(TEXT("|"), TEXT(""));
			fxresourcecontent = fxresourcecontent.Replace(TEXT("FXAsset = {"), TEXT(""));
			fxresourcecontent = fxresourcecontent.Replace(TEXT("};"), TEXT(""));
			TArray<FString> lines;
			fxresourcecontent.ParseIntoArray(lines, TEXT(","), true);
			for (int i = 0; i < lines.Num(); i++)
			{
				TArray<FString> assets;
				lines[i].ParseIntoArray(assets, TEXT("="), true);
				FString assetname = assets[0].Replace(TEXT("["), TEXT("")).Replace(TEXT("]"), TEXT("")).TrimStart().Replace(TEXT("\""), TEXT("")).TrimStartAndEnd();
				FString assetpath = assets[1].Replace(TEXT("\""), TEXT("")).TrimStartAndEnd();
				if (!FXResourceGenMap.Contains(assetname))
				{
					FXResourceGenMap.Add(assetname, assetpath);
				}
			}
		}

		TMap<FString, FString> BlueprintTypeGenMap;
		FString blueprinttypepath = FPaths::ProjectContentDir() + "LuaSource/GamePlay/Gen/BlueprintTypeGen.lua";
		FString blueprinttypecontent;
		if (FFileHelper::LoadFileToString(blueprinttypecontent, *blueprinttypepath))
		{
			blueprinttypecontent.ReplaceCharInline('\n', '|');
			blueprinttypecontent.ReplaceCharInline('\r', '|');
			blueprinttypecontent = blueprinttypecontent.Replace(TEXT("|"), TEXT(""));
			TArray<FString> blueprint;
			blueprinttypecontent.ParseIntoArray(blueprint, TEXT("};"), true);
			FString lines = blueprint[0];
			lines = lines.Replace(TEXT("BPType = {"), TEXT(""));

			TArray<FString> inlines;
			lines.ParseIntoArray(inlines, TEXT(","), true);
			for (int i = 0; i < inlines.Num(); i++)
			{
				TArray<FString> assets;
				inlines[i].ParseIntoArray(assets, TEXT("="), true);
				FString assetname = assets[0].TrimStartAndEnd();
				FString assetpath = assets[1].Replace(TEXT("\""), TEXT("")).TrimStartAndEnd();
				if (!BlueprintTypeGenMap.Contains(assets[0]))
				{
					BlueprintTypeGenMap.Add(assetname, assetpath);
				}
			}
		}

#pragma region 检查任务表中的特效
		FString MissionCSVPath = FPaths::ProjectContentDir() + "Quest";
		TArray<FString> MissionFiles;
		IFileManager::Get().FindFilesRecursive(MissionFiles, *MissionCSVPath, UTF8_TO_TCHAR("*.DT"), true, false, false);
		for (FString File : MissionFiles)
		{
			FString MissionContentString;
			if (FFileHelper::LoadFileToString(MissionContentString, *File))
			{
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MissionContentString);
				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(TEXT("nodes"));

					for (int32 i = 0; i < objArray.Num(); i++)
					{
						TSharedPtr<FJsonValue> value = objArray[i];
						TSharedPtr<FJsonObject> json = value->AsObject();
						if (json->HasField("_action"))
						{
							TSharedPtr<FJsonObject> actionjson = json->GetObjectField("_action");
							if (actionjson->HasField("effectname"))
							{
								FString effectname = actionjson->GetStringField("effectname");
								if (!FXResMap.Contains(effectname))
								{
									if (effectname.Contains("BP_"))
									{
										if (BlueprintTypeGenMap.Contains(effectname))
										{
											FXResMap.Add(effectname, BlueprintTypeGenMap[effectname]);
										}
									}
									else
									{
										if (FXResourceGenMap.Contains(effectname))
										{
											FXResMap.Add(effectname, FXResourceGenMap[effectname]);
										}
									}
								}
							}
						}
					}
				}
			}
		}
#pragma endregion

#pragma region 检查Lua中的特效
		TMap<FString, FString> ResourcePathMap;
		FString resourcemappath = FPaths::ProjectContentDir() + "LuaSource/Config/ResourcePathMap.lua";
		FString resourcemapcontent;
		if (FFileHelper::LoadFileToString(resourcemapcontent, *resourcemappath))
		{
			resourcemapcontent.ReplaceCharInline('\n', '|');
			resourcemapcontent.ReplaceCharInline('\r', '|');
			resourcemapcontent = resourcemapcontent.Replace(TEXT("|"), TEXT(""));
			TArray<FString> resourcemap;
			resourcemapcontent.ParseIntoArray(resourcemap, TEXT("-- FX"), true);
			TArray<FString> lines;
			resourcemap[1].ParseIntoArray(lines, TEXT("-- Mat"), true);
			TArray<FString> inlines;
			lines[0].TrimStartAndEnd().ParseIntoArray(inlines, TEXT(","), true);

			for (int i = 0; i < inlines.Num(); i++)
			{
				TArray<FString> assets;
				inlines[i].ParseIntoArray(assets, TEXT(" = FXAsset."), true);
				FString assetname = assets[1].TrimStartAndEnd();
				if (!FXResMap.Contains(assetname))
				{
					if (FXResourceGenMap.Contains(assetname))
					{
						FXResMap.Add(assetname, FXResourceGenMap[assetname]);
					}
				}
			}
		}
#pragma endregion

		//输出特效名称及其位置
		FString OutputResultStr = "ID,Name,Position\n";
		OutputResultStr += "number,string,string\n";
		auto OutputResultPathPicker = [](const FString& Title) {
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
			const FString DEFAULT_SAVE_PATH = FPaths::ProjectContentDir();
			FString OutFilename;
			DesktopPlatform->OpenDirectoryDialog(ParentWindowWindowHandle, Title, DEFAULT_SAVE_PATH, OutFilename);
			return OutFilename;
		};
		int count = 0;
		for (TPair<FString, FString>& element : FXResMap)
		{
			FString str_Count = FString::FromInt(count);
			OutputResultStr += str_Count + "," + element.Key + "," + element.Value + "\n";
			count++;
		}

		FString OutputResultPath = OutputResultPathPicker(FString::Printf(TEXT("Saved Output Result Folder")));
		FString OutputPath = OutputResultPath + "/PSAssetsStatistics.csv";
		if (!FFileHelper::SaveStringToFile(OutputResultStr, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			UE_LOG(LogTemp, Error, TEXT("Error to save art assets statistics on path: %s"), *OutputPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Check Completed!"));
		}
	}

	static void CheckCookedArtResource()
	{
		auto OutputResultPathPicker = [](const FString& Title) {
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
			const FString DEFAULT_SAVE_PATH = FPaths::ProjectContentDir();
			FString OutFilename;
			DesktopPlatform->OpenDirectoryDialog(ParentWindowWindowHandle, Title, DEFAULT_SAVE_PATH, OutFilename);
			return OutFilename;
		};

		FString CookedAssetsPath = FPaths::ProjectDir() + "Saved/Cooked/";
		FString OutputResultPath = OutputResultPathPicker(FString::Printf(TEXT("Saved Output Result Folder")));

		TArray<FString> Files;
		IFileManager::Get().FindFilesRecursive(Files, *CookedAssetsPath, UTF8_TO_TCHAR("*.*"), true, false, false);
		FString OutputResultStr = "Name,Position,Size\n";

		for (FString File : Files)
		{
			FString FileName = FPaths::GetCleanFilename(File);
			int64 FileSize = IFileManager::Get().FileSize(*File);
			File = File.Replace(*(FPaths::ProjectDir() + "Saved/Cooked/WindowsNoEditor/Atelier"), UTF8_TO_TCHAR(""));
			FString Output = FString::Format(TEXT("{0},{1},{2}\n"), { FileName, File, FileSize });
			OutputResultStr += Output;
		}

		FString OutputPath = OutputResultPath + "/ArtAssetsStatistics.csv";

		if (!FFileHelper::SaveStringToFile(OutputResultStr, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			UE_LOG(LogTemp, Error, TEXT("Error to save art assets statistics on path: %s"), *OutputPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Check Completed!"));
		}
	}

	static void CheckParticle()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		FARFilter Filter;
		Filter.ClassNames.Add(UParticleSystem::StaticClass()->GetFName());

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);
		Filter.bRecursiveClasses = true;
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add("/Game");
		int ErrorNum = 0;

		static const FName NAME_AssetCheck("AssetCheck");
		FMessageLog AssetCheckLog(NAME_AssetCheck);

		for (FAssetData Asset : AssetList)
		{
			UParticleSystem* Ps = Cast<UParticleSystem>(Asset.GetAsset());
			if (Ps && Ps->bUseFixedRelativeBoundingBox == false)
			{
				++ErrorNum;
				const FText Message = FText::Format(
					NSLOCTEXT("CheckParticle", "ParticleSystem_ShouldSetFixedRelativeBoundingBox", "Particle system {0} Should Use FixedRelativeBoundingBox."),
					FText::AsCultureInvariant(Ps->GetPathName()));
				AssetCheckLog.Error()
					->AddToken(FUObjectToken::Create(Ps))
					->AddToken(FTextToken::Create(Message));
			}
		}
		if (ErrorNum > 0)
			AssetCheckLog.Open(EMessageSeverity::Error);
	}

	//检查材质资源的合规性：
	static void CheckMaterial()
	{
		CheckRootMaterailInstance();
		CheckDefaultMaterialHasAdditional();
	}

	//检查角色文件夹下SkeletalMesh的材质的根Instance是否都在Default文件夹下
	//检查角色文件夹下SkeletalMesh的材质中的静态switch参数自材质是否和Root材质保持一致
	static void CheckRootMaterailInstance()
	{
		//筛选角色资源
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray< FAssetData > AssetList;
		FARFilter Filter;
		Filter.ClassNames.Add(USkeletalMesh::StaticClass()->GetFName());
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add("/Game/Assets/_Art/_Character");
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		//注册Log信息
		static const FName NAME_AssetCheck("AssetCheck");
		FMessageLog AssetCheckLog(NAME_AssetCheck);

		FString DefaultPath = "/Game/Assets/_Art/_Shader/_Base/_Character/Default";
		int ErrorNum = 0;
		for (FAssetData Asset : AssetList)
		{
			USkeletalMesh* Sm = Cast<USkeletalMesh>(Asset.GetAsset());
			TArray<FSkeletalMaterial> materials = Sm->Materials;
			for (int i = 0; i < materials.Num(); i++)
			{
				UMaterialInterface* baseParent = materials[i].MaterialInterface;

				while (true)
				{
					UMaterialInstance* toInstance = Cast<UMaterialInstance>(baseParent);
					if (toInstance && toInstance->Parent)
					{
						UMaterialInstance* parentToInstance = Cast<UMaterialInstance>(toInstance->Parent);
						if (parentToInstance)
							baseParent = parentToInstance;
						else
							break;
					}
					else
					{
						break;
					}
				}
				if (baseParent != nullptr)
				{
					FString OriginPathName = UKismetSystemLibrary::GetPathName(baseParent);
					if (OriginPathName.Len() > DefaultPath.Len() && OriginPathName.Left(DefaultPath.Len()) != DefaultPath)
					{
						++ErrorNum;
						AssetCheckLog.Error()
							->AddToken(FUObjectToken::Create(Sm))
							->AddToken(FTextToken::Create(FText::FromString("Root Material Instance: ")))
							->AddToken(FUObjectToken::Create(baseParent))
							->AddToken(FTextToken::Create(FText::FromString(" doesn't in the default folder")));
					}

					TArray<FMaterialParameterInfo> MineOutParameterInfo;
					TArray<FGuid> MineOutParameterIds;
					UMaterialInstance* mineInstance = Cast<UMaterialInstance>(materials[i].MaterialInterface);
					if (mineInstance != nullptr)
					{
						mineInstance->GetAllStaticSwitchParameterInfo(MineOutParameterInfo, MineOutParameterIds);

						TArray<FMaterialParameterInfo> ParentOutParameterInfo;
						TArray<FGuid> ParentOutParameterIds;
						baseParent->GetAllStaticSwitchParameterInfo(ParentOutParameterInfo, ParentOutParameterIds);
						if (MineOutParameterInfo == ParentOutParameterInfo)
						{
							for (FMaterialParameterInfo Info : MineOutParameterInfo)
							{
								bool minevalue;
								FGuid mineid;
								mineInstance->GetStaticSwitchParameterValue(Info, minevalue, mineid);

								bool basevalue;
								FGuid baseid;
								baseParent->GetStaticSwitchParameterValue(Info, basevalue, baseid);

								if (minevalue != basevalue)
								{
									++ErrorNum;
									AssetCheckLog.Error()
										->AddToken(FUObjectToken::Create(mineInstance))
										->AddToken(FTextToken::Create(FText::FromString(" has differenc switch parameter with root:" + Info.ToString())))
										->AddToken(FUObjectToken::Create(baseParent));
								}
							}
						}
						else
						{
							++ErrorNum;
							AssetCheckLog.Error()
								->AddToken(FUObjectToken::Create(Sm))
								->AddToken(FTextToken::Create(FText::FromString(" has differenc switch parameter with root")));
						}
					}
				}
			}
		}
		if (ErrorNum > 0)
			AssetCheckLog.Open(EMessageSeverity::Error);
	}

	//所有Default文件夹下的材质在Aditional文件夹下是否有对应的替换材质
	static void CheckDefaultMaterialHasAdditional()
	{
		//筛选角色资源
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

		TArray< FAssetData > DefaultAssetList;
		FARFilter DefaultFilter;
		DefaultFilter.ClassNames.Add(UMaterialInstanceConstant::StaticClass()->GetFName());
		DefaultFilter.PackagePaths.Add("/Game/Assets/_Art/_Shader/_Base/_Character/Default");
		AssetRegistryModule.Get().GetAssets(DefaultFilter, DefaultAssetList);

		TArray< FAssetData > AdditionalAssetList;
		FARFilter AdditionalFilter;
		AdditionalFilter.ClassNames.Add(UMaterialInstanceConstant::StaticClass()->GetFName());
		AdditionalFilter.PackagePaths.Add("/Game/Assets/_Art/_Shader/_Base/_Character/Additional");
		AssetRegistryModule.Get().GetAssets(AdditionalFilter, AdditionalAssetList);

		TArray<FString> AdditionalAssetNameList;
		TMap<FString, FAssetData> AdditionalAssetDic;
		for (FAssetData AdditionalAsset : AdditionalAssetList)
		{
			AdditionalAssetNameList.Add(AdditionalAsset.AssetName.ToString());
			AdditionalAssetDic.Add(AdditionalAsset.AssetName.ToString(), AdditionalAsset);
		}

		//注册Log信息
		static const FName NAME_AssetCheck("AssetCheck");
		FMessageLog AssetCheckLog(NAME_AssetCheck);

		int ErrorNum = 0;
		for (FAssetData DefaultAsset : DefaultAssetList)
		{
			UMaterialInstanceConstant* DefaultMic = Cast<UMaterialInstanceConstant>(DefaultAsset.GetAsset());
			TArray<FString> NameList;
			TArray<FString> ConfigList;
			FString DefaultName = DefaultMic->GetFName().ToString();
			NameList.Add(DefaultName + "_Hit");
			NameList.Add(DefaultName + "_Avatar");
			NameList.Add(DefaultName + "_Death");
			NameList.Add(DefaultName + "_Rim");
			ConfigList.Add("Hit");
			ConfigList.Add("Avatar");
			ConfigList.Add("Death");
			ConfigList.Add("Rim");

			for (int i = 0; i < 4; i++)
			{
				FString name = NameList[i];
				FString config = ConfigList[i];

				if (!AdditionalAssetNameList.Contains(name))
				{
					++ErrorNum;
					AssetCheckLog.Error()
						->AddToken(FUObjectToken::Create(DefaultMic))
						->AddToken(FTextToken::Create(FText::FromString("don't have [" + config + "] material in additional folder")));
				}
				else
				{
					FAssetData AdditionalAsset = AdditionalAssetDic.FindRef(name);
					UMaterialInstanceConstant* AdditionalMic = Cast<UMaterialInstanceConstant>(AdditionalAsset.GetAsset());
					if (!CompareDefaultAndAdditional(DefaultMic, AdditionalMic, config))
					{
						++ErrorNum;
						AssetCheckLog.Error()
							->AddToken(FUObjectToken::Create(AdditionalMic))
							->AddToken(FTextToken::Create(FText::FromString(" don't have correct static switch parameter by")))
							->AddToken(FUObjectToken::Create(DefaultMic));
					}
				}
			}
		}
		if (ErrorNum > 0)
			AssetCheckLog.Open(EMessageSeverity::Error);
	}

	static bool CompareDefaultAndAdditional(UMaterialInstanceConstant* DefaultMic, UMaterialInstanceConstant* AdditionalMic, FString config)
	{
		bool result = false;

		TArray<FMaterialParameterInfo> DefaultOutParameterInfo;
		TArray<FGuid> DefaultOutParameterIds;
		DefaultMic->GetAllStaticSwitchParameterInfo(DefaultOutParameterInfo, DefaultOutParameterIds);

		TArray<FMaterialParameterInfo> AdditionalOutParameterInfo;
		TArray<FGuid> AdditionalOutParameterIds;
		AdditionalMic->GetAllStaticSwitchParameterInfo(AdditionalOutParameterInfo, AdditionalOutParameterIds);

		if (DefaultOutParameterInfo == AdditionalOutParameterInfo)
		{
			for (FMaterialParameterInfo Info : DefaultOutParameterInfo)
			{
				bool defaultvalue;
				FGuid defaultid;
				DefaultMic->GetStaticSwitchParameterValue(Info, defaultvalue, defaultid);

				bool additionalvalue;
				FGuid additionalid;
				AdditionalMic->GetStaticSwitchParameterValue(Info, additionalvalue, additionalid);

				if (defaultvalue != additionalvalue)
				{
					if (config == "Avatar")
					{
						if (Info.Name != "LightFolloCamera")
						{
							result = false;
							break;
						}
					}
					else if (config == "Death")
					{
						if (Info.Name != "UseDeathDissolve")
						{
							result = false;
							break;
						}
					}
					else if (config == "Hit")
					{
						if (Info.Name != "UseAdditionalRim")
						{
							result = false;
							break;
						}
					}
					else if (config == "Rim")
					{
						if (Info.Name != "UseAdditionalRim")
						{
							result = false;
							break;
						}
					}
				}
			}
		}
		else
		{
			result = false;
		}

		return result;
	}

	static void CheckAnimation()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray< FAssetData > AssetList;
		FARFilter Filter;
		Filter.ClassNames.Add(UAnimSequence::StaticClass()->GetFName());
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add("/Game/Assets/_Art/_Character");
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		static const FName NAME_AssetCheck("AssetCheck");
		FMessageLog AssetCheckLog(NAME_AssetCheck);

		FString DefaultPath = "/Game/Assets/_Art/_Shader/_Base/_Character/Default";
		int ErrorNum = 0;
		for (FAssetData Asset : AssetList)
		{
			if (Asset.AssetName.ToString().Right(5) == "_Anim")
			{
				UAnimSequence* Sequence = Cast<UAnimSequence>(Asset.GetAsset());
				if (Sequence->GetAdditiveAnimType() == EAdditiveAnimationType::AAT_None)
				{
					++ErrorNum;
					AssetCheckLog.Error()
						->AddToken(FUObjectToken::Create(Sequence))
						->AddToken(FTextToken::Create(FText::FromString(" Morpher animation must set additive!")));
				}
			}
		}
		if (ErrorNum > 0)
			AssetCheckLog.Open(EMessageSeverity::Error);
	}

	static void CheckPhysAsset()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray< FAssetData > AssetList;
		FARFilter Filter;
		Filter.ClassNames.Add(UPhysicsAsset::StaticClass()->GetFName());
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add("/Game");
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		static const FName NAME_AssetCheck("AssetCheck");
		FMessageLog AssetCheckLog(NAME_AssetCheck);

		int ErrorNum = 0;
		for (FAssetData Asset : AssetList)
		{
			UPhysicsAsset* PhysAsset = Cast<UPhysicsAsset>(Asset.GetAsset());
			FString LogStr;
			bool Rst = FEDProgrameCommonChecker::CheckPhysicsAssetSafely(PhysAsset, LogStr);
			if (!Rst)
			{
				++ErrorNum;
				AssetCheckLog.Error()
					->AddToken(FUObjectToken::Create(PhysAsset))
					->AddToken(FTextToken::Create(FText::FromString(LogStr)));
			}
		}
		if (ErrorNum > 0)
			AssetCheckLog.Open(EMessageSeverity::Error);
	}

	static void CheckStaticMeshCollision()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		TArray< FAssetData > AssetList;
		FARFilter Filter;
		Filter.ClassNames.Add(UStaticMesh::StaticClass()->GetFName());
		Filter.bRecursivePaths = true;
		Filter.PackagePaths.Add("/Game/Assets/_Art/_Level");
		AssetRegistryModule.Get().GetAssets(Filter, AssetList);

		for (FAssetData Asset : AssetList)
		{
			UStaticMesh* Mesh = Cast<UStaticMesh>(Asset.GetAsset());
#if WITH_CHAOS
			if (Mesh->BodySetup && !Mesh->BodySetup->bNeverNeedsCookedCollisionData && Mesh->ComplexCollisionMesh && (Mesh->BodySetup->ChaosTriMeshes.Num() == 0 || (Mesh->NavCollision && Mesh->NavCollision->GetTriMeshCollision().VertexBuffer.Num() == 0 && Mesh->NavCollision->GetConvexCollision().VertexBuffer.Num() == 0)))
#else
			if (Mesh->BodySetup && !Mesh->BodySetup->bNeverNeedsCookedCollisionData && Mesh->ComplexCollisionMesh && (Mesh->BodySetup->TriMeshes.Num() == 0 || (Mesh->NavCollision && Mesh->NavCollision->GetTriMeshCollision().VertexBuffer.Num() == 0 && Mesh->NavCollision->GetConvexCollision().VertexBuffer.Num() == 0)))
#endif
			{
				Mesh->Build();
				Mesh->MarkPackageDirty();
			}
		}
	}

	static void RepairAnimEvents()
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
		FARFilter FilterAnimSequences;
		FilterAnimSequences.ClassNames.Add(UAnimSequenceBase::StaticClass()->GetFName());
		FilterAnimSequences.bRecursivePaths = true;
		FilterAnimSequences.bRecursiveClasses = true;
		FilterAnimSequences.PackagePaths.Add("/Game");

		//Filter.bRecursiveClasses = true;
		TArray< FAssetData > AssetList;
		AssetRegistryModule.Get().GetAssets(FilterAnimSequences, AssetList);

		TMap<FName, FName> AssetRegistry;
		for (FAssetData Asset : AssetList)
		{
			bool bError = false;
			TArray<FName> FileDependencies;
			AssetRegistryModule.Get().GetDependencies(*Asset.PackageName.ToString(), FileDependencies);
			for (auto FileDependence : FileDependencies)
			{
				FString strFileDependence(FileDependence.ToString());
				if (strFileDependence.Right(15) == "_AnimDataEffect" || strFileDependence.Right(14) == "_AnimDataAudio" || strFileDependence.Right(14) == "_AnimDataEvent")
				{
					bError = true;
					break;
				}
			}

			if (bError)
			{
				UAnimSequenceBase* AnimSequence = Cast<UAnimSequenceBase>(Asset.GetAsset());
				if (AnimSequence && AnimSequence->Notifies.Num() > 0)
				{
					AnimSequence->Notifies.Empty();
					AnimSequence->NotifyDatas.Empty();
					AnimSequence->PostEditChange();
					AnimSequence->MarkPackageDirty();
				}
			}
		}
	}

	static void ExportAnimationReference()
	{
		TArray<FString> Character;
		TArray<FString> CustomzationPanel;
		TArray<FString> Quest;
		TArray<FString> Skill;
		ExportCharacterInfoCtrlAnimName(Character);
		ExportCustomzationPanelAnimName(CustomzationPanel);
		ExportQuestAnimName(Quest);
		ExportSkillAnimName(Skill);

		TArray<FString> Result;
		Result.Append(Character);
		Result.Append(CustomzationPanel);
		Result.Append(Quest);
		Result.Append(Skill);

		//输出特效名称及其位置
		FString OutputResultStr = "AnimName,AnimPath,File,Index\n";
		auto OutputResultPathPicker = [](const FString& Title) {
			IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
			const void* ParentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
			const FString DEFAULT_SAVE_PATH = FPaths::ProjectContentDir();
			FString OutFilename;
			DesktopPlatform->OpenDirectoryDialog(ParentWindowWindowHandle, Title, DEFAULT_SAVE_PATH, OutFilename);
			return OutFilename;
		};

		for (FString info : Result)
		{
			OutputResultStr += info + "\n";
		}

		FString OutputResultPath = OutputResultPathPicker(FString::Printf(TEXT("Saved Output Result Folder")));
		FString OutputPath = OutputResultPath + "/AnimReferenceInfo.csv";
		if (!FFileHelper::SaveStringToFile(OutputResultStr, *OutputPath, FFileHelper::EEncodingOptions::ForceUTF8WithoutBOM))
		{
			UE_LOG(LogTemp, Error, TEXT("Error to save art animation reference info on path: %s"), *OutputPath);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Check Completed!"));
		}
	}

	//输出角色展示使用到的动画资源信息
	static void ExportCharacterInfoCtrlAnimName(TArray<FString>& Output)
	{
		Output.Add(FString("PlayShowSystem_Idle,,GlobalGameConfig.lua,CharacterInfoCtrlAnimName.Idle"));
		Output.Add(FString("PlayShow,,GlobalGameConfig.lua,CharacterInfoCtrlAnimName.PlayShow"));
	}

	//输出捏人界面使用到的动画资源信息
	static void ExportCustomzationPanelAnimName(TArray<FString>& Output)
	{
		FString FilePath = FPaths::ProjectContentDir() + TEXT("Table/GameConfig.txt");
		FString configString = "";

		if (!FFileHelper::LoadFileToString(configString, *FilePath))

		{
			UE_LOG(LogAtelier, Error, TEXT("Can not load GameConfig.txt!!!"));
			return;
		}


		FString left = "";
		FString right = "";
		configString.Split("GameConfig.CustomzatioAnim", &left, &right);
		right.Split("--GameConfig.CustomzatioAnim", &left, &right);

		TArray<FString> configArray;
		configString.ParseIntoArray(configArray, TEXT("\r\n"), false);
		for (auto& Path : configArray)
		{
			if (Path.Contains(TEXT("anim = \"")))
			{
				Path.Split(TEXT("anim = \""), &left, &right);
				right.Split(TEXT("\""), &left, &right);
				if (left != "")
				{
					left.Append(TEXT(",,,GameConfig.CustomzatioAnim"));
					Output.Add(left);
				}
			}
			if (Path.Contains(TEXT("morpher = \"")))
			{
				Path.Split(TEXT("morpher = \""), &left, &right);
				right.Split(TEXT("\""), &left, &right);
				if (left != "")
				{
					left.Append(TEXT(",,,GameConfig.CustomzatioAnim"));
					Output.Add(left);
				}
			}
		}

		//for (auto Path : Output)
		//{
		//	UE_LOG(LogTemp, Log, TEXT("Xuweiwei log : %s"), *Path);
		//}
	}

	//输出剧情对话使用到的动画资源信息
	static void ExportQuestAnimName(TArray<FString>& Output)
	{
		Output.Add(TEXT("story_pose_0,,,"));

		FString MissionCSVPath = FPaths::ProjectContentDir() + "Quest";
		TArray<FString> MissionFiles;
		FString basePose{ TEXT("story_pose_0") };
		IFileManager::Get().FindFilesRecursive(MissionFiles, *MissionCSVPath, UTF8_TO_TCHAR("*.DT"), true, false, false);
		TArray<FString> animKeys{ "emotionalPose","actionName", "mouthAction", "eyeAction" };
		for (FString File : MissionFiles)
		{
			FString MissionContentString;
			if (FFileHelper::LoadFileToString(MissionContentString, *File))
			{
				TSharedPtr<FJsonObject> JsonObject;
				TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(MissionContentString);
				if (FJsonSerializer::Deserialize(Reader, JsonObject))
				{
					TArray<TSharedPtr<FJsonValue>> objArray = JsonObject->GetArrayField(TEXT("nodes"));

					for (int32 i = 0; i < objArray.Num(); i++)
					{
						TSharedPtr<FJsonValue> value = objArray[i];
						TSharedPtr<FJsonObject> json = value->AsObject();
						for (auto iAnimKey : animKeys)
						{
							if (json->HasField(iAnimKey))
							{
								FString poseName = json->GetStringField(iAnimKey);
								Output.Add(poseName + TEXT(",,") + (File)+TEXT(",") + (json->GetStringField("$id")));
								// iterate all previous nodes to find pose transition
								if (iAnimKey == "emotionalPose"
									&& i > 0)
								{
									for (int32 j = i - 1; j >= 0; j--)
									{
										TSharedPtr<FJsonValue> prevValue = objArray[j];
										TSharedPtr<FJsonObject> prevJson = prevValue->AsObject();
										if (prevJson->HasField("emotionalPose"))
										{
											FString prevPoseName = prevJson->GetStringField("emotionalPose");
											if (prevPoseName != poseName)
											{
												Output.Add(prevPoseName + (poseName.RightChop(10)) + (TEXT(",,")) + (File)+TEXT(",") + (json->GetStringField("$id")));
											}
											break;
										}
										if (j == 0)
										{
											if (poseName != basePose)
											{
												Output.Add(TEXT("story_pose_0") + (poseName.RightChop(10)) + (TEXT(",,")) + (File)+TEXT(",") + (json->GetStringField("$id")));
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	//输出技能使用到的动画资源信息
	static void ExportSkillAnimName(TArray<FString>& Output)
	{
		FString SkillEffectCSVPath = FPaths::ProjectContentDir() + "Table/Skill";
		TArray<FString> Files;
		IFileManager::Get().FindFilesRecursive(Files, *SkillEffectCSVPath, UTF8_TO_TCHAR("*.csv"), true, false, false);
		for (FString File : Files)
		{
			FString FileName = FPaths::GetCleanFilename(File);
			if (FileName.Contains("Skills."))
			{
				FString SkillContentString;
				if (FFileHelper::LoadFileToString(SkillContentString, *File))
				{
					const FCsvParser Parser(SkillContentString);
					const auto& Rows = Parser.GetRows();
					for (int32 RowIndex = 1; RowIndex < Rows.Num(); RowIndex++)
					{
						const TArray<const TCHAR*>& Cells = Rows[RowIndex];
						FString Id = Cells[2];
						FString Actions = Cells[3];
						TArray<FString> strs;
						Actions.ParseIntoArray(strs, TEXT(","), true);
						for (int i = 0; i < strs.Num(); i++)
						{
							if (strs[i].StartsWith("AnimReference="))
							{
								FString path = strs[i].Replace(TEXT("AnimReference="), TEXT("")).Replace(TEXT("'"), TEXT(""));
								if (path != "None")
								{
									TArray<FString> infos;
									path.ParseIntoArray(infos, TEXT("."), true);
									FString animName = infos[1];
									FString output = animName + "," + path + "," + File + "," + Id;
									Output.Add(output);
								}
							}
						}
					}
				}
			}
		}
	}

	static void GenLipSyncAnim()
	{
		FString log;
		FEdCommonGenerate::AutoLipSycnAnimGen(log);
	}

public:
	static TArray<FString> CurrentSelectedPaths;

public:
	static TSharedRef< SWidget > GenerateOpenBlueprintMenuContent()
	{
		const bool bShouldCloseWindowAfterMenuSelection = true;
		FMenuBuilder MenuBuilder(bShouldCloseWindowAfterMenuSelection, nullptr);
		// ~Begin: Open Main Level
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Main Level"),
				FText::FromString("Open Main Level"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenMainLevel)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
		}
		// ~End: Open Main Level

		// ~Begin: Workshop Editor
		/*{
			MenuBuilder.BeginSection("WorkshopEditor", FText::FromString("WorkshopEditor"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Workshop Editor"),
				FText::FromString("Open Workshop Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenWorkshopEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Furniture Editor"),
				FText::FromString("Open Furniture Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenFurnitureEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}*/
		// ~End: Workshop Editor

		// ~Begin: Skill Editor
		// SkillEditor Entrance
		/*MenuBuilder.BeginSection("SkillEditor", FText::FromString("SkillEditor"));
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Skill Editor"),
				FText::FromString("Open Skill Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenSkillEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
		}
		MenuBuilder.EndSection();*/

		MenuBuilder.BeginSection("Backend Utilis", FText::FromString("Backend Utilis"));
		{

			MenuBuilder.AddMenuEntry(
				FText::FromString("Export San Data Only"),
				FText::FromString("Export San Data Only"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportSanData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Export All Backend Data"),
				FText::FromString("Export All Backend Data"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportAllBackendData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
		}
		MenuBuilder.EndSection();

		// ~Begin: Map Editor
		// MapEditor Entrance
		MenuBuilder.BeginSection("MapEditor", FText::FromString("MapEditor"));
		{
			MenuBuilder.AddMenuEntry(
				FText::FromString("Generate Jump Links"),
				FText::FromString("Generate Jump Links"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenerateJumpLinks)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Export Nav Mesh Data"),
				FText::FromString("Export Nav Mesh Data"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportNavMeshData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

		}
		MenuBuilder.EndSection();

		// ~End: Map Editor

		// ~Begin: Sockets Editor
		{
			MenuBuilder.BeginSection("CustomSocketsEditor", FText::FromString("CustomSocketsEditor"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Accessories Editor"),
				FText::FromString("Open Accessories Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenAccessoriesEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Interactive Editor"),
				FText::FromString("Open Interactive Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenInteractiveEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Avatar Editor"),
				FText::FromString("Open Avatar Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenAvatarEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}
		// ~End: Sockets Editor

		MenuBuilder.BeginSection("Quest Tools", FText::FromString("Quest Tools"));
		MenuBuilder.AddMenuEntry(
			FText::FromString("Gen Stage Monster Data"),
			FText::FromString("Gen Stage Monster Data"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportStageMonsterCsvData)),
			NAME_None,
			EUserInterfaceActionType::Button
		);
		MenuBuilder.EndSection();


		// ~Begin: Lua Editor
		{
			MenuBuilder.BeginSection("Lua Tools", FText::FromString("Lua Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen Blueprint Type"),
				FText::FromString("Gen Blueprint Type"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenBlueprintTypeRefForLua)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen ParticleSystem Type"),
				FText::FromString("Gen ParticleSystem Type"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenParticleSystemRefForLua)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen UI Type"),
				FText::FromString("Gen UI Type"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenUIRefForLua)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Export All AnimationData"),
				FText::FromString("Export All AnimationData"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportAllAnimationData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen Pbc"),
				FText::FromString("Gen Pbc"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEdCommonGenerate::AutoGenPbcLocal)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen XDApp Pbc"),
				FText::FromString("Gen XDApp Pbc"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenXDAppPbc)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Convert Anim Event"),
				FText::FromString("Convert Anim Event"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ConvertAnimEvent)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen UEnum To Proto"),
				FText::FromString("Gen UEnum To Proto"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenerateUEnumToProto)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen QuestClassNameMap"),
				FText::FromString("Gen QuestClassNameMap"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenQuestClassNameMap)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen AvatarPersonality LuaTable"),
				FText::FromString("Gen AvatarPersonality LuaTable"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenPersonalityLuaTable)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Switch Anim NotifyData"),
				FText::FromString("Switch Anim NotifyData"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::SwitchAnimNotifyData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Refresh Anim NotifyData"),
				FText::FromString("Refresh Anim NotifyData"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::RefreshAnimNotifyData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Deduplication Anim NotifyData"),
				FText::FromString("Deduplication Anim NotifyData"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::DeduplicationAnimNotifyData)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}
		// ~End: Lua Editor

		// ~Begin: Anim Tools
		{
			/*MenuBuilder.BeginSection("Anim Tools", FText::FromString("Anim Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen AnimFile Path"),
				FText::FromString("Gen AnimFile Path"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CreateAnimPath)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			//MenuBuilder.AddMenuEntry(
			//	FText::FromString("Gen AnimComposite From AnimSequence"),
			//	FText::FromString("Gen AnimComposite From AnimSequence. The new AnimComposite File will use the AnimSequence's filename and the original AnimSequence's filename will add postfix '__'"),
			//	FSlateIcon(),
			//	FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenerateAnimComposite)),
			//	NAME_None,
			//	EUserInterfaceActionType::Button
			//);
			MenuBuilder.EndSection();*/
		}
		// ~End: Anim Tools

		// ~Begin: Audio Tools
		{
			MenuBuilder.BeginSection("Audio Tools", FText::FromString("Audio Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Open Audio Editor"),
				FText::FromString("Open Audio Editor"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OpenAudioEditor)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen Audio Resource"),
				FText::FromString("Gen Audio Resource"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::AutoGenAudioRefForLua)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}
		// ~End: Audio Tools

		{
			MenuBuilder.BeginSection("Check Resource Tools", FText::FromString("Check Resource Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Check Particle"),
				FText::FromString("Check Particle"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckParticle)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Check Material"),
				FText::FromString("Check Material"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckMaterial)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Check Animation"),
				FText::FromString("Check Animation"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckAnimation)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Check PhysAsset"),
				FText::FromString("Check PhysAsset"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckPhysAsset)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Check StaticMeshCollision"),
				FText::FromString("Check StaticMeshCollision"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckStaticMeshCollision)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}

		// Pre check tools
		{
			MenuBuilder.BeginSection("Pre-check Tools", FText::FromString("Pre-check Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Pre-check Model"),
				FText::FromString("Pre-check Model"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::PreCheckModel)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.AddMenuEntry(
				FText::FromString("Pre-check Texture"),
				FText::FromString("Pre-check Texture"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::PreCheckTexture)),
				NAME_None,
				EUserInterfaceActionType::Button
			);
			MenuBuilder.EndSection();
		}

		// Other tools
		{
			MenuBuilder.BeginSection("Other Tools", FText::FromString("Other Tools"));
			MenuBuilder.AddMenuEntry(
				FText::FromString("Init External Art Resource"),
				FText::FromString("Init External Art Resource"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::InitExternalArtRes)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Check Cooked Art Resource"),
				FText::FromString("Check Cooked Art Resource"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckCookedArtResource)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Check FX Assets"),
				FText::FromString("Check FX Assets"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::CheckFXAssets)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Export Animation Reference"),
				FText::FromString("Export Animation Reference"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::ExportAnimationReference)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Repair Anim Events"),
				FText::FromString("Repair Anim Events"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::RepairAnimEvents)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.AddMenuEntry(
				FText::FromString("Gen LipSyncAnim"),
				FText::FromString("Gen LipSyncAnim"),
				FSlateIcon(),
				FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenLipSyncAnim)),
				NAME_None,
				EUserInterfaceActionType::Button
			);

			MenuBuilder.EndSection();
		}
		return MenuBuilder.MakeWidget();
	}

	static void CreateToolbarEntries(FToolBarBuilder& ToolbarBuilder)
	{
		ToolbarBuilder.BeginSection("Atelier");

		ToolbarBuilder.AddComboButton(
			FUIAction(),
			FOnGetContent::CreateStatic(&FEditorMenuExtensionImpl::GenerateOpenBlueprintMenuContent),
			FText::FromString("Atelier Tools"),
			FText::FromString("List Of Atelier Tools"),
			//FSlateIcon()
			//FSlateIcon(FEditorStyle::GetStyleSetName(), "ConfigEditor.TabIcon")
			FSlateIcon(FEditorStyle::GetStyleSetName(), "LevelEditor.OpenLevelBlueprint")
		);
		ToolbarBuilder.EndSection();
	}

	static void OnMenuExecute()
	{}

	static void CreateMenuEntries(FMenuBuilder& MenuBuilder)
	{
		FUIAction Action(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::OnMenuExecute));

		MenuBuilder.BeginSection(TEXT("Atelier"), FText::FromString("Test"));

		MenuBuilder.AddMenuEntry(
			FText::FromString("Menu Test2"),
			FText::FromString("Menu Test Tips"),
			FSlateIcon(),
			Action,
			NAME_None,
			EUserInterfaceActionType::Button);

		MenuBuilder.EndSection();
	}

	static TSharedRef<FExtender> CreateContentBrowserMenuEntries(const TArray<FString>& Path)
	{
		FEditorMenuExtensionImpl::CurrentSelectedPaths = Path;
		// Create extender that contains a delegate that will be called to get information about new context menu items
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		// Create a Shared-pointer delegate that keeps a weak reference to object
		// "NewFolder" is a hook name that is used by extender to identify externders that will extend path context menu
		MenuExtender->AddMenuExtension("NewFolder", EExtensionHook::After, TSharedPtr<FUICommandList>(),
			FMenuExtensionDelegate::CreateStatic(&FEditorMenuExtensionImpl::_CreateContentBrowserMenuEntries));
		return MenuExtender.ToSharedRef();
	}

	static void _CreateContentBrowserMenuEntries(FMenuBuilder& MenuBuilder)
	{
		MenuBuilder.AddMenuEntry(
			FText::FromString("Convert AnimSequences to AnimComposites"),
			FText::FromString("Convert AnimSequences to AnimComposites"),
			FSlateIcon(),
			FUIAction(FExecuteAction::CreateStatic(&FEditorMenuExtensionImpl::GenerateAnimComposite)),
			NAME_None,
			EUserInterfaceActionType::Button
		);
	}
};

TArray<FString> FEditorMenuExtensionImpl::CurrentSelectedPaths = TArray<FString>();

void FEditorMenuExtensions::ExtendMenus()
{
	if (!GIsEditor)
		return;
	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedRef<FExtender> Extender(new FExtender());
	Extender->AddMenuExtension(
		TEXT("EditMain"),
		EExtensionHook::After,
		nullptr,
		FMenuExtensionDelegate::CreateStatic(&FEditorMenuExtensionImpl::CreateMenuEntries)
	);
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
}

void FEditorMenuExtensions::ExtendToolbars()
{
	if (!GIsEditor)
		return;
	FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	TSharedRef<FExtender> Extender(new FExtender());
	Extender->AddToolBarExtension(
		TEXT("Game"),
		EExtensionHook::After,
		nullptr,
		FToolBarExtensionDelegate::CreateStatic(&FEditorMenuExtensionImpl::CreateToolbarEntries)
	);
	Extender->AddToolBarExtension(
		TEXT("Game"),
		EExtensionHook::After,
		nullptr,
		FToolBarExtensionDelegate::CreateStatic(&FEditorExtensionCustomEditorList::CreateToolbarEntries)
	);

	LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(Extender);
}

void FEditorMenuExtensions::ExtendContexMenu()
{

}

void FEditorMenuExtensions::ExtendContentBrowserContextMenu()
{
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
	TArray<FContentBrowserMenuExtender_SelectedPaths>& MenuExtenderDelegates = ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	// Create new delegate that will be called to provide our menu extener
	MenuExtenderDelegates.Add(FContentBrowserMenuExtender_SelectedPaths::CreateStatic(&FEditorMenuExtensionImpl::CreateContentBrowserMenuEntries));
}
