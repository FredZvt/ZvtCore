#include "Module.h"
#include "Engine/Font.h"
#include "ZvtCoreLogCategories.h"

static const TCHAR* MonoFontPath = TEXT("/ZvtCore/Fonts/RobotoMono-VariableFont_wght_Font.RobotoMono-VariableFont_wght_Font");

void FZvtCoreModule::StartupModule()
{
    MonoFont = Cast<UFont>(StaticLoadObject(UFont::StaticClass(), nullptr, MonoFontPath));
    if (MonoFont)
    {
        MonoFont->AddToRoot();
    }
    else
    {
        UE_LOG(LogZvtCore, Error, TEXT("FZvtCoreModule: Failed to load mono font at '%s'. Debug messages will not render."), MonoFontPath);
    }
}

void FZvtCoreModule::ShutdownModule()
{
}

IMPLEMENT_MODULE(FZvtCoreModule, ZvtCore)
