# CMake generated Testfile for 
# Source directory: /source/cmake-2.8.10.2/Utilities
# Build directory: /source/cmake-2.8.10.2/Utilities
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(CMake.HTML "/usr/bin/xmllint" "--valid" "--noout" "--nonet" "--path" "/source/cmake-2.8.10.2/Utilities/xml/xhtml1" "/source/cmake-2.8.10.2/Docs/cmake.html" "/source/cmake-2.8.10.2/Docs/cmake-policies.html" "/source/cmake-2.8.10.2/Docs/cmake-properties.html" "/source/cmake-2.8.10.2/Docs/cmake-variables.html" "/source/cmake-2.8.10.2/Docs/cmake-modules.html" "/source/cmake-2.8.10.2/Docs/cmake-commands.html" "/source/cmake-2.8.10.2/Docs/cmake-compatcommands.html" "/source/cmake-2.8.10.2/Docs/ctest.html" "/source/cmake-2.8.10.2/Docs/cpack.html" "/source/cmake-2.8.10.2/Docs/ccmake.html")
ADD_TEST(CMake.DocBook "/usr/bin/xmllint" "--valid" "--noout" "--nonet" "--path" "/source/cmake-2.8.10.2/Utilities/xml/docbook-4.5" "/source/cmake-2.8.10.2/Docs/cmake.docbook" "/source/cmake-2.8.10.2/Docs/ctest.docbook" "/source/cmake-2.8.10.2/Docs/cpack.docbook" "/source/cmake-2.8.10.2/Docs/ccmake.docbook")
SUBDIRS(Doxygen)
SUBDIRS(KWStyle)
