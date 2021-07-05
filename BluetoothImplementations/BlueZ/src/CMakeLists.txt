add_definitions("-DACSDK_LOG_MODULE=bluetoothImplementationsBlueZ")
add_definitions(-DACSDK_DEBUG_LOG_ENABLED)

add_library(BluetoothImplementationsBlueZ SHARED 
    DBusConnection.cpp
    DBusObjectBase.cpp
    DBusPropertiesProxy.cpp
    DBusProxy.cpp
    GVariantMapReader.cpp
    GVariantTupleReader.cpp
    BlueZDeviceManager.cpp
    BlueZBleAdvertising.cpp
    )

target_include_directories(BluetoothImplementationsBlueZ PUBLIC
    "${BluetoothImplementationsBlueZ_SOURCE_DIR}/include"
    "${GIO_INCLUDE_DIRS}"
    "${GIO_UNIX_INCLUDE_DIRS}"
    "${AVSCommon_INCLUDE_DIRS}")

target_link_libraries(BluetoothImplementationsBlueZ "${GIO_LDFLAGS}" AVSCommon)
