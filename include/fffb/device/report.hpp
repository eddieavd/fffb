///
///
///     fffb
///     device/report.hpp
///

#pragma once

#include <fffb/util/types.hpp>
#include <fffb/util/log.hpp>

#include <mach/mach_error.h>

#define FFFB_REPORT_MAX_LEN 8


namespace fffb
{


struct report
{
        uti::u8_t data [ FFFB_REPORT_MAX_LEN ] ;

        constexpr uti::u8_t operator[] ( uti::ssize_t const index ) const noexcept { return data[ index ] ; }
} ;


UTI_NODISCARD constexpr bool write_report ( apple::hid_device * device, report const & report )
{
        return apple::_try( IOHIDDeviceSetReport( device, kIOHIDReportTypeOutput, time( nullptr ), report.data, FFFB_REPORT_MAX_LEN ), "send_report" ) ;
}

UTI_NODISCARD constexpr report read_report ( apple::hid_device const * )
{
        FFFB_F_ERR_S( "read_report", "unimplemented" ) ;
        return {} ;
}

UTI_NODISCARD constexpr vector< report > read_reports ( apple::hid_device const * )
{
        FFFB_F_ERR_S( "read_reports", "unimplemented" ) ;
        return {} ;
}


} // namespace fffb
