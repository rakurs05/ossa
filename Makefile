GCCVERSION = $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
utils-plugman:
	$(info [GO] Building (UTILS) ./utilities/plugman.go -> ./bin/plugman)
	@go build ./utilities/plugman.go
	@mv ./plugman ./bin/plugman
info:
	$(info $(CC))
	$(info "$(GCCVERSION)")
gssa-main.cpp:
	$(info [CC] Building (GUI) ./gui/main.cpp)
	@$(CXX) ./gui/main.cpp $(CXXFLAGS) -o ./bin/gssa
ossa_gui: gssa-main.cpp
deps-dlist:
	$(info [CC] Building (DEPS) ./core/dlist/list.c -> ./lib/list.o)
	@$(CC) -c ./core/dlist/list.c $(CFLAGS) -fpic -o ./lib/list.o
core-core.c:
	$(info [CC] Building (CORE) ./core/core.c -> ./lib/core.o)
	@$(CC) ./core/core.c $(CFLAGS) -fpic -lzip -ldl -o ./lib/core.o ./lib/list.o
core-shared:
	$(info [CC] Building (CORE) ./lib/core.o -> ./lib/libossa.so)
	@$(CC) -shared -o ./lib/libossa.so ./lib/core.o
core-clean-shared:
	$(info [SH] Cleaning (LIB) .o files)
	@rm -f ./lib/*.o
corelib: deps-dlist core-core.c core-shared core-clean-shared
core-dlist.c:
	$(info [CC] Building (CORE) ./core/dlist/list.c)
	@$(CC) ./core/dlist/list.c $(CFLAGS) -c -o ./lib/dlist.a
core: info corelib

#Cleaning
binclean:
	$(info [SH] Cleaning binaries)
	@find ./bin/ ! -name 'README.md' -type f -exec rm -f {} +