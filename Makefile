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
deps-elf:
	$(info [CC] Building (DEPS) ./core/elfloader/elf_loader.c -> ./lib/libelfloader.so)
	@$(CC) ./core/elfloader/wheelc/list.c ./core/elfloader/elf_loader.c -fPIC -g3 -shared -o ./lib/libelfloader.so
deps-json:
	$(info [CC] Building (DEPS) ./core/parson/parson.c -> ./lib/parson.o)
	@$(CC) -c ./core/parson/parson.c -o ./lib/parson.o -g3 -fPIC
core-core.c:
	$(info [CC] Building (CORE) ./core/core.c -> ./lib/core.o)
	$(CC) ./core/core.c $(CFLAGS) $(BUILDMODE) -c -fpic -ldl -o ./lib/core.o -lpthread -lwindows -lzip
core-shared:
	$(info [CC] Building (CORE) ./lib/core.o -> ./lib/libossa.so)
	$(CC) -shared -o ./lib/libossa.so ./lib/core.o ./lib/list.o ./lib/parson.o  -ldl -lpthread
core-clean-shared:
	$(info [SH] Cleaning (LIB ) .o files)
	@rm -f ./lib/*.o
corelib: deps-elf deps-json deps-dlist core-core.c core-shared
core-dlist.c:
	$(info [CC] Building (CORE) ./core/dlist/list.c)
	@$(CC) ./core/dlist/list.c $(CFLAGS) -c -o ./lib/dlist.a
core: corelib core-clean-shared
cssa.c:
	$(info [CC] Building (CLI ) ./cli/cssa.c -> ./bin/cssa)
	@$(CC) ./cli/cssa.c $(CFLAGS) ./lib/parson.o ./lib/core.o ./lib/list.o -o ./bin/cssa -ldl -lzip -lpthread -L/usr/lib64 -L./core/opjp -lossajsonparser -lelfloader
cssa: $(BUILDMODE = -DCOMPILE_DYNAMIC) corelib cssa.c
cli: cssa
gui: core gssa-main.cpp
plugin-compile:
	$(info [CC] Building (PLUG) ./plugin/src/plugin.c -> ./plugin/plugin.o)
	@$(CC) ./plugin/src/plugin.c -fpic -c -o ./plugin/plugin.o -g3
plugin-shared:
	$(info [CC] Building (PLUG) ./plugin/src/plugin.o -> ./plugin/plugin.so)
	@$(CC) ./plugin/plugin.o ./lib/list.o -shared -o ./plugin/plugin.so -g3 -ldl 
plugin-clear:
	$(info [SH] Cleaning (PLUG) ./plugin/plugin.o)
	@rm -f ./plugin/plugin.o
plugin: deps-dlist plugin-compile plugin-shared plugin-clear
all: corelib cli gui utils-plugman

#Cleaning
binclean:
	$(info [SH] Cleaning binaries)
	@find ./bin/ ! -name 'README.md' -type f -exec rm -f {} +