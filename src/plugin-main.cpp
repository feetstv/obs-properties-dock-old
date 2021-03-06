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
#include "transform-dock.hpp"
#include "plugin-macros.generated.h"

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

PropertiesDock *properties;
TransformDock *transform;

OBSSignal selectSignal;
OBSSignal deselectSignal;
int selectedItemsCount;

void SceneItemSelectSignal(void *, calldata_t *data)
{
	selectedItemsCount++;

    OBSSceneItem item = (obs_sceneitem_t *)calldata_ptr(data, "item");
	OBSSource source = obs_sceneitem_get_source(item);

	properties->SetSource(source);
	transform->SetSceneItem(item);
}

void SceneItemDeselectSignal(void *, calldata_t *)
{
	selectedItemsCount--;
	if (selectedItemsCount == 0) {
		properties->SetSource(nullptr);
		transform->SetSceneItem(nullptr);
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

    selectSignal.Disconnect();
    deselectSignal.Disconnect();

	/* Connect signal to new source */
	signal_handler_t *sh =
		obs_source_get_signal_handler(currentSceneSource);
    selectSignal.Connect(sh, "item_select", SceneItemSelectSignal, nullptr);
    deselectSignal.Connect(sh, "item_deselect", SceneItemDeselectSignal, nullptr);

	struct cb_data {
		int *selectedItemsCount;
		OBSSceneItem selectedItem;
	} data;
	data.selectedItemsCount = &selectedItemsCount;
	data.selectedItem = nullptr;

	auto cb = [](obs_scene_t *, obs_sceneitem_t *item, void *data) {
		struct cb_data *cb_data = static_cast<struct cb_data *>(data);
		if (obs_sceneitem_selected(item)) {
			cb_data->selectedItemsCount++;
			/* Always override to get the top-most item */
			cb_data->selectedItem = item;
		}
		return true;
	};

	obs_scene_enum_items(obs_scene_from_source(currentSceneSource), cb,
			     &data);

	OBSSceneItem item = data.selectedItem;
	OBSSource source = obs_sceneitem_get_source(item);
	properties->SetSource(source);
	transform->SetSceneItem(item);
}

bool obs_module_load(void)
{
	blog(LOG_INFO, "Version %s", PLUGIN_VERSION);
	blog(LOG_INFO, "Compile time: OBS %s, QT %s", OBS_VERSION,
	     QT_VERSION_STR);
	blog(LOG_INFO, "Run time: OBS %s, QT %s", obs_get_version_string(),
	     qVersion());

	QMainWindow *main =
		static_cast<QMainWindow *>(obs_frontend_get_main_window());
	properties = new PropertiesDock(main);
	transform = new TransformDock(main);
    
    obs_frontend_add_dock(properties);
	obs_frontend_add_dock(transform);

	obs_frontend_add_event_callback(FrontendEvent, nullptr);
	return true;
}

void obs_module_unload()
{
	obs_frontend_remove_event_callback(FrontendEvent, nullptr);
    selectSignal.Disconnect();
    deselectSignal.Disconnect();
}
