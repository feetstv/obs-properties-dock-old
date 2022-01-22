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

#pragma once

#include "obs-classes/properties-view.hpp"
#include "plugin-macros.generated.h"
#include "obs-classes-helper-functions.hpp"

#include <obs.hpp>
#include <obs-module.h>
#include <obs-frontend-api.h>

#include <QDockWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>

class PropertiesDock : public QDockWidget {
	Q_OBJECT

public:
	PropertiesDock(QWidget *parent = nullptr);
	~PropertiesDock();

private:
	QWidget *widget = nullptr;
	OBSPropertiesView *propertiesView = nullptr;

	OBSWeakSource currentScene;
	static void FrontendEvent(enum obs_frontend_event event, void *data);
	static void SceneItemSelectSignal(void *param, calldata_t *data);
	static void SceneItemDeselectSignal(void *param, calldata_t *data);
	int selectedItemsCount;
	void Refresh();
};
