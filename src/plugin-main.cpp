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

PropertiesDock *properties;

int selectedItemsCount;
OBSWeakSourceAutoRelease currentScene;

/* This is stolen straight from obs-websocket */
template<typename T>
T *calldata_get_pointer(const calldata_t *data, const char *name)
{
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return reinterpret_cast<T *>(ptr);
}

void SceneItemSelectSignal(void *, calldata_t *data)
{
	selectedItemsCount++;

	OBSSceneItem item = calldata_get_pointer<obs_sceneitem_t>(data, "item");
	OBSSource source = obs_sceneitem_get_source(item);

	properties->SetSource(source);
}

void SceneItemDeselectSignal(void *, calldata_t *)
{
	selectedItemsCount--;
	if (selectedItemsCount == 0) {
		properties->SetSource(nullptr);
	}
}

void FrontendEvent(enum obs_frontend_event event, void *)
{
	if (event != OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED)
		return;

	OBSSourceAutoRelease currentSceneSource =
		obs_frontend_preview_program_mode_active()
			? obs_frontend_get_current_preview_scene()
			: obs_frontend_get_current_scene();

	/* Remove signal from old source */
	if (currentScene && !obs_weak_source_expired(currentScene)) {
		OBSSourceAutoRelease oldScene =
			obs_weak_source_get_source(currentScene);
		signal_handler_t *sh = obs_source_get_signal_handler(oldScene);
		signal_handler_disconnect(sh, "item_select",
					  SceneItemSelectSignal, nullptr);
		signal_handler_disconnect(sh, "item_deselect",
					  SceneItemDeselectSignal, nullptr);
	}

	/* Add signal to new source */
	signal_handler_t *sh =
		obs_source_get_signal_handler(currentSceneSource);
	signal_handler_connect(sh, "item_select", SceneItemSelectSignal,
			       nullptr);
	signal_handler_connect(sh, "item_deselect", SceneItemDeselectSignal,
			       nullptr);
	OBSWeakSourceAutoRelease currentSceneWeak =
		obs_source_get_weak_source(currentSceneSource);
	currentScene = currentSceneWeak;

	properties->SetSource(nullptr);
}

bool obs_module_load(void)
{
	properties = new PropertiesDock(
		static_cast<QMainWindow *>(obs_frontend_get_main_window()));
	obs_frontend_add_dock(properties);
	obs_frontend_add_event_callback(FrontendEvent, nullptr);

	blog(LOG_INFO, "Properties dock loaded successfully. Version %s",
	     PLUGIN_VERSION);
	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(FrontendEvent, nullptr);
}
