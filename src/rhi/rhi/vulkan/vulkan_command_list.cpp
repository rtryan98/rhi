#include "rhi/vulkan/vulkan_command_list.hpp"

#include "rhi/vulkan/vulkan_graphics_device.hpp"

namespace rhi::vulkan
{
Vulkan_Command_Pool::Vulkan_Command_Pool(
    Vulkan_Graphics_Device* device,
    const Command_Pool_Create_Info& create_info) noexcept
    : m_queue_type(create_info.queue_type)
    , m_device(device)
    , m_unused()
    , m_used()
{}

Vulkan_Command_Pool::~Vulkan_Command_Pool() noexcept
{
    reset();
    for (auto cmd_alloc : m_unused)
    {
        vkDestroyCommandPool(m_device->get_device(), cmd_alloc.pool, nullptr);
    }
    for (auto cmd_alloc : m_used)
    {
        vkDestroyCommandPool(m_device->get_device(), cmd_alloc.pool, nullptr);
    }
    m_command_lists.clear();
}

void Vulkan_Command_Pool::reset() noexcept
{
    m_unused.insert(m_unused.end(), m_used.begin(), m_used.end());
    m_used.clear();
    m_command_lists.clear();
}

Command_List* Vulkan_Command_Pool::acquire_command_list() noexcept
{
    auto cmd_alloc = Vulkan_Command_List_Allocator{};
    if (m_unused.empty())
    {
        auto queue_type = vkb::QueueType::graphics;
        switch (m_queue_type)
        {
        case Queue_Type::Graphics: queue_type = vkb::QueueType::graphics; break;
        case Queue_Type::Compute: queue_type = vkb::QueueType::compute; break;
        case Queue_Type::Copy: queue_type = vkb::QueueType::transfer; break;
        default: std::unreachable();
        }

        VkCommandPoolCreateInfo command_pool_create_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .pNext = nullptr,
            .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
            .queueFamilyIndex = m_device->get_queue_family_index(queue_type)
        };
        vkCreateCommandPool(m_device->get_device(), &command_pool_create_info, nullptr, &cmd_alloc.pool);

        VkCommandBufferAllocateInfo command_buffer_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = cmd_alloc.pool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1
        };
        vkAllocateCommandBuffers(m_device->get_device(), &command_buffer_allocate_info, &cmd_alloc.cmd);
    }
    else
    {
        cmd_alloc = m_unused.back();
        m_unused.pop_back();
    }
    m_used.push_back(cmd_alloc);

    vkResetCommandPool(m_device->get_device(), cmd_alloc.pool, 0);

    VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr
    };
    vkBeginCommandBuffer(cmd_alloc.cmd, &command_buffer_begin_info);

    if (m_queue_type == Queue_Type::Graphics || m_queue_type == Queue_Type::Compute)
    {
        const auto& resource_descriptor_heap = m_device->get_resource_descriptor_heap();
        VkBindHeapInfoEXT bind_resource_heap_info = {
            .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
            .pNext = nullptr,
            .heapRange = resource_descriptor_heap.heap_range,
            .reservedRangeOffset = resource_descriptor_heap.reserved_offset,
            .reservedRangeSize = resource_descriptor_heap.reserved_size
        };
        vkCmdBindResourceHeapEXT(cmd_alloc.cmd, &bind_resource_heap_info);

        const auto& sampler_descriptor_heap = m_device->get_sampler_descriptor_heap();
        VkBindHeapInfoEXT bind_sampler_heap_info = {
            .sType = VK_STRUCTURE_TYPE_BIND_HEAP_INFO_EXT,
            .pNext = nullptr,
            .heapRange = sampler_descriptor_heap.heap_range,
            .reservedRangeOffset = sampler_descriptor_heap.reserved_offset,
            .reservedRangeSize = sampler_descriptor_heap.reserved_size
        };
        vkCmdBindSamplerHeapEXT(cmd_alloc.cmd, &bind_sampler_heap_info);
    }
    return m_command_lists.emplace_back(std::make_unique<Vulkan_Command_List>(cmd_alloc.cmd, m_device)).get();
}
}
