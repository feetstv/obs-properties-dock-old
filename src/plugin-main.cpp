/*
OBS Properties Dock
Copyright (C) 2022 Sebastian Beckmann

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/


#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>

#include "properties-dock.hpp"
#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	obs_frontend_add_dock(new PropertiesDock(
		static_cast<QMainWindow *>(obs_frontend_get_main_window())));

	blog(LOG_INFO, "Properties dock loaded successfully. Version %s",
	     PLUGIN_VERSION);
	return true;
}

void obs_module_unload() {}
