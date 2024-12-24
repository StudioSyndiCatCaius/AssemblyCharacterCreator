// Copyright Studio Syndicat 2020. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "UnrealEd.h"



class FAssemblyEditorModule : public IModuleInterface
{
public:

	TSharedPtr<FSlateStyleSet> StyleSet;

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
