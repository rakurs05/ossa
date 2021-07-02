gssa-main.cpp:
	$(info [CC] Building ($@) ./gui/main.cpp ($@))
	@$(CXX) ./gui/main.cpp $(CXXFLAGS) -o ./bin/gssa
ossa_gui: gssa-main.cpp
deps-dlist:
	$(info [CC] Building ($@) ./core/dlist/list.c -> ./lib/list.o ($@))
	@$(CC) -c ./core/dlist/list.c $(CFLAGS) -fpic -o ./lib/list.o
core-core.c:
	$(info [CC] Building ($@) ./core/core.c -> ./lib/core.o ($@))
	@$(CC) ./core/core.c $(CFLAGS) -fpic -lzip -ldl -o ./lib/core.o ./lib/list.o
core-shared:
	$(info [CC] Building ($@) ./lib/core.o -> ./lib/libossa.so ($@))
	@$(CC) -shared -o ./lib/libossa.so ./lib/core.o
core-clean-shared:
	$(info [SH] Cleaning ($@) .o files ($@))
	@rm -f ./lib/*.o
corelib: deps-dlist core-core.c core-shared core-clean-shared
core-dlist.c:
	$(info [CC] Building ./core/dlist/list.c ($@))
	@$(CC) ./core/dlist/list.c $(CFLAGS) -c -o ./lib/dlist.a
core: corelib

#Cleaning
binclean:
	$(info [SH] Cleaning binaries)
	@find ./bin/ ! -name 'README.md' -type f -exec rm -f {} +