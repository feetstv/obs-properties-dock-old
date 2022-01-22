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

#include "properties-dock.hpp"

/* Stolen straight from obs-websocket */
template<typename T>
T *calldata_get_pointer(const calldata_t *data, const char *name)
{
	void *ptr = nullptr;
	calldata_get_ptr(data, name, &ptr);
	return reinterpret_cast<T *>(ptr);
}

void PropertiesDock::SceneItemSelectSignal(void *param, calldata_t *data)
{
	PropertiesDock *dock = static_cast<PropertiesDock *>(param);

	dock->selectedItemsCount++;

	OBSSource itemSource = obs_sceneitem_get_source(
		calldata_get_pointer<obs_sceneitem_t>(data, "item"));

	OBSDataAutoRelease settings = obs_source_get_settings(itemSource);

	auto updateCb = [](void *obj, obs_data_t *old_settings,
			   obs_data_t *new_settings) {
		UNUSED_PARAMETER(old_settings); // TODO: Undo/Redo
		OBSSource source = static_cast<obs_source_t *>(obj);
		obs_source_update(source, new_settings);
	};

	auto updateCbWithoutUndo = [](void *obj, obs_data_t *settings) {
		OBSSource source = static_cast<obs_source_t *>(obj);
		obs_source_update(source, settings);
	};

	if (dock->propertiesView)
		dock->propertiesView = nullptr;

	dock->propertiesView = new OBSPropertiesView(
		settings.Get(), itemSource,
		(PropertiesReloadCallback)obs_source_properties,
		(PropertiesUpdateCallback)updateCb,
		(PropertiesVisualUpdateCb)updateCbWithoutUndo);

	dock->Refresh();
}

void PropertiesDock::SceneItemDeselectSignal(void *param, calldata_t *)
{
	PropertiesDock *dock = static_cast<PropertiesDock *>(param);

	dock->selectedItemsCount--;
	if (dock->propertiesView && dock->selectedItemsCount == 0)
		dock->propertiesView = nullptr;

	dock->Refresh();
}

void PropertiesDock::FrontendEvent(enum obs_frontend_event event, void *data)
{
	if (event != OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED)
		return;

	PropertiesDock *dock = static_cast<PropertiesDock *>(data);

	OBSSourceAutoRelease currentScene =
		obs_frontend_preview_program_mode_active()
			? obs_frontend_get_current_preview_scene()
			: obs_frontend_get_current_scene();

	/* Remove signal from old source */
	if (dock->currentScene &&
	    !obs_weak_source_expired(dock->currentScene)) {
		OBSSourceAutoRelease oldScene =
			obs_weak_source_get_source(dock->currentScene);
		signal_handler_t *sh = obs_source_get_signal_handler(oldScene);
		signal_handler_disconnect(sh, "item_select",
					  SceneItemSelectSignal, data);
		signal_handler_disconnect(sh, "item_deselect",
					  SceneItemDeselectSignal, data);
	}

	/* Add signal to new source */
	signal_handler_t *sh = obs_source_get_signal_handler(currentScene);
	signal_handler_connect(sh, "item_select", SceneItemSelectSignal, data);
	signal_handler_connect(sh, "item_deselect", SceneItemDeselectSignal,
			       data);
	OBSWeakSourceAutoRelease currentSceneWeak =
		obs_source_get_weak_source(currentScene);
	dock->currentScene = currentSceneWeak;

	if (dock->propertiesView) {
		dock->propertiesView->deleteLater();
		dock->propertiesView = nullptr;
	}

	dock->selectedItemsCount = 0;
	dock->Refresh();
}

void PropertiesDock::Refresh()
{
	if (widget)
		widget->deleteLater();

	widget = new QWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	widget->setLayout(layout);

	setWidget(widget);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (propertiesView) {
		layout->addWidget(propertiesView);
	} else {
		QLabel *label = new QLabel(widget);
		label->setText(obs_module_text("NoSelection"));
		layout->addWidget(label);
	}
}

PropertiesDock::PropertiesDock(QWidget *parent) : QDockWidget(parent)
{
	setFeatures(DockWidgetMovable | DockWidgetFloatable |
		    DockWidgetClosable);
	setWindowTitle(obs_module_text("Dock.Title"));
	setObjectName("PropertiesDock");
	setFloating(false);

	obs_frontend_add_event_callback(FrontendEvent, this);
}

PropertiesDock::~PropertiesDock()
{
	obs_frontend_remove_event_callback(FrontendEvent, this);
}
