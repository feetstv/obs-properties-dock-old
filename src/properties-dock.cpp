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

#include <obs-module.h>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include "obs-classes-helper-functions.hpp"

static void frontendEvent(enum obs_frontend_event event, void *data)
{
	if (event != OBS_FRONTEND_EVENT_PREVIEW_SCENE_CHANGED)
		return;

	PropertiesDock *dock = static_cast<PropertiesDock *>(data);
	dock->RefreshSources();
	dock->RefreshPropertiesView();
	dock->Refresh();
}

void PropertiesDock::RefreshSources()
{
	std::vector<OBSSceneItem> items;
	/* Generate sources list */
	{
		OBSSourceAutoRelease scene =
			obs_frontend_preview_program_mode_active()
				? obs_frontend_get_current_preview_scene()
				: obs_frontend_get_current_scene();
		auto cb = [](obs_scene_t *, obs_sceneitem_t *item, void *data) {
			std::vector<OBSSceneItem> *items =
				static_cast<std::vector<OBSSceneItem> *>(data);
			items->push_back(item);
			return true;
		};
		obs_scene_enum_items(obs_scene_from_source(scene), cb, &items);
		std::reverse(items.begin(), items.end());
	}
	/* Generate combobox */
	{
		if (combobox)
			combobox = nullptr;

		combobox = new QComboBox();
		combobox->setEditable(false);
		for (OBSSceneItem item : items) {
			QVariant v;
			v.setValue((void *)item);
			combobox->addItem(
				obs_source_get_name(
					obs_sceneitem_get_source(item)),
				v);
		}
		connect(combobox,
			QOverload<int>::of(&QComboBox::currentIndexChanged),
			[&](int) {
				RefreshPropertiesView();
				Refresh();
			});
	}
}

void PropertiesDock::RefreshPropertiesView()
{

	OBSSceneItem item =
		(obs_sceneitem_t *)combobox->currentData().value<void *>();
	OBSSource source = obs_sceneitem_get_source(item);

	OBSDataAutoRelease settings = obs_source_get_settings(source);

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

	if (propertiesView)
		propertiesView = nullptr;

	propertiesView = new OBSPropertiesView(
		settings.Get(), source,
		(PropertiesReloadCallback)obs_source_properties,
		(PropertiesUpdateCallback)updateCb,
		(PropertiesVisualUpdateCb)updateCbWithoutUndo);
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

	layout->addWidget(combobox);
	layout->addWidget(propertiesView);
}

PropertiesDock::PropertiesDock(QWidget *parent) : QDockWidget(parent)
{
	setFeatures(DockWidgetMovable | DockWidgetFloatable |
		    DockWidgetClosable);
	setWindowTitle(obs_module_text("Dock.Title"));
	setObjectName("PropertiesDock");
	setFloating(false);

	obs_frontend_add_event_callback(frontendEvent, this);
}

PropertiesDock::~PropertiesDock()
{
	obs_frontend_remove_event_callback(frontendEvent, this);
}
