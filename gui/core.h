#ifndef CORE_H
#define CORE_H
#include "plugin.h"
#include "dummyplugin.h"
#include "datamodels.h"

class Core {
public:
    QList<Plugin*> getPluginList();
};
#endif // CORE_H
