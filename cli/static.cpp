#include "../core/core.h"

struct ossaPlugin plugin;

int main(){
    loadChatPlugin(&plugin, "./plugin.so");
}