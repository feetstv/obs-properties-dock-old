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

#include <QDockWidget>
#include <QComboBox>
#include <vector>

#include <obs.hpp>
#include <obs-frontend-api.h>

class PropertiesDock : public QDockWidget {
	Q_OBJECT

public:
	PropertiesDock(QWidget *parent = nullptr);
	~PropertiesDock();

	void RefreshSources();
	void RefreshPropertiesView();
	void Refresh();

private:
	QWidget *widget = nullptr;
	QComboBox *combobox = nullptr;
	OBSPropertiesView *propertiesView = nullptr;
};
