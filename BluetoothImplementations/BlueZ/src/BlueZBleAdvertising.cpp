
#include "BlueZ/BlueZBleAdvertising.h"

#include <AVSCommon/Utils/Logger/Logger.h>

namespace alexaClientSDK {
namespace bluetoothImplementations {
namespace blueZ {

/// String to identify log entries originating from this file.
static const std::string TAG{"BlueZBleBleAdvertising"};

/**
 * Create a LogEntry using this file's TAG and the specified event string.
 *
 * @param The event string for this @c LogEntry.
 */
#define LX(event) alexaClientSDK::avsCommon::utils::logger::LogEntry(TAG, event)

/// Name of the BlueZ MediaEndpoint1::Release method.
constexpr const char* LEADVERTISEMENT1_RELEASE_METHOD_NAME = "Release";

/**
 * XML description of the MediaEndpoint1 interface to be implemented by this obejct. The format is defined by DBus.
 * This data is used during the registration of the media endpoint object.
 */
static const gchar bleAdvertisingIntrospectionXml[] =
    "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection "
    "1.0//EN\"\n"
    "\"http://www.freedesktop.org/standards/dbus/1.0/introspect.dtd\";>\n"
    "<node>"
    " <interface name=\"org.bluez.LEAdvertisement1\">"
    "  <method name=\"Release\">"
    "  </method>"
    "  <property name=\"Type\" type=\"s\" access=\"read\"/>"
    " </interface>"
    " <interface name=\"org.freedesktop.DBus.Properties\">"
    "  <method name=\"GetAll\">"
    "   <arg name=\"InterfaceName\" direction=\"in\" type=\"s\"/>"
    "   <arg name=\"properties\" direction=\"out\" type=\"a{sv}\"/>"
    "  </method>"
    "  <property name=\"Type\" type=\"s\" access=\"read\"/>"
    " </interface>"
    "</node>";



BlueZBleBleAdvertising::BlueZBleBleAdvertising(std::shared_ptr<DBusConnection> connection, const std::string& advertisingPath) :
    DBusObject(
        connection,
        bleAdvertisingIntrospectionXml,
        advertisingPath,
        {{LEADVERTISEMENT1_RELEASE_METHOD_NAME, &BlueZBleBleAdvertising::onRelease}})
    {
}

void BlueZBleBleAdvertising::onRelease(GVariant* arguments, GDBusMethodInvocation* invocation) {
    ACSDK_DEBUG5(LX(__func__));
    g_dbus_method_invocation_return_value(invocation, nullptr);
}



void BlueZBleBleAdvertising::callRelease() {
    ManagedGError error;
    auto m_adapter = DBusProxy::create("org.bluez.LEAdvertisement1", "/com/bluez/advertising");
    m_adapter->callMethod("Release", nullptr, error.toOutputParameter() );
}

void BlueZBleBleAdvertising::advRegister() {

    ACSDK_DEBUG5(LX("advRegister ...---------------1"));
     ACSDK_DEBUG5(LX("advRegister ...---------------1-1-1"));
    ManagedGError error;
    auto m_adapter = DBusProxy::create("org.bluez.LEAdvertisingManager1", "/org/bluez/hci0");
    ACSDK_DEBUG5(LX("advRegister ...---------------1-1"));
    // //"/com/bluez/advertising"
    GVariantBuilder *b;
    GVariantBuilder *advBuilder;
    ACSDK_DEBUG5(LX("advRegister ...---------------1-2"));
    

    advBuilder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    ACSDK_DEBUG5(LX("advRegister ...---------------1-3"));
    //g_variant_builder_add(advBuilder, "{sv}", "Codec", g_variant_new_string("test"));
    ACSDK_DEBUG5(LX("advRegister ...---------------2"));
    GVariant* adv = g_variant_builder_end(advBuilder);
    ACSDK_DEBUG5(LX("advRegister ...---------------3"));


    // b = g_variant_builder_new(G_VARIANT_TYPE("s"));
    // g_variant_builder_add(b, "s", "/com/bluez/advertising");


    // GVariant* parameters = g_variant_builder_end(b);



    m_adapter->callMethod("RegisterAdvertisement", g_variant_new ("(o@a{sv})","/com/bluez/advertising",adv), error.toOutputParameter() );
    ACSDK_DEBUG5(LX("advRegister ...---------------4"));
}
// std::string BlueZBleBleAdvertising::getMac(){

//     m_adapterProperties = DBusPropertiesProxy::create(m_adapterObjectPath);
//     if (!m_adapterProperties) {
//         ACSDK_ERROR(LX(__func__).d("reason", "createPropertiesProxyFailed"));
//         return nullptr;
//     }

//     // Get the MAC address.
//     std::string mac;
//     if (!m_adapterProperties->getStringProperty(
//             BlueZConstants::BLUEZ_ADAPTER_INTERFACE, BlueZConstants::BLUEZ_DEVICE_INTERFACE_ADDRESS, &mac)) {
//         ACSDK_ERROR(LX(__func__).d("reason", "noMACAddress"));
//         return nullptr;
//     }

//     auto bleProperties = DBusPropertiesProxy::create("/org/bluez/advertising");
//     if (!bleProperties) {
//         ACSDK_ERROR(LX(__func__).d("reason", "createPropertiesProxyFailed"));
//         return nullptr;
//     }

//     std::string type;
//     if (!bleProperties->getStringProperty(
//             BlueZConstants::BLUEZ_LE_ADVERTISING_INTERFACE, "LocalName", &type)) {
//         ACSDK_ERROR(LX(__func__).d("reason", "noMACAddress"));
//         return nullptr;
//     }

//     //test
//     ACSDK_DEBUG9(LX(__func__).d("Tracy::get type", type));

//     ACSDK_DEBUG9(LX(__func__).d("Tracy::get mac", mac));
//     return mac;
// }

}  // namespace blueZ
}  // namespace bluetoothImplementations
}  // namespace alexaClientSDK