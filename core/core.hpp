#define OSSA_CPP
#define OSSA_STDLIST
#include "./core.h"

namespace  ossa{

class OssaChat;

class OssaPlugin{
    protected:
        struct ossaPlugin plugin;
        ossalist(OssaChat) сhats;
    public:
        void start();
        void stop();

        void login(ossastr token);
        void login(ossastr login, ossastr pwd);
        void unauth();

        ossalist(OssaChat) getChats();
        OssaPlugin(ossastr path);
};

class OssaChat{
    protected:
        struct ossaChat chat;
    public:
        ossalist(ossaMessage) getMessagelist();
        ossaMessage getMessage(ossaMID mid);
        ossaUser getUser(ossaUID uid);

        ossalist(ossaUser) getUserlist();
        int inviteUser(ossastr globalUID);
        int removeUser(ossaUID uid);

        ossalist(ossastr) getProperties();
        int setProperty(ossastr fieldname, ossastr value);
        int send(ossaMessage message);
        int edit(ossaMID target, ossaMessage edited);
        int act(ossastr actname);
        bool check();

        int drop(ossastr path);

        OssaChat(Plugin *plugin);
        ~OssaChat();
};

}