//
//
//      fffb
//      hid/report.hxx
//

#pragma once

#include <fffb/util/log.hxx>
#include <fffb/util/types.hxx>

#define FFFB_REPORT_MAX_LEN 8


namespace fffb
{


struct report
{
        uti::u8_t data [ FFFB_REPORT_MAX_LEN ] ;

        constexpr uti::u8_t       & operator[] ( uti::ssize_t const index )       noexcept { return data[ index ] ; }
        constexpr uti::u8_t const & operator[] ( uti::ssize_t const index ) const noexcept { return data[ index ] ; }
} ;


[[ nodiscard ]] constexpr bool write_report ( apple::hid_device * device, report const & report ) noexcept
{
        return apple::_try(
                IOHIDDeviceSetReport( device, kIOHIDReportTypeOutput, time( nullptr ), report.data, FFFB_REPORT_MAX_LEN ),
                "send_report"
        ) ;
}

[[ nodiscard ]] constexpr report read_report ( apple::hid_device * ) noexcept
{
        FFFB_ERR_S( "read_report", "unimplemented" ) ;
        return {} ;
}


} // namespace fffb
