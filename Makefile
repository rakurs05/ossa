.PHONY: corelib ore cssa cli plugin all
GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
BUILDMODE = "-DCOMPILE_STATIC"
CFLAGS = "-g3"
CXX = $(CC) + " -std=gnu++17"
utils-plugman:
	$(info [GO] Building (UTILS) ./utilities/plugman.go -> ./bin/plugman)
	@go build ./utilities/plugman.go
	@mv ./plugman ./bin/plugman
info:
	$(info $(CXX))
	$(info "$(GCCVERSION)")
gssa-main.cpp:
	$(info [CC] Building (GUI ) ./gui/main.cpp)
	@$(CXX) ./gui/main.cpp $(CXXFLAGS) -o ./bin/gssa
ossa_gui: gssa-main.cpp
deps-dlist:
	$(info [CC] Building (DEPS) ./core/dlist/list.c -> ./lib/list.o)
	@$(CC) -c ./core/dlist/list.c $(CFLAGS) -fpic -o ./lib/list.o
core-core.c:
	$(info [CC] Building (CORE) ./core/core.c -> ./lib/core.o)
	@$(CC) ./core/core.c $(CFLAGS) $(BUILDMODE) -c -fpic -lzip -ldl -o ./lib/core.o
core-shared:
	$(info [CC] Building (CORE) ./lib/core.o -> ./lib/libossa.so)
	@$(CC) -shared -o ./lib/libossa.so ./lib/core.o ./lib/list.o -lzip -ldl
core-clean-shared:
	$(info [SH] Cleaning (LIB ) .o files)
	@rm -f ./lib/*.o
corelib: deps-dlist core-core.c core-shared 
core-dlist.c:
	$(info [CC] Building (CORE) ./core/dlist/list.c)
	@$(CC) ./core/dlist/list.c $(CFLAGS) -c -o ./lib/dlist.a
core: corelib core-clean-shared
cssa.c:
	$(info [CC] Building (CLI ) ./cli/cssa.c -> ./bin/cssa)
	@$(CC) ./cli/cssa.c $(CFLAGS) ./lib/core.o ./lib/list.o -o ./bin/cssa -lzip -ldl
cssa: $(BUILDMODE = -DCOMPILE_DYNAMIC) corelib cssa.c
cli: cssa
gui: core gssa-main.cpp
plugin-compile:
	$(info [CC] Building (PLUG) ./plugin/src/plugin.c -> ./plugin/plugin.o)
	@$(CC) ./plugin/src/plugin.c -fpic -c -o ./plugin/plugin.o
plugin-shared:
	$(info [CC] Building (PLUG) ./plugin/src/plugin.o -> ./plugin/plugin.so)
	@$(CC) ./plugin/plugin.o ./lib/list.o -shared -o ./plugin/plugin.so
plugin-clear:
	$(info [SH] Cleaning (PLUG) ./plugin/plugin.o)
	@rm -f ./plugin/plugin.o
plugin: deps-dlist plugin-compile plugin-shared plugin-clear
all: corelib cli gui utils-plugman

#Cleaning
binclean:
	$(info [SH] Cleaning binaries)
	@find ./bin/ ! -name 'README.md' -type f -exec rm -f {} +