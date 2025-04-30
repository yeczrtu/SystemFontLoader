// Copyright (c) 2025 yeczrtu
// Released under the MIT license
// https://opensource.org/licenses/mit-license.php

#pragma once

#include "Modules/ModuleManager.h"

class FSystemFontLoaderModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
