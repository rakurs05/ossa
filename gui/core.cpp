#include "core.h"

QList<Plugin*> Core::getPluginList() {
    QList<Plugin*> list;
    list.append(new DummyPlugin());
    list.append(new DummyPlugin());
    return list;
}
