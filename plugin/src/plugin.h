#ifndef OSSA_MUSTHAVE_TYPES
#include "./mhs.h"
#endif
#include "./ossaconsts.h"

#define ossa_init_args(ptr, ...) \
    struct { \
        void *env; \
        __VA_ARGS__; \
    } *params = (struct { \
        void *env; \
        __VA_ARGS__; \
    } *)(ptr)

#define PLUGIN_NAME(name) char *plugin_name = name
#define importFunc(rettype, name) rettype (*name)
#define exportFunc(name) int name(struct __list *args, ossaCID cid)
#define OSSA_LOG_FATAL    0x10 // Core can't continue work
#define OSSA_LOG_CRIT     0x20 // Core can continue, but target can't be finished
#define OSSA_LOG_BIG      0x30 // Taret will be finished, but some data can be currupted or lost
#define OSSA_LOG_WARNING  0x40 // Warning: unlikely actions
#define OSSA_LOG_INFO     0x50 // Just info, can ignore
#define OSSA_LOG_DEBUG    0x60 // Debug info

void *self;

/**
 * @brief init of plugin
 * 
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_init();

/**
 * @brief go online
 * 
 * @return 0 if processed successfully, -1 if not
 */
int plugin_connect();

/**
 * @brief go offline
 * 
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_disconnect();

/**
 * @brief get status of plugin
 * 
 * @return see ossaconsts.h
 */
int plugin_state();



/**
 * @brief authentication
 * 
 * @param username username
 * @param pwd password
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_user_auth(char* username, char *pwd);

/**
 * @brief authentication via token by oauth tech.
 * 
 * @param token 
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_user_oauth(char *token);

/**
 * @brief exit from account
 * 
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_user_exit();

/**
 * @brief change user's name
 * 
 * @param newName 
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_user_rename(char *newName);

/**
 * @brief get full info about user
 * 
 * @return ossaUser 
 */
ossaUser plugin_user_info();

/**
 * @brief get info about another user
 * 
 * @param globalUID global User ID
 * @return ossaUser 
 */
ossaUser plugin_user_ginfo(ossastr globalUID);


/**
 * @brief send message
 * 
 * @param cid Chat ID
 * @param mes Message
 * @return int 
 */
int plugin_message_send(ossaCID cid, ossaMessage mes);

/**
 * @brief edit message
 * 
 * @param cid Chat ID
 * @param mes edited message
 * @param mid Message ID
 * @return 0 if processed successfully, -1 if not 
 */
int plugin_message_editMes(ossaCID cid, ossaMessage mes, ossaMID mid);



/**
 * @brief Make new chat
 * 
 * @param title chat title
 * @return ossaCID 
 */
ossaCID plugin_chat_makeChat(ossastr title);

/**
 * @brief get prefences of chat (json)
 * 
 * @param cid Chat ID
 * @return json of chat prefences
 */
ossastr plugin_chat_getprefs(ossaCID cid);

/**
 * @brief set new prefences of chat (json)
 * 
 * @param cid Chat ID
 * @param fieldname json field name/location
 * @param value new value
 * @return 0 if processed successfully, -1 if not
 */
int plugin_chat_setpref(ossaCID cid, ossastr fieldname, ossastr value);

/**
 * @brief get info about messages in chat
 * 
 * @param cid Chat ID
 * @return count of unread messages
 */
int plugin_chat_update(ossaCID cid);

/**
 * @brief load chat from disk or web and try to update it
 * 
 * @param cid Chat ID
 * @param location location of file
 * @return 0 if processed successfully, -1 if not
 */
int plugin_chat_loadChat(ossaCID cid, ossastr location);

/**
 * @brief get list of chat for plugin separeted by '\\n' character
 * 
 * @return list of chat for plugin separeted by '\\n' character 
 */
ossastr plugin_chat_list();

/**
 * @brief get global User ID for each user in chat. First in this list is always yourself
 * 
 * @param cid Chat ID
 * @return list of global User's ID separated by '\n' charecter
 */
ossastr plugin_chat_getGUIDs(ossaCID cid);

//importing funcs
int   (*ossaChatClear)(ossaCID cid);
int   (*ossaChatAddMessage)(ossaCID cid, ossaMessage);
int   (*ossaChatRemoveMessage)(ossaCID cid, ossaMessage);
int   (*ossaChatAddUser)(ossaCID cid, ossaUser);
int   (*ossaChatRemoveUser)(ossaCID cid, ossaUser);
int   (*ossaClientSetRoutineDirect)(void *pluginptr,void*(*routine)(void *args), void* args);
int   (*ossaClientNotifyDirect)(void *pluginptr, ossaCID cid, ossaMessage message);
int   (*ossaClientEventDirect)(void *pluginptr, ossastr event, int evId);
void* (*ossaClientImportDirect)(void *pluginptr, char *symname);
int   (*ossaClientExportDirect)(void *pluginptr, char *name, void *ptr);
int   (*ossaClientRequireLibDirect)(void *pluginptr, ossastr libname);
void  (*ossaLog)(char type, const char *format, ...);

/**
 * @brief Set function to working on background in another thread (supported by OSSA core)
 * 
 * @param routine function for thread
 * @param args arguments for function
 * 
 * @return (int) ID of thread
 */
#define ossaClientSetRoutine(routine, args) ossaClientSetRoutineDirect(self, routine, args)

/**
 * @brief Notify about new message
 * 
 * @param cid Chat ID where is message comes
 * @param message message to write into the chat
 * @return (int) 0 if success or -1 if not
 */
#define ossaClientNotify(cid, message) ossaClientNotifyDirect(self, cid, message)

/**
 * @brief create event for OSSA core
 * 
 * @param event json of event description. Special for every plugin
 * @param eid event ID, plugin for sending event. first plugins - Log Daemon, Status Control, File System, Userspace, Network Control, Enviroment (OSSA), External System, OSSA Interpreter
 * see doc.md5:events for more
 * @return (int) 0 if success or -1 if not
 */
#define ossaClientEvent(event, eid) ossaClientEventDirect(self, event, eid)

/**
 * @brief Import symbol from OSSA enviroment
 * 
 * @param symbol name of symbol
 * @return pointer to symbol
 */
#define ossaClientImport(symbol) ossaClientImportDirect(self, symbol)

/**
 * @brief Export symbol to OSSA enviroment
 * 
 * @param name name of symbol for export
 * @param ptr pointer of symbol
 * @return (int) 0 if success or -1 if not
 */
#define ossaClientExport(name, ptr) ossaClientExportDirect(self, name, ptr)

/**
 * @brief Import set of symbols from OSSA enviroment
 *  Before using you should to declare needed symbols in your code as pointer. Like 'type *var;' or 'type (*function)(args);'
 * @param libname name of sybols set or library for loading
 * @return count of unloaded symbols from library or symbols set
 */
#define ossaClientRequireLib(libname) ossaClientRequireLibDirect(self, libname)