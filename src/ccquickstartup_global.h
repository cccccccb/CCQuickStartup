#ifndef CCQUICKSTARTUP_GLOBAL_H
#define CCQUICKSTARTUP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(CC_QUICKSTARTUP_LIBRARY)
#define CC_QUICKSTARTUP_EXPORT Q_DECL_EXPORT
#else
#define CC_QUICKSTARTUP_EXPORT Q_DECL_IMPORT
#endif

#endif // CCQUICKSTARTUP_GLOBAL_H
