//
//
//      flt
//      device_manager.hpp
//

#pragma once

#include <types.hpp>

#include <uti/container/vector.hpp>

#include <iterator>
#include <algorithm>

#define make_device_id(productID, vendorID) ( ( ( ( productID ) % 0xFFFF ) << 16 ) | ( ( vendorID ) & 0xFFFF ) )


namespace flt
{


void set_applier_function_copy_to_cfarray ( void const * value, void * context ) ;

CFStringRef get_property_string ( IOHIDDeviceRef hid_device, CFStringRef property ) ;
uti::u32_t  get_property_number ( IOHIDDeviceRef hid_device, CFStringRef property ) ;

IOReturn  open_device ( hid_device const & device ) ;
IOReturn close_device ( hid_device const & device ) ;


class device_manager
{
public:
        device_manager () noexcept : hid_manager_( _create_hid_manager() ) {}

        auto list_devices () -> uti::vector< hid_device >
        {
                uti::vector< hid_device > dev_set ;

                CFSetRef device_setref = IOHIDManagerCopyDevices( hid_manager_ ) ;
                CFIndex count = CFSetGetCount( device_setref ) ;

                CFMutableArrayRef device_arrayref = CFArrayCreateMutable( kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks ) ;

                CFSetApplyFunction( device_setref, set_applier_function_copy_to_cfarray, static_cast< void * >( device_arrayref ) ) ;

                for( CFIndex i = 0; i < count; ++i )
                {
                        hid_device_t * device = ( hid_device_t * ) CFArrayGetValueAtIndex( device_arrayref, i ) ;

                        uti::u32_t  vendor_id = get_property_number( device, CFSTR( kIOHIDVendorIDKey  ) ) ;
                        uti::u32_t product_id = get_property_number( device, CFSTR( kIOHIDProductIDKey ) ) ;

                        device_id_t device_id = make_device_id( product_id, vendor_id ) ;

                        hid_device device_data{ vendor_id, product_id, device_id, device } ;

                        dev_set.push_back( device_data ) ;
                }
                return dev_set ;
        }

        auto find_known_wheels () -> uti::vector< hid_device >
        {
                uti::vector< hid_device > dev_set = list_devices() ;
                uti::vector< hid_device > wheels ;

                std::copy_if( dev_set.begin(), dev_set.end(), std::back_inserter( wheels ),
                             [ & ]( hid_device const & dev )
                             {
                                return std::find( known_wheel_ids.begin(), known_wheel_ids.end(), dev.device_id_ ) != known_wheel_ids.end() ;
                             }
                ) ;
                return wheels ;
        }

        ~device_manager () { _destroy_hid_manager( hid_manager_ ) ; }
private:
        hid_manager_t * hid_manager_ ;

        hid_manager_t * _create_hid_manager ()
        {
                hid_manager_t * manager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDManagerOptionNone ) ;
                IOHIDManagerSetDeviceMatching( manager, NULL ) ;
                IOHIDManagerOpen( manager, kIOHIDOptionsTypeSeizeDevice ) ;
                return manager ;
        }
        void _destroy_hid_manager ( hid_manager_t * manager )
        {
                IOHIDManagerClose( manager, kIOHIDManagerOptionNone ) ;
        }
} ;


} // namespace flt
