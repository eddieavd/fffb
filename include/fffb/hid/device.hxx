//
//
//      fffb
//      hid/device.hxx
//

#pragma once

#include <fffb/util/types.hxx>
#include <fffb/hid/report.hxx>


namespace fffb
{


namespace _detail
{


constexpr void set_applier_fn_copy_to_cfarray ( void const * value, void * context ) noexcept ;

[[ nodiscard ]] constexpr uti::string get_property_string ( apple::hid_device * hid_device, char const * property ) noexcept ;
[[ nodiscard ]] constexpr uti::i32_t  get_property_number ( apple::hid_device * hid_device, char const * property ) noexcept ;

[[ nodiscard ]] constexpr uti::u32_t make_device_id ( uti::u32_t product_id, uti::u32_t vendor_id ) noexcept ;


} // namespace _detail


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class hid_device
{
public:
        constexpr hid_device () noexcept
                : hid_device_( nullptr )
                ,  vendor_id_( 0 )
                , product_id_( 0 )
                ,  device_id_( 0 )
                , usage_page_( 0 )
                , usage_     ( 0 )
        {}

        constexpr hid_device ( apple::hid_device * hid_device ) noexcept
                : hid_device_( hid_device )
                ,  vendor_id_( get_property< device_id_t >( kIOHIDVendorIDKey  ) )
                , product_id_( get_property< device_id_t >( kIOHIDProductIDKey ) )
                ,  device_id_( _detail::make_device_id( product_id_, vendor_id_ ) )
                , usage_page_( get_property< device_id_t >( kIOHIDPrimaryUsagePageKey ) )
                , usage_     ( get_property< device_id_t >( kIOHIDPrimaryUsageKey ) )
        {}

        [[ nodiscard ]] constexpr operator bool () const noexcept { return hid_device_ != nullptr ; }

        [[ nodiscard ]] constexpr bool  open () const noexcept { return apple::_try( IOHIDDeviceOpen ( hid_device_, kIOHIDOptionsTypeSeizeDevice ),  "open_device" ) ; }
                        constexpr bool close () const noexcept { return apple::_try( IOHIDDeviceClose( hid_device_,                            0 ), "close_device" ) ; }

        [[ nodiscard ]] constexpr   bool write ( report const & report ) const noexcept { return write_report( hid_device_, report ) ; }
        [[ nodiscard ]] constexpr report  read (                       ) const noexcept { return  read_report( hid_device_         ) ; }

        template< typename T >
        [[ nodiscard ]] constexpr T get_property  ( char const * property ) const noexcept
        {
                if constexpr( uti::is_convertible_v< T, uti::string > )
                {
                        return _detail::get_property_string( hid_device_, property ) ;
                }
                else if constexpr( uti::is_convertible_v< T, uti::i32_t > )
                {
                        return _detail::get_property_number( hid_device_, property ) ;
                }
                else
                {
                        UTI_CEXPR_ASSERT( uti::always_false_v< T >, "fffb::hid_device::get_property< T >: requested type not supported" ) ;
                        return {} ;
                }
        }

        [[ nodiscard ]] constexpr device_id_t  vendor_id () const noexcept { return  vendor_id_ ; }
        [[ nodiscard ]] constexpr device_id_t product_id () const noexcept { return product_id_ ; }
        [[ nodiscard ]] constexpr device_id_t  device_id () const noexcept { return  device_id_ ; }

        [[ nodiscard ]] constexpr device_id_t usage_page () const noexcept { return usage_page_ ; }
        [[ nodiscard ]] constexpr device_id_t usage      () const noexcept { return usage_      ; }

        constexpr bool operator== ( hid_device const & other ) const noexcept
        {
                return hid_device_ == other.hid_device_
                    && usage_page_ == other.usage_page_
                    && usage_      == other.usage_    ;
        }
        constexpr bool operator!= ( hid_device const & other ) const noexcept { return !operator==( other ) ; }
private:
        apple::hid_device * hid_device_ ;

        device_id_t  vendor_id_ ;
        device_id_t product_id_ ;
        device_id_t  device_id_ ;
        device_id_t usage_page_ ;
        device_id_t usage_      ;
} ;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


namespace _detail
{


constexpr apple::hid_manager * _create_hid_manager (                              ) noexcept ;
constexpr void                _destroy_hid_manager ( apple::hid_manager * manager ) noexcept ;

constexpr vector< hid_device > _list_devices ( apple::hid_manager * manager ) noexcept ;


} // namespace _detail


constexpr vector< hid_device > list_hid_devices () noexcept
{
        apple::hid_manager * manager = _detail::_create_hid_manager() ;
        vector< hid_device > devices = _detail::_list_devices( manager ) ;
        _detail::_destroy_hid_manager( manager ) ;

        return devices ;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


namespace _detail
{


constexpr void set_applier_fn_copy_to_cfarray ( void const * value, void * context ) noexcept
{
        CFArrayAppendValue( static_cast< apple::array * >( context ), value ) ;
}

[[ nodiscard ]] constexpr uti::string get_property_string ( apple::hid_device * hid_device, char const * property ) noexcept
{
        auto propname = CFStringCreateWithCString( kCFAllocatorDefault, property, kCFStringEncodingASCII ) ;

        apple::type data_ref = IOHIDDeviceGetProperty( hid_device, propname ) ;
        CFRelease( propname ) ;

        apple::string const * data_str = CFStringCreateCopy( kCFAllocatorDefault, CFStringRef( data_ref ) ) ;
        char          const *    c_str = CFStringGetCStringPtr( data_str, kCFStringEncodingASCII ) ;

        if( !c_str )
        {
                return {} ;
        }
        uti::string value( c_str ) ;
        CFRelease( data_str ) ;

        return value ;
}

[[ nodiscard ]] constexpr uti::i32_t get_property_number ( apple::hid_device * hid_device, char const * property ) noexcept
{
        auto propname = CFStringCreateWithCString( kCFAllocatorDefault, property, kCFStringEncodingASCII ) ;

        apple::type data_ref = IOHIDDeviceGetProperty( hid_device, propname ) ;

        CFRelease( propname ) ;

        if( data_ref && ( CFNumberGetTypeID() == CFGetTypeID( data_ref ) ) )
        {
                uti::i32_t number ;
                CFNumberGetValue( static_cast< apple::number const * >( data_ref ), kCFNumberSInt32Type, &number ) ;
                return number ;
        }
        return 0 ;
}

[[ nodiscard ]] constexpr uti::u32_t make_device_id ( uti::u32_t product_id, uti::u32_t vendor_id ) noexcept
{
        return ( ( product_id & 0xFFFF ) << 16 ) | ( vendor_id & 0xFFFF ) ;
}


constexpr apple::hid_manager * _create_hid_manager () noexcept
{
        apple::hid_manager * manager = IOHIDManagerCreate( kCFAllocatorDefault, kIOHIDManagerOptionNone ) ;
        IOHIDManagerSetDeviceMatching( manager, nullptr ) ;
        IOHIDManagerOpen( manager, kIOHIDOptionsTypeSeizeDevice ) ;
        FFFB_F_DBG_S( "_create_hid_manager", "device manager created." ) ;
        return manager ;
}

constexpr void _destroy_hid_manager ( apple::hid_manager * manager ) noexcept
{
        if( manager ) IOHIDManagerClose( manager, kIOHIDManagerOptionNone ) ;
        FFFB_F_DBG_S( "_destroy_hid_manager", "device manager destroyed." ) ;
}

constexpr vector< hid_device > _list_devices ( apple::hid_manager * manager ) noexcept
{
        vector< hid_device > devices ;

        apple::set const * device_set = IOHIDManagerCopyDevices( manager ) ;
        apple::index       count      = CFSetGetCount( device_set ) ;

        apple::array * device_array = CFArrayCreateMutable( kCFAllocatorDefault, count, &kCFTypeArrayCallBacks ) ;

        CFSetApplyFunction( device_set, _detail::set_applier_fn_copy_to_cfarray, static_cast< void * >( device_array ) ) ;

        for( apple::index i = 0; i < count; ++i )
        {
                apple::hid_device * device = static_cast< apple::hid_device * >(
                                                const_cast< void * >( CFArrayGetValueAtIndex( device_array, i ) )
                ) ;
                devices.emplace_back( device ) ;
        }
        CFRelease( device_array ) ;
        FFFB_F_DBG_S( "_list_devices", "found %d devices", devices.size() ) ;
        return devices ;
}


} // namespace _detail


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


} // namespace fffb
