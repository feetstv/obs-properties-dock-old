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

#include "transform-dock.hpp"

void TransformDock::SetSceneItem(OBSSceneItem item)
{
	if (widget)
		widget->deleteLater();

	widget = new QWidget();

	QVBoxLayout *layout = new QVBoxLayout();
	widget->setLayout(layout);

	setWidget(widget);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	if (item) {
		if (transformView)
			transformView = nullptr;

		transformView = new OBSBasicTransform(item);

		layout->addWidget(transformView);
	} else {
		QLabel *label = new QLabel(widget);
		label->setText(obs_module_text("NoSelection"));
		layout->addWidget(label);
	}
}

TransformDock::TransformDock(QWidget *parent) : QDockWidget(parent)
{
	setFeatures(DockWidgetMovable | DockWidgetFloatable |
		    DockWidgetClosable);
	setWindowTitle(obs_module_text("TransformDock.Title"));
	setObjectName("TransformDock");
	setFloating(false);
}
