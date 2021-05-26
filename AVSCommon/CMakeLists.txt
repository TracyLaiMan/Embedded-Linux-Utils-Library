project(AVSCommon)

# specify the C++ standard C++11的支持
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions("-DACSDK_LOG_MODULE=avsCommon")
add_definitions("-DRAPIDJSON_INCLUDE_DIR=../ThirdParty/rapidjson/rapidjson-1.1.0/include/")

add_subdirectory("Utils")

add_library(AVSCommon SHARED
    Utils/src/SafeCTimeAccess.cpp
    Utils/src/JSON/JSONGenerator.cpp
    Utils/src/JSON/JSONUtils.cpp
    Utils/src/Configuration/ConfigurationNode.cpp
    Utils/src/Logger/ConsoleLogger.cpp
    Utils/src/Logger/Level.cpp
    Utils/src/Logger/LogEntry.cpp
    Utils/src/Logger/LogEntryBuffer.cpp
    Utils/src/Logger/LogEntryStream.cpp
    Utils/src/Logger/Logger.cpp
    Utils/src/Logger/LoggerSinkManager.cpp
    Utils/src/Logger/LoggerUtils.cpp
    Utils/src/Logger/LogStringFormatter.cpp
    Utils/src/Logger/ModuleLogger.cpp
    Utils/src/Logger/ThreadMoniker.cpp)

target_include_directories(AVSCommon PUBLIC
    "${AVSCommon_SOURCE_DIR}/Utils/include"
    "${AVSCommon_SOURCE_DIR}/../ThirdParty/rapidjson/rapidjson-1.1.0/include/")