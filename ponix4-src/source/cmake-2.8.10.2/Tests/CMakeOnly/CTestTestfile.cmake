# CMake generated Testfile for 
# Source directory: /source/cmake-2.8.10.2/Tests/CMakeOnly
# Build directory: /source/cmake-2.8.10.2/Tests/CMakeOnly
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(CMakeOnly.LinkInterfaceLoop "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=LinkInterfaceLoop" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
SET_TESTS_PROPERTIES(CMakeOnly.LinkInterfaceLoop PROPERTIES  TIMEOUT "90")
ADD_TEST(CMakeOnly.CheckSymbolExists "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CheckSymbolExists" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.CheckCXXSymbolExists "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CheckCXXSymbolExists" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.CheckCXXCompilerFlag "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CheckCXXCompilerFlag" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.CheckLanguage "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CheckLanguage" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.CompilerIdC "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CompilerIdC" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.CompilerIdCXX "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=CompilerIdCXX" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.AllFindModules "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=AllFindModules" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.SelectLibraryConfigurations "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=SelectLibraryConfigurations" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.TargetScope "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=TargetScope" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.find_library "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=find_library" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
ADD_TEST(CMakeOnly.ProjectInclude "/source/cmake-2.8.10.2/bin/cmake" "-DTEST=ProjectInclude" "-DCMAKE_ARGS=-DCMAKE_PROJECT_ProjectInclude_INCLUDE=/source/cmake-2.8.10.2/Tests/CMakeOnly/ProjectInclude/include.cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeOnly/Test.cmake")
