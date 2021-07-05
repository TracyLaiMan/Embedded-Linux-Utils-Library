#ifndef ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZDEVICEMANAGER_H_
#define ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZDEVICEMANAGER_H_

#include <list>
#include <map>
#include <mutex>
#include <future>
#include <thread>

#include "BlueZUtils.h"
#include "BlueZ/DBusPropertiesProxy.h"
#include "BlueZ/DBusConnection.h"
#include "BlueZ/BlueZBleAdvertising.h"
#include <gio/gio.h>

namespace alexaClientSDK {
namespace bluetoothImplementations {
namespace blueZ {

class BlueZDeviceManager{

public:
    /**
     * Factory method to create a class.
     */
    static std::shared_ptr<BlueZDeviceManager> create();

    /**
     * Destructor.
     */
    virtual ~BlueZDeviceManager();

private:

    /**
     * A constructor
     */
    BlueZDeviceManager();

    /**
     * Method used to initialize a fresh @c BlueZDeviceManager object before returning it by @c create() function.
     *
     * @return true on success
     */
    bool init();

    /**
     * DBus callback called when BlueZ service has a new interface implemented by an object in a DBus object tree.
     *
     * @param conn DBus connection
     * @param sender_name The sender service name
     * @param object_path Path of the sender object
     * @param interface_name Name of the interface that sends the signal
     * @param signal_name Name of the signal. "InterfacesAdded" in this case.
     * @param parameters Parameters of the signal, containing the description of all added interfaces.
     * @param data User data associated with signal
     */
    static void interfacesAddedCallback(
        GDBusConnection* conn,
        const gchar* sender_name,
        const gchar* object_path,
        const gchar* interface_name,
        const gchar* signal_name,
        GVariant* parameters,
        gpointer data);

    /**
     * DBus callback called when BlueZ service loses an interface implementation
     *
     * @param conn DBus connection
     * @param sender_name The sender service name
     * @param object_path Path of the sender object
     * @param interface_name Name of the interface that sends the signal
     * @param signal_name Name of the signal. "InterfacesRemoved" in this case.
     * @param parameters Parameters of the signal, containing the description of the removed interfaces.
     * @param data User data associated with signal
     */
    static void interfacesRemovedCallback(
        GDBusConnection* conn,
        const gchar* sender_name,
        const gchar* object_path,
        const gchar* interface_name,
        const gchar* signal_name,
        GVariant* parameters,
        gpointer data);

     /**
     * Handles the new interface being added to BlueZ objects.
     *
     * @param objectPath Object path for the object implementing new interfaces.
     * @param interfacesChangedMap Variant with the interfaces changes.
     */
    void onInterfaceAdded(const char* objectPath, ManagedGVariant& interfacesChangedMap);

    /**
     * Handles the removal of the DBus interfaces. In our case it also means that the object is deleted.
     *
     * @param objectPath Path of the object.
     */
    void onInterfaceRemoved(const char* objectPath);

        /**
     * Retrieves the current state from BlueZ service and updates internal state accordingly.
     */
    bool getStateFromBlueZ();

     /// Thread procedure to setup and handle GLib events
    void mainLoopThread();

    bool initializeBleAdvertising();

    std::shared_ptr<BlueZBleBleAdvertising> m_blueZBleBleAdvertising;

    /// DBus object path of the hardware bluetooth adapter used by device manager
    std::string m_adapterPath;

    /// DBus proxy for ObjectManager interface of the BlueZ service
    std::shared_ptr<DBusProxy> m_objectManagerProxy;

    /// DBus proxy for Media1 interface of the BlueZ service
    std::shared_ptr<DBusProxy> m_mediaProxy;

       // TODO Having a separate thread for event listening is wasteful. Can optimize with a BlueZ bus listener.
    /// Event loop to listen for signals.
    GMainLoop* m_eventLoop;

    /// GLib context to run event loop in.
    GMainContext* m_workerContext;

    /// DBus connection
    std::shared_ptr<DBusConnection> m_connection;

    /// Mutex to synchronize known device list access.
    mutable std::mutex m_devicesMutex;

     /// Promise to hold the result of a glib's main loop thread initialization.
    std::promise<bool> m_mainLoopInitPromise;

    /// Thread to run event listener on.
    std::thread m_eventThread;

};


}  // namespace blueZ
}  // namespace bluetoothImplementations
}  // namespace alexaClientSDK

#endif // ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZDEVICEMANAGER_H_
