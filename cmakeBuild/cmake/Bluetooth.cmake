#
# Setup the Bluetooth compiler options.
#
# BlueZ is currently the only Bluetooth implementation provided with the SDK.
#
# However, if you provide your own Bluetooth implementation, if your application is using the manufactory
# you can specify ACSDKBLUETOOTHIMPLEMENTATION_LIB as a cmake argument and provide the path to your own component.
# See applications/acsdkBlueZBluetoothImplementation for the BlueZ example of this.
#
# To build with Bluetooth capabilities using the BlueZ implementation, specify:
# cmake <path-to-source> -DBLUETOOTH_ENABLED=ON -DBLUETOOTH_BLUEZ=ON

option(BLUETOOTH_ENABLED "Enable Bluetooth" ON)
option(BLUETOOTH_BLUEZ "Use the BlueZ implementation of Bluetooth" ON)

if (BLUETOOTH_BLUEZ)
    # To maintain backwards compatibility, when BLUETOOTH_BLUEZ=ON, BLUETOOTH_ENABLED=ON by default.
    if (NOT BLUETOOTH_ENABLED)
        set(BLUETOOTH_ENABLED ON)
    endif()

    message("Creating ${PROJECT_NAME} with BlueZ implementation of Bluetooth")
    find_package(PkgConfig)
    pkg_check_modules(GIO REQUIRED gio-2.0>=2.4)
    pkg_check_modules(GIO_UNIX REQUIRED gio-unix-2.0>=2.4)
    add_definitions(-DBLUETOOTH_BLUEZ)

endif()

if (BLUETOOTH_ENABLED)
    add_definitions(-DBLUETOOTH_ENABLED)
endif()
