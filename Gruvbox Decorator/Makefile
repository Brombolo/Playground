NAME = Gruvbox
TYPE = SHARED
SRCS = GruvboxControlLook.cpp GruvboxDecorator.cpp
LIBS = be
OPTIMIZE = FULL

SYSTEM_INCLUDE_PATHS = /boot/system/develop/headers/private/interface \
                       /boot/system/develop/headers/private/shared \
                       /boot/system/develop/headers/be

# Percorsi di installazione
CL_DIR = /boot/home/config/non-packaged/add-ons/control_look
DEC_DIR = /boot/home/config/non-packaged/add-ons/decorators

include /boot/system/develop/etc/makefile_engine

install: all
	mkdir -p $(CL_DIR) $(DEC_DIR)
	cp obj/Gruvbox $(CL_DIR)/Gruvbox
	cp obj/Gruvbox $(DEC_DIR)/Gruvbox
