# What is Plugin
Hey! You in plugin root directory!

Here is one plugin - fLogger. Plugin for testing core code and example plugin. You can use it as template

Logger write EVERYTHING to files. use auth call for choose username (`username`) and log file (`password`)

Also Every plugin must have some main fields. Next - is it

# Usage

Here is MD info for user. It's parsing by ossa frontend

Usually here is short guid for user

# Functions

## Standart

Ok, it's important part of this MarkDown file. Here you should describe all unusial info about your plugin

## Plugin

Ok, it's MOST important part of this MarkDown file. Here you shold describe using of ALL user-space functons.
User-space functions is functions, which can be called from user as usual CLI application inside OSSA client

Example - call chatClear
```
    user@chat> :chatClear -quet #this is same syntax like in OSSA command, but if it isn't OSSA command it try to find it in plugin commandlist
    user@chat> :plugin.chatClear -quet #this is also plugin call
```

If user call some help page it's will display under-flag from this fragment

for example: user call man for `chatClear` and OSSA frontend will display this README on `Functions.Plugin.chatClear` mark. Do not forget write README header same like command name

### chatClear

Clearing all chat file and other info

### chatWhere

Print file location

# Depends

Describe deps of plugin here. You also can generate part of manifest deps from this file. Parser tries to find table in `Depends` header. Table should with `dep-name` (string) and `dep-type` ('runtime','installed','shared' or 'located') collums titles. User tipial MarkDown table

`dep-name` - is name of depends. Here is a few preficses:
- **sys** - *system* package dep
- **ossa** - *ossa* package dep
- `nothing` - only for files
- **call** - dep for `OpenOSSACallEnv`. It's usual plugin custom call like in `Functions.Plugin`

After all prefixes you should to insert `/` and write dependency name

Below example of table. *plugman* can detect it and add all deps to `manifest.json`

|  dep-name   |  dep-type  |
| ----------- | ---------- |
| sys/zip     | runtime    |
| ossa/fssa   | installed  |
| /usr/bin/cd | located    |
| call/ossa.l | runtime    |

For push it to manifest use:

```shell
    user@host ~/ossa/plugin $ ./plugman manifest -import deps -i ./README.md -f md
```

Also you can paste deps here

```shell
    user@host ~/ossa/plugin $ ./plugman manifest -export deps -o ./README.md -f md
```