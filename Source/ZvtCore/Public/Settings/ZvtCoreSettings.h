#pragma once

#include "Engine/DeveloperSettings.h"
#include "ZvtCoreSettings.generated.h"

UCLASS(Config=Editor, defaultconfig, meta=(DisplayName="Zvt Core"))
class ZVTCORE_API UZvtCoreSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Logging")
	bool bShowPIEIdentifierInLogs = false;

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); }
};
