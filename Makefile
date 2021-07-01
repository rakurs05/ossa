gssa-main.cpp:
	@echo [CC] Building ./gui/main.cpp
	@$(CXX) ./gui/main.cpp $(CXXFLAGS) -o ./bin/gssa
ossa_gui: gssa-main.cpp
core-core.c:
	@echo [CC] Building ./core/core.c
	@$(CC) ./core/core.c $(CFLAGS) -c -o ./bin/core.a
core-dlist.c:
	@echo [CC] Building ./core/dlist/list.c
	@$(CC) ./core/dlist/list.c $(CFLAGS) -c -o ./bin/dlist.a
core: core-dlist.c core-core.c

#Cleaning
binclean:
	@echo [SH] Cleaning binaries
	@find ./bin/ ! -name 'README.md' -type f -exec rm -f {} +