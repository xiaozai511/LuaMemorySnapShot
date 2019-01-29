#ifndef COMMON_H
#define COMMON_H

#include <QDebug>
#include <QSettings>
#include <QString>
#include <QVariant>

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)			if(p) { delete p; p = 0; }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)    if(p) { delete[] p; p = 0; }
#endif

#ifndef SAFE_FREE
#define SAFE_FREE(p)			if(p) { free(p); p = 0; }
#endif

#ifndef MAX
#define MAX(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(c,d)            (((c) < (d)) ? (c) : (d))
#endif

#define INIT_PATH_STR_KEY "initpath"
QSettings* globalSetting();

#endif // COMMON_H
