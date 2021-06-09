#include <AVSCommon/Utils/Logger/Logger.h>

#include "BlueZ/BlueZConstants.h"
#include "BlueZ/BlueZDeviceManager.h"

namespace alexaClientSDK {
namespace bluetoothImplementations {
namespace blueZ {


/// DBus object root path
static const char* OBJECT_PATH_ROOT = "/";

/// String to identify log entries originating from this file.
static const std::string TAG{"BlueZDeviceManager"};

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)


std::shared_ptr<BlueZDeviceManager> BlueZDeviceManager::create() {
    ACSDK_DEBUG5(LX(__func__));

    std::shared_ptr<BlueZDeviceManager> deviceManager(new BlueZDeviceManager());
    if (!deviceManager->init()) {
        return nullptr;
    }

    return deviceManager;
}

bool BlueZDeviceManager::init() {
    ACSDK_DEBUG5(LX(__func__));
    ACSDK_DEBUG5(LX("Creating connection..."));
    m_connection = DBusConnection::create();
    if (nullptr == m_connection) {
        ACSDK_ERROR(LX("initFailed").d("reason", "Failed to create DBus connection"));
        return false;
    }

    ACSDK_DEBUG5(LX("Creating ObjectManagerProxy..."));
    // Create ObjectManager proxy used to find Adapter to use and a list of known Devices
    m_objectManagerProxy = DBusProxy::create(BlueZConstants::OBJECT_MANAGER_INTERFACE, OBJECT_PATH_ROOT);
    if (nullptr == m_objectManagerProxy) {
        ACSDK_ERROR(LX("initFailed").d("Failed to create ObjectManager proxy", ""));
        return false;
    }

    m_workerContext = g_main_context_new();
    if (nullptr == m_workerContext) {
        ACSDK_ERROR(LX("initFailed").d("reason", "Failed to create glib main context"));
        return false;
    }

    m_eventLoop = g_main_loop_new(m_workerContext, false);
    if (nullptr == m_eventLoop) {
        ACSDK_ERROR(LX("initFailed").d("reason", "Failed to create glib main loop"));
        return false;
    }

    m_eventThread = std::thread(&BlueZDeviceManager::mainLoopThread, this);

    if (!m_mainLoopInitPromise.get_future().get()) {
        ACSDK_ERROR(LX("initFailed").d("reason", "failed to initialize glib main loop"));
        return false;
    }

    ACSDK_DEBUG5(LX("BlueZDeviceManager initialized..."));
    
    return true;

}

void BlueZDeviceManager::interfacesAddedCallback(
    GDBusConnection* conn,
    const gchar* sender_name,
    const gchar* object_path,
    const gchar* interface_name,
    const gchar* signal_name,
    GVariant* parameters,
    gpointer deviceManager) {
    ACSDK_DEBUG5(LX(__func__));

    if (nullptr == parameters) {
        ACSDK_ERROR(LX("interfacesAddedCallbackFailed").d("reason", "parameters are null"));
        return;
    }

    if (nullptr == deviceManager) {
        ACSDK_ERROR(LX("interfacesAddedCallbackFailed").d("reason", "deviceManager is null"));
        return;
    }

    GVariantTupleReader tupleReader(parameters);
    char* addedObjectPath = tupleReader.getObjectPath(0);
    ManagedGVariant interfacesChangedMap = tupleReader.getVariant(1);

    static_cast<BlueZDeviceManager*>(deviceManager)->onInterfaceAdded(addedObjectPath, interfacesChangedMap);
}

void BlueZDeviceManager::interfacesRemovedCallback(
    GDBusConnection* conn,
    const gchar* sender_name,
    const gchar* object_path,
    const gchar* interface_name,
    const gchar* signal_name,
    GVariant* variant,
    gpointer deviceManager) {
    ACSDK_DEBUG5(LX(__func__));

    char* interfaceRemovedPath;

    if (nullptr == variant) {
        ACSDK_ERROR(LX("interfacesRemovedCallbackFailed").d("reason", "variant is null"));
        return;
    }

    if (nullptr == deviceManager) {
        ACSDK_ERROR(LX("interfacesRemovedCallbackFailed").d("reason", "deviceManager is null"));
        return;
    }

    g_variant_get(variant, "(oas)", &interfaceRemovedPath, NULL);

    static_cast<BlueZDeviceManager*>(deviceManager)->onInterfaceRemoved(interfaceRemovedPath);
}


void BlueZDeviceManager::onInterfaceAdded(const char* objectPath, ManagedGVariant& interfacesChangedMap) {
    ACSDK_DEBUG7(LX(__func__).d("path", objectPath));
    ACSDK_DEBUG9(LX(__func__).d("Details", g_variant_print(interfacesChangedMap.get(), true)));
}

void BlueZDeviceManager::onInterfaceRemoved(const char* objectPath) {
    ACSDK_DEBUG7(LX(__func__));
}

BlueZDeviceManager::~BlueZDeviceManager() {
    ACSDK_DEBUG5(LX(__func__));
}

BlueZDeviceManager::BlueZDeviceManager() {
}

void BlueZDeviceManager::mainLoopThread() {
    g_main_context_push_thread_default(m_workerContext);

    // Connect signals.
    ACSDK_DEBUG5(LX("Connecting signals..."));

    do {
        // New interface added. Used to track new devices found by BlueZ
        int subscriptionId = m_connection->subscribeToSignal(
            BlueZConstants::BLUEZ_SERVICE_NAME,
            BlueZConstants::OBJECT_MANAGER_INTERFACE,
            "InterfacesAdded",
            nullptr,
            BlueZDeviceManager::interfacesAddedCallback,
            this);

        if (0 == subscriptionId) {
            ACSDK_ERROR(LX("initFailed").d("reason", "failed to subscribe to InterfacesAdded signal"));
            m_mainLoopInitPromise.set_value(false);
            break;
        }

        // Track device removal
        subscriptionId = m_connection->subscribeToSignal(
            BlueZConstants::BLUEZ_SERVICE_NAME,
            BlueZConstants::OBJECT_MANAGER_INTERFACE,
            "InterfacesRemoved",
            nullptr,
            BlueZDeviceManager::interfacesRemovedCallback,
            this);

        if (0 == subscriptionId) {
            ACSDK_ERROR(LX("initFailed").d("reason", "failed to subscribe to InterfacesRemoved signal"));
            m_mainLoopInitPromise.set_value(false);
            break;
        }
#if 0
        // Track properties changes
        subscriptionId = m_connection->subscribeToSignal(
            BlueZConstants::BLUEZ_SERVICE_NAME,
            BlueZConstants::PROPERTIES_INTERFACE,
            "PropertiesChanged",
            nullptr,
            BlueZDeviceManager::propertiesChangedCallback,
            this);
#endif
        if (0 == subscriptionId) {
            ACSDK_ERROR(LX("initFailed").d("reason", "failed to subscribe to PropertiesChanged signal"));
            m_mainLoopInitPromise.set_value(false);
            break;
        }

        m_mainLoopInitPromise.set_value(true);

        g_main_loop_run(m_eventLoop);
    } while (false);

    g_main_loop_unref(m_eventLoop);
    g_main_context_pop_thread_default(m_workerContext);
    g_main_context_unref(m_workerContext);
}



}  // namespace blueZ
}  // namespace bluetoothImplementations
}  // namespace alexaClientSDK
