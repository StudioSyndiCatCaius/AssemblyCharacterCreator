// Copyright Studio Syndicat 2020. All Rights Reserved.

#include "AssemblyEditor.h"
#include "Modules/ModuleManager.h"
#include "Modules/ModuleInterface.h"
// 5.3 #include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/AssetRegistryModule.h"



#define LOCTEXT_NAMESPACE "TutorialEditor"

void FAssemblyEditorModule::StartupModule()
{
   

}

void FAssemblyEditorModule::ShutdownModule()
{
    //UE_LOG(TutorialEditor, Warning, TEXT("AssemblyEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FAssemblyEditorModule, AssemblyEditor)