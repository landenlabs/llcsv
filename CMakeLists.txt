# Generated on 2020-4-18 21:00:10 by version 0.9.0 of mulle-xcode-to-cmake
# Command line:
#    mulle-xcode-to-cmake -b export llcsv.xcodeproj/


cmake_minimum_required (VERSION 3.4)
project( llcsv CXX)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

##
## llcsv Files
##

# uncomment this for mulle-objc to search libraries first
# set( CMAKE_FIND_FRAMEWORK "LAST")

set( SOURCES
llcsv/csvcmds/csvappendcolumns.cpp
llcsv/csvcmds/csvappendrows.cpp
llcsv/csvcmds/csvcmds.cpp
llcsv/csvcmds/csvinputfiles.cpp
llcsv/csvcmds/csvjoincolumns.cpp
llcsv/csvcmds/csvmatchcolumns.cpp
llcsv/csvcmds/csvoutputfile.cpp
llcsv/csvcmds/csvreplacecolumns.cpp
llcsv/csvcmds/csvselectcolumns.cpp
llcsv/csvcmds/csvselectrows.cpp
llcsv/csvcmds/csvsortrowsbycolumnkeys.cpp
llcsv/llcsv.cpp
llcsv/util/directory.cpp
)

set( STATIC_DEPENDENCIES
)

if( NOT SKIP_FIND_FOUNDATION_LIBRARY)
   find_library( FOUNDATION_LIBRARY Foundation)
   message( STATUS "FOUNDATION_LIBRARY is ${FOUNDATION_LIBRARY}")
endif()

set( DEPENDENCIES
${FOUNDATION_LIBRARY}
)


##
## llcsv
##

add_executable( llcsv
${SOURCES}
${PUBLIC_HEADERS}
${PROJECT_HEADERS}
${PRIVATE_HEADERS}
${RESOURCES}
)

target_link_libraries( llcsv
${STATIC_DEPENDENCIES}
${DEPENDENCIES}
)
