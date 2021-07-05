/*
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */

#ifndef ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_DBUSOBJECT_H_
#define ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_DBUSOBJECT_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "BlueZ/DBusObjectBase.h"

namespace alexaClientSDK {
namespace bluetoothImplementations {
namespace blueZ {

/**
 * Typed version of the base class for objects exposed to DBus. This class simplifies the handling of DBus method calls
 * by binding them to class member functions
 *
 * @tparam T Name of the derived class to be used to imlpement the object. The derived class declaration would look
 * like this:
 * @code
 * class MyDBusObjectClass : public DBusObject<MyDBusObjectClass> {
 * };
 * @endcode
 */
template <class T>
class DBusObject : public DBusObjectBase {
public:
    virtual ~DBusObject() = default;
    /**
     * Callback type to be used by member functions handling DBus method calls
     *
     * @param parameters Parameters provided to method call
     * @param invocation @ GDBusMethodInvocation object associated with a method call
     */
    using commandHandler_t = void (T::*)(GVariant* parameters, GDBusMethodInvocation* invocation);

protected:
    /**
     * Constructor
     *
     * @param connection DBus connection to be used to register this obejct
     * @param xmlInterfaceIntrospection XML containing the description of the interfaces this object implements. See
     * <a href="https://dbus.freedesktop.org/doc/dbus-api-design.html"/>
     * @param objectPath Path to register the object to
     * @param methodMap A Method name to callback function map
     */
    DBusObject(
        std::shared_ptr<DBusConnection> connection,
        std::string xmlInterfaceIntrospection,
        std::string objectPath,
        std::unordered_map<std::string, commandHandler_t> methodMap) :
            DBusObjectBase(connection, xmlInterfaceIntrospection, objectPath, &onMethodCallStatic, &onGetPropertyStatic),
            m_commands{methodMap} {
    }

private:
    /**
     * Static method used as a callback to subscribe to DBus method calls for the object
     *
     * @param conn DBus connection this object belonging to
     * @param sender_name Name of the service calling the method
     * @param object_path Object path of the service instance calling the method
     * @param interface_name Name of the interface this method call belongs to
     * @param method_name Name of the method DBus wants this object to invoke
     * @param parameters @c GVariant containing a tuple with parameters passed to the method
     * @param invocation A @c GDBusMethodInvocation object to report method return values or errors. See <a
     * href="https://developer.gnome.org/gio/stable/GDBusMethodInvocation.html#g-dbus-method-invocation-return-value">
     * g_dbus_method_invocation_return_value()</a> for details.
     * @param data User data that was previously passed to DBus during the object registration
     */
    static void onMethodCallStatic(
        GDBusConnection* conn,
        const gchar* sender_name,
        const gchar* object_path,
        const gchar* interface_name,
        const gchar* method_name,
        GVariant* parameters,
        GDBusMethodInvocation* invocation,
        gpointer data) {
        auto thisPtr = static_cast<T*>(data);
        printf("hahhahahahahahah------------------1\n");
        thisPtr->onMethodCall(method_name, parameters, invocation);
    }

    /**
     * Internal method routing the DBus method calls to proper implementation member functions of the derived class
     *
     * @param method_name Name of the method being invoked
     * @param parameters Parameters of the method
     * @param invocation  @c GDBusMethodInvocation object to report method return values or errors. See <a
     * href="https://developer.gnome.org/gio/stable/GDBusMethodInvocation.html#g-dbus-method-invocation-return-value">
     * g_dbus_method_invocation_return_value()</a> for details.
     */
    void onMethodCall(const gchar* method_name, GVariant* parameters, GDBusMethodInvocation* invocation);

    static GVariant* onGetPropertyStatic(
        GDBusConnection* conn,
        const gchar* sender_name,
        const gchar* object_path,
        const gchar* interface_name,
        const gchar* property_name,
        GError **errors,
        gpointer data) {
        auto thisPtr = static_cast<T*>(data);
        printf("hahhahahahahahah----------------%s-：-%s\n",interface_name,property_name);
        thisPtr->onGetProperty(property_name);
    }

    GVariant* onGetProperty(const gchar* property_name);

    // Map between method names and member functions implementing them
    std::unordered_map<std::string, commandHandler_t> m_commands;

};

template <class T>
void DBusObject<T>::onMethodCall(const gchar* method_name, GVariant* parameters, GDBusMethodInvocation* invocation) {
    onMethodCalledInternal(method_name);

  {
    // Build Dict of properties.
    GVariantBuilder *builder;
    builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));

    //g_variant_builder_init(advBuilder, G_VARIANT_TYPE("a{sv}"));
    //g_variant_builder_add(&builder, "{sv}", CAN_GO_NEXT.c_str(), g_variant_new("(s)","peripheral"));
    g_variant_builder_add(builder,"{sv}","Type",g_variant_new_string("peripheral"));
    g_variant_builder_add(builder,"{sv}","LocalName",g_variant_new_string("TracyA"));

    //GVariant* properties = g_variant_builder_end(builder);
    g_dbus_method_invocation_return_value(invocation, g_variant_new("(a{sv})",builder));
  }

    // GVariantBuilder *advBuilder;
    // advBuilder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
    // g_variant_builder_add(advBuilder, "({sv})", "Type", g_variant_new_string("peripheral"));
    // GVariant* adv = g_variant_builder_end(advBuilder);
    // g_dbus_method_invocation_return_value(invocation, g_variant_new("(a{sv})",adv));
#if 0
    auto iter = m_commands.find(method_name);
    if (iter != m_commands.end()) {
        commandHandler_t handler = iter->second;
        (static_cast<T*>(this)->*handler)(parameters, invocation);
    } else {
        g_dbus_method_invocation_return_error(
            invocation, G_DBUS_ERROR, G_DBUS_ERROR_UNKNOWN_METHOD, "Unknown method: test");
    }
#endif
}

template <class T>
GVariant* DBusObject<T>::onGetProperty(const gchar* property_name) {

    printf("aaaaaaaa-------------------\r\n");
    // auto iter = m_property_commands.find(property_name);
    // if (iter != m_property_commands.end()) {
    //     commandPropertyHandler_t handler = iter->second;
    //     (static_cast<T*>(this)->*handler)(property_name);
    // }
}


}  // namespace blueZ
}  // namespace bluetoothImplementations
}  // namespace alexaClientSDK

#endif  // ALEXA_CLIENT_SDK_BLUETOOTHIMPLEMENTATIONS_BLUEZ_INCLUDE_BLUEZ_DBUSOBJECT_H_
