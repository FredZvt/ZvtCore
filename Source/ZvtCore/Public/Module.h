#pragma once

#include "Modules/ModuleInterface.h"

class UFont;

/**
 * Module interface for ZvtCore.
 *
 * Loads and pins the RobotoMono variable font asset during StartupModule so that
 * other systems (e.g. UZvtDebugManager) can retrieve it without independently
 * managing the asset lifetime. Consumers should call GetMonoFont() rather than
 * loading the font themselves.
 */
class FZvtCoreModule final : public IModuleInterface
{

public:

    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

    /**
     * Returns the RobotoMono variable font asset loaded at startup.
     * The returned pointer is guaranteed non-null while the module is loaded.
     */
    UFont* GetMonoFont() const { return MonoFont; }

private:
    
    UFont* MonoFont = nullptr;
};
