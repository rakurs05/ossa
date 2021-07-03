#include <stdio.h>
#include "../core/core.h"

struct ossaPlugin plugin;
struct ossaChat chat;

int main(int argc, char **argv){

    fprintf(stderr, "unloaded: %i\n", loadChatPlugin(&plugin, argv[1]));
    plugin.pcall.connect();
    plugin.pcall.auth("alexthunder", ".");
    chat = makeChat("chat.ffgelogger", &plugin);
    sendMessage(&chat, makeMessage(&chat, "Hello!", lnothing));
    sendMessage(&chat, makeMessage(&chat, "Hello!", lnothing));

    while(1){
        
    }
}