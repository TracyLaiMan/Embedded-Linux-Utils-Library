#ifndef ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZBLEADVERTISING_H_
#define ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZBLEADVERTISING_H_

#include <string.h>
#include <memory>
#include <mutex>

#include "BlueZ/DBusPropertiesProxy.h"

#include "BlueZ/BlueZConstants.h"

#include "BlueZ/DBusConnection.h"

#include "BlueZ/DBusObject.h"

namespace alexaClientSDK {
namespace bluetoothImplementations {
namespace blueZ {

class BlueZBleBleAdvertising : public DBusObject<BlueZBleBleAdvertising> {

public:
    /**                                                                                 
     * Constructor                                                                      
     *
     * @param adapterObjectPath The object path of the adapter.
     */
    BlueZBleBleAdvertising(std::shared_ptr<DBusConnection> connection, const std::string& advertisingPath);

    void callRelease();
    void advRegister();
private:

    void onRelease(GVariant* arguments, GDBusMethodInvocation* invocation);



    /// The BlueZ object path of the adapter.
    std::string m_adapterObjectPath;

    /// A proxy of the Adapter Properties interface.
    std::shared_ptr<DBusPropertiesProxy> m_adapterProperties;

};

}  // namespace blueZ
}  // namespace bluetoothImplementations
}  // namespace alexaClientSDK


#endif // ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_BLUEZBLEADVERTISING_H_