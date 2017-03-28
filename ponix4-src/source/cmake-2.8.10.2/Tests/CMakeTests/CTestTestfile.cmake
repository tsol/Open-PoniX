# CMake generated Testfile for 
# Source directory: /source/cmake-2.8.10.2/Tests/CMakeTests
# Build directory: /source/cmake-2.8.10.2/Tests/CMakeTests
# 
# This file includes the relevent testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
ADD_TEST(CMake.List "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ListTest.cmake")
ADD_TEST(CMake.VariableWatch "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/VariableWatchTest.cmake")
ADD_TEST(CMake.Include "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/IncludeTest.cmake")
ADD_TEST(CMake.FindBase "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/FindBaseTest.cmake")
ADD_TEST(CMake.Toolchain "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ToolchainTest.cmake")
ADD_TEST(CMake.GetFilenameComponentRealpath "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/GetFilenameComponentRealpathTest.cmake")
ADD_TEST(CMake.Version "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/VersionTest.cmake")
ADD_TEST(CMake.Message "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/MessageTest.cmake")
ADD_TEST(CMake.File "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/FileTest.cmake")
ADD_TEST(CMake.ConfigureFile "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ConfigureFileTest.cmake")
ADD_TEST(CMake.SeparateArguments "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/SeparateArgumentsTest.cmake")
ADD_TEST(CMake.ImplicitLinkInfo "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ImplicitLinkInfoTest.cmake")
ADD_TEST(CMake.ModuleNotices "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ModuleNoticesTest.cmake")
ADD_TEST(CMake.GetProperty "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/GetPropertyTest.cmake")
ADD_TEST(CMake.If "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/IfTest.cmake")
ADD_TEST(CMake.String "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/StringTest.cmake")
ADD_TEST(CMake.Math "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/MathTest.cmake")
ADD_TEST(CMake.CMakeMinimumRequired "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/CMakeMinimumRequiredTest.cmake")
ADD_TEST(CMake.CompilerIdVendor "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/CompilerIdVendorTest.cmake")
ADD_TEST(CMake.ProcessorCount "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ProcessorCountTest.cmake")
ADD_TEST(CMake.PushCheckState "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/PushCheckStateTest.cmake")
ADD_TEST(CMake.While "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/WhileTest.cmake")
ADD_TEST(CMake.FileDownload "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/FileDownloadTest.cmake")
SET_TESTS_PROPERTIES(CMake.FileDownload PROPERTIES  PASS_REGULAR_EXPRESSION "file already exists with expected MD5 sum")
ADD_TEST(CMake.FileUpload "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/FileUploadTest.cmake")
ADD_TEST(CMake.ELF "/source/cmake-2.8.10.2/bin/cmake" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/ELFTest.cmake")
ADD_TEST(CMake.EndStuff "/source/cmake-2.8.10.2/bin/cmake" "-Ddir:STRING=/source/cmake-2.8.10.2/Tests/CMakeTests/EndStuffTest" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/EndStuffTest.cmake")
ADD_TEST(CMake.GetPrerequisites "/source/cmake-2.8.10.2/bin/cmake" "-DCTEST_CONFIGURATION_TYPE:STRING=${CTEST_CONFIGURATION_TYPE}" "-P" "/source/cmake-2.8.10.2/Tests/CMakeTests/GetPrerequisitesTest.cmake")
