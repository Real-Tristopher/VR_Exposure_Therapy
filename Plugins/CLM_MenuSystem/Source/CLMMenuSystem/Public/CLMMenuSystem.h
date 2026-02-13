#pragma once

#include "Modules/ModuleManager.h"

class CLMMenuSystemModule: public IModuleInterface
{
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};