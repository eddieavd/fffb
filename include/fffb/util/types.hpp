///
///
///     fffb
///     util/types.hpp
///

#pragma once

#include <fffb/util/log.hpp>

#include <uti/type/traits.hpp>
#include <uti/container/array.hpp>
#include <uti/container/vector.hpp>

#include <IOKit/hid/IOHIDDevice.h>
#include <IOKit/hid/IOHIDManager.h>

#include <mach/mach_error.h>


namespace fffb
{


namespace apple
{


using type        = void const *   ;
using io_result   =   IOReturn     ;
using hid_device  = __IOHIDDevice  ;
using hid_manager = __IOHIDManager ;
using number      = __CFNumber     ;
using string      = __CFString     ;
using array       = __CFArray      ;
using set         = __CFSet        ;
using index       =   CFIndex      ;


constexpr bool _try ( io_result result, [[ maybe_unused ]] char const * scope ) noexcept
{
        if( result != kIOReturnSuccess )
        {
                FFFB_ERR_S( scope, "failed with error code %x (%s)", result, mach_error_string( result ) ) ;
                return false ;
        }
        return true ;
}


} // namespace apple


using timestamp_t = uti::u64_t ;
using device_id_t = uti::u32_t ;

template< typename T > using vector = uti::vector< T > ;


} // namespace fffb
