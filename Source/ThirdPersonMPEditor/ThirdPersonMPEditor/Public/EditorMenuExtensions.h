#pragma once

#include "Atelier/System/Project.h"

class FEditorMenuExtensions
{
public:
	static void ExtendMenus();
	static void ExtendToolbars();
	static void ExtendContexMenu();
	static void ExtendContentBrowserContextMenu();
};

struct FInteractiveReadTableInfo
{
public:
	int32 id;
	FString tableName;
	FString resourcePath;
	FString model;
	FString interactiveType;
};
