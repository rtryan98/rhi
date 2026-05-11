#pragma once

namespace rhi
{
enum class Result
{
    Success,
    Wait_Timeout,
    Error_Wait_Failed,
    Error_Out_Of_Memory,
    Error_Invalid_Parameters,
    Error_Device_Lost,
    Error_No_Resource, // Create a view of a destroyed resource
    Error_Acceleration_Structure_Invalid_Alignment,
    Error_Unknown
};
}
