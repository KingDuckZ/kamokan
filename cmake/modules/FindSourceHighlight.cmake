# Copyright 2017 by Michele Santullo
#
# Try to find sourge-highlight
# Usage of this module as follows:
#
#     find_package(SourceHighlight)
#
# The following variables will be defined:
#
#  SourceHighlight_FOUND - system has source-highlight
#  SourceHighlight_INCLUDE_DIR - the libsource-highlight include directory
#  SourceHighlight_LIBRARIES - The libraries needed to use libsource-highlight

find_path(SourceHighlight_ROOT_DIR
    NAMES include/srchilite/sourcehighlight.h
)

find_path(SourceHighlight_INCLUDE_DIR
	NAMES srchilite/sourcehighlight.h
	HINTS ${SourceHighlight_ROOT_DIR}/include
)

find_library(SourceHighlight_LIBRARIES
	NAMES source-highlight
	HINTS ${SourceHighlight_ROOT_DIR}/lib
)

if(SourceHighlight_INCLUDE_DIR AND SourceHighlight_LIBRARIES AND Ncurses_LIBRARY)
	set(SourceHighlight_FOUND ON)
else(SourceHighlight_INCLUDE_DIR AND SourceHighlight_LIBRARIES AND Ncurses_LIBRARY)
	find_library(SourceHighlight_LIBRARIES NAMES source-highlight)
	include(FindPackageHandleStandardArgs)
	find_package_handle_standard_args(SourceHighlight DEFAULT_MSG SourceHighlight_INCLUDE_DIR SourceHighlight_LIBRARIES )
	mark_as_advanced(SourceHighlight_INCLUDE_DIR SourceHighlight_LIBRARIES)
endif(SourceHighlight_INCLUDE_DIR AND SourceHighlight_LIBRARIES AND Ncurses_LIBRARY)

if (SourceHighlight_FOUND AND NOT TARGET SourceHighlight::SourceHighlight)
	add_library(SourceHighlight::SourceHighlight INTERFACE IMPORTED)
	set_property(TARGET SourceHighlight::SourceHighlight
		PROPERTY INTERFACE_LINK_LIBRARIES "${SourceHighlight_LIBRARIES}"
	)
	set_property(TARGET SourceHighlight::SourceHighlight
		PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SourceHighlight_INCLUDE_DIR}"
	)
endif()

mark_as_advanced(
	SourceHighlight_INCLUDE_DIR
    SourceHighlight_LIBRARIES
)
