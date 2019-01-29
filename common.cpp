#include "common.h"

QSettings* globalSetting()
{
	static QSettings* s_setting = nullptr;
	if(!s_setting) {
		s_setting = new QSettings(QString("editor.ini"), QSettings::IniFormat);
	}

	return s_setting;
}
