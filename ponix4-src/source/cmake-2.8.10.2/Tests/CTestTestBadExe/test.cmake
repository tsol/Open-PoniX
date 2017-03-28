cmake_minimum_required(VERSION 2.1)

# Settings:
set(CTEST_DASHBOARD_ROOT                "/source/cmake-2.8.10.2/Tests/CTestTest")
set(CTEST_SITE                          "ubuntu4")
set(CTEST_BUILD_NAME                    "CTestTest-Linux-i486-TSOL-linux-gnu-g++-BadExe")

set(CTEST_SOURCE_DIRECTORY              "/source/cmake-2.8.10.2/Tests/CTestTestBadExe")
set(CTEST_BINARY_DIRECTORY              "/source/cmake-2.8.10.2/Tests/CTestTestBadExe")
set(CTEST_CVS_COMMAND                   "CVSCOMMAND-NOTFOUND")
set(CTEST_CMAKE_GENERATOR               "Unix Makefiles")
set(CTEST_BUILD_CONFIGURATION           "$ENV{CMAKE_CONFIG_TYPE}")
set(CTEST_COVERAGE_COMMAND              "COVERAGE_COMMAND-NOTFOUND")
set(CTEST_NOTES_FILES                   "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}")

#CTEST_EMPTY_BINARY_DIRECTORY(${CTEST_BINARY_DIRECTORY})

CTEST_START(Experimental)
CTEST_CONFIGURE(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
CTEST_BUILD(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
CTEST_TEST(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE res)
