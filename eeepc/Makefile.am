noinst_PROGRAMS = eeepc-install-menu
eeepc_install_menu_SOURCES = eeepc-install-menu.cpp

noinst_SCRIPTS = run-installer

AM_CXXFLAGS = -Wall `pkg-config glib-2.0 --cflags`

LIBS = -Wall `pkg-config glib-2.0 --libs`

EXTRA_DIST = *
CLEANFILES = *~ *.tar.gz

install-exec-hook:
	./run-installer
