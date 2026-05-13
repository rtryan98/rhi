#pragma once

#include <cstdint>

namespace rhi
{
enum class Image_Format;
enum class Index_Type;

struct Acceleration_Structure;
struct Buffer;

enum class Acceleration_Structure_Type
{
    Top_Level,
    Bottom_Level
};

enum class Acceleration_Structure_Geometry_Type
{
    Triangles,
    AABBs
};

enum class Acceleration_Structure_Flags
{
    None = 0x0,
    Allow_Update = 0x1,
    Allow_Compaction = 0x2,
    Fast_Trace = 0x4,
    Fast_Build = 0x8,
};

enum class Acceleration_Structure_Geometry_Flags
{
    None = 0x0,
    Opaque = 0x1,
    No_Duplicate_Any_Hit_Invocation = 0x2,
};

enum class Acceleration_Structure_Instance_Flags : uint32_t
{
    None = 0x0,
    Triangle_Cull_Disable = 0x1,
    Triangle_Front_CCW = 0x2,
    Force_Opaque = 0x4,
    Force_Non_Opaque = 0x8
};

struct alignas(16) Acceleration_Structure_Instance
{
    float transform[3][4];
    uint32_t instance_id : 24;
    uint32_t instance_mask : 8;
    uint32_t instance_sbt_record_offset : 24;
    uint32_t flags : 8;
    uint64_t acceleration_structure_gpu_address;
};

struct Acceleration_Structure_Geometry_Data
{
    Acceleration_Structure_Geometry_Type type;
    Acceleration_Structure_Geometry_Flags flags;
    union
    {
        struct
        {
            uint64_t transform_gpu_address;
            uint64_t vertex_gpu_address;
            uint64_t index_gpu_address;
            alignas(uint64_t) Image_Format vertex_format;
            alignas(uint64_t) uint32_t vertex_count;
            alignas(uint64_t) uint32_t vertex_stride;
            alignas(uint64_t) uint32_t index_count;
            alignas(uint64_t) Index_Type index_type;
        } triangles;
        struct
        {
            uint64_t aabb_count;
            uint64_t aabb_gpu_address;
            alignas(uint64_t) uint32_t aabb_stride;
        } aabbs;
    } geometry;
};

struct Acceleration_Structure_Instance_Data
{
    bool array_of_pointers;
    uint64_t instance_gpu_address;
};

struct Acceleration_Structure_Build_Geometry_Info
{
    Acceleration_Structure_Type type;
    Acceleration_Structure_Flags flags;
    uint32_t geometry_or_instance_count;
    Acceleration_Structure* src; // if not null -> update
    Acceleration_Structure* dst;
    union {
        Acceleration_Structure_Geometry_Data* geometry;
        Acceleration_Structure_Instance_Data instances;
    };
};

struct Acceleration_Structure_Build_Sizes
{
    uint64_t acceleration_structure_size;
    uint64_t acceleration_structure_scratch_build_size;
    uint64_t acceleration_structure_scratch_update_size;
};

struct Acceleration_Structure_Create_Info
{
    Buffer* buffer;
    uint64_t offset; // Must be 256 byte aligned
    uint64_t size;
    Acceleration_Structure_Type type;
};

struct Acceleration_Structure
{
    Buffer* buffer;
    uint64_t address;
    Acceleration_Structure_Type type;
    uint32_t bindless_index;
};
}
