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

void PropertiesDock::SetSource(OBSSource source)
{
	if (widget)
		widget->deleteLater();

	widget = new QWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	widget->setLayout(layout);

	setWidget(widget);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (source) {
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
	setWindowTitle(obs_module_text("PropertiesDock.Title"));
	setObjectName("PropertiesDock");
	setFloating(false);
}
