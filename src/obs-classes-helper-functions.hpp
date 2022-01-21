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

#include <obs-module.h>
#include <obs-frontend-api.h>
#include <QMainWindow>
#include <QMetaObject>
#include <QApplication>

/* Modern problems require modern solutions */
class YupThisReallyIsAThing {
public:
	QMainWindow *GetMainWindow()
	{
		return static_cast<QMainWindow *>(
			obs_frontend_get_main_window());
	}

	const char *MainText(const char *val)
	{
		const char *out;
		if (QMetaObject::invokeMethod(qApp, "GetString",
					      Q_RETURN_ARG(const char *, out),
					      Q_ARG(const char *, val))) {
			return out;
		} else {
			return val;
		}
	}
};

/* I really hope these get destroyed immediately */
#define App() (new YupThisReallyIsAThing())

//#define Str(lookupVal) App()->MainText(lookupVal)
#define Str(lookupVal) obs_module_text(lookupVal)
#define QTStr(lookupVal) QString::fromUtf8(Str(lookupVal))
