#pragma once

#include "rhi/resource.hpp"
#include "rhi/common/index_free_list.hpp"

#include <type_traits>
#include <plf_colony.h>
#include <functional>

namespace rhi
{
template<typename T>
concept Buffer_Concept = std::is_base_of_v<Buffer, T>;

template<typename T>
concept Buffer_View_Concept = std::is_base_of_v<Buffer_View, T>;

template<typename T>
concept Image_Concept = std::is_base_of_v<Image, T>;

template<typename T>
concept Image_View_Concept = std::is_base_of_v<Image_View, T>;

template<typename T>
concept Sampler_Concept = std::is_base_of_v<Sampler, T>;

template<typename T>
concept Acceleration_Structure_Concept = std::is_base_of_v<Acceleration_Structure, T>;

template<
    Buffer_Concept Buffer_Type,
    Buffer_View_Concept Buffer_View_Type,
    Image_Concept Image_Type,
    Image_View_Concept Image_View_Type,
    Sampler_Concept Sampler_Type,
    Acceleration_Structure_Concept Acceleration_Structure_Type>
class Resource_Pool
{
public:
    struct Deleters
    {
        std::function<void(Buffer_Type*)> buffer_delete_function;
        std::function<void(Buffer_View_Type*)> buffer_view_delete_function;
        std::function<void(Image_Type*)> image_delete_function;
        std::function<void(Image_View_Type*)> image_view_delete_function;
        std::function<void(Sampler_Type*)> sampler_delete_function;
        std::function<void(Acceleration_Structure_Type*)> acceleration_structure_delete_function;
    };

public:
    Resource_Pool(
        uint32_t max_resource_index,
        uint32_t max_sampler_index,
        uint32_t resource_index_stride,
        Deleters&& deleters)
        : m_deleters(std::move(deleters))
        , m_resource_stride(resource_index_stride)
        , m_resource_indices(max_resource_index, resource_index_stride)
        , m_sampler_indices(max_sampler_index)
    {}

    ~Resource_Pool()
    {
        for (auto& buffer_view : m_buffer_views)
        {
            m_deleters.buffer_view_delete_function(&buffer_view);
        }
        for (auto& buffer : m_buffers)
        {
            m_deleters.buffer_delete_function(&buffer);
        }
        for (auto& image_view : m_image_views)
        {
            m_deleters.image_view_delete_function(&image_view);
        }
        for (auto& image : m_images)
        {
            m_deleters.image_delete_function(&image);
        }
        for (auto& sampler : m_samplers)
        {
            m_deleters.sampler_delete_function(&sampler);
        }
        for (auto& acceleration_structure : m_acceleration_structures)
        {
            m_deleters.acceleration_structure_delete_function(&acceleration_structure);
        }
    }

    [[nodiscard]] Buffer_Type* acquire_buffer(
        const Buffer_Create_Info& buffer_create_info,
        uint32_t bindless_resource_index = ~0u)
    {
        auto buffer = &*m_buffers.emplace();
        buffer->size = buffer_create_info.size;
        buffer->heap_type = buffer_create_info.heap;
        buffer->buffer_view = &*m_buffer_views.emplace();
        buffer->buffer_view->bindless_index = maybe_acquire_resource_index(bindless_resource_index);
        buffer->buffer_view->size = buffer->size;
        buffer->buffer_view->offset = 0;
        buffer->buffer_view->buffer = buffer;
        buffer->buffer_view->next_buffer_view = nullptr;
        buffer->buffer_view_linked_list_head = buffer->buffer_view;

        return buffer;
    }

    [[nodiscard]] Buffer_View_Type* acquire_buffer_view(
        Buffer* buffer,
        const Buffer_View_Create_Info& buffer_view_create_info,
        uint32_t bindless_resource_index = ~0u)
    {
        auto buffer_view = &*m_buffer_views.emplace();
        buffer_view->bindless_index = maybe_acquire_resource_index(bindless_resource_index);
        buffer_view->size = buffer_view_create_info.size;
        buffer_view->offset = buffer_view_create_info.offset;
        buffer_view->buffer = buffer;
        buffer_view->next_buffer_view = buffer->buffer_view_linked_list_head;
        buffer->buffer_view_linked_list_head = buffer_view;

        return buffer_view;
    }

    void release_buffer(Buffer* base_buffer)
    {
        m_resource_indices.release_index(base_buffer->buffer_view->bindless_index);
        auto next_buffer_view = static_cast<Buffer_View_Type*>(base_buffer->buffer_view_linked_list_head);
        while (next_buffer_view != nullptr)
        {
            auto current_buffer_view = next_buffer_view;
            m_resource_indices.release_index(current_buffer_view->bindless_index);
            m_deleters.buffer_view_delete_function(current_buffer_view);
            next_buffer_view = static_cast<Buffer_View_Type*>(current_buffer_view->next_buffer_view);
            m_buffer_views.erase(m_buffer_views.get_iterator(current_buffer_view));
        }
        auto derived_buffer = static_cast<Buffer_Type*>(base_buffer);
        m_deleters.buffer_delete_function(derived_buffer);
        m_buffers.erase(m_buffers.get_iterator(derived_buffer));
    }

    [[nodiscard]] Image_Type* acquire_image(
        const Image_Create_Info& image_create_info,
        uint32_t bindless_resource_index = ~0u)
    {
        auto image = &*m_images.emplace();
        image->format = image_create_info.format;
        image->width = image_create_info.width;
        image->height = image_create_info.height;
        image->depth = image_create_info.depth;
        image->array_size = image_create_info.array_size;
        image->mip_levels = image_create_info.mip_levels;
        image->usage = image_create_info.usage;
        image->primary_view_type = image_create_info.primary_view_type;
        image->image_view = &*m_image_views.emplace();
        image->image_view->bindless_index = maybe_acquire_resource_index(bindless_resource_index);
        image->image_view->image = image;
        image->image_view_linked_list_head = image->image_view;

        // TODO: Change this?
        // ANSWER: Yes, change this. Remove descriptor type entirely and create RTV/DSV descriptors
        //         instead when using images as attachments.
        bool is_rtv = bool(image->usage & Image_Usage::Color_Attachment);
        bool is_dsv = bool(image->usage & Image_Usage::Depth_Stencil_Attachment);

        image->image_view->descriptor_type = is_rtv
            ? Descriptor_Type::Color_Attachment
            : is_dsv
                ? Descriptor_Type::Depth_Stencil_Attachment
                : Descriptor_Type::Resource;

        return image;
    }

    [[nodiscard]] Image_View_Type* acquire_image_view(
        Image* image,
        const Image_View_Create_Info& image_view_create_info,
        uint32_t bindless_resource_index = ~0u)
    {
        auto image_view = &*m_image_views.emplace();

        image_view->image = image;
        image_view->bindless_index = maybe_acquire_resource_index(bindless_resource_index);
        image_view->next_image_view = image->image_view_linked_list_head;
        image->image_view_linked_list_head = image_view;

        return image_view;
    }

    void release_image(Image* base_image)
    {
        auto next_image_view = static_cast<Image_View_Type*>(base_image->image_view_linked_list_head);
        while (next_image_view != nullptr)
        {
            auto current_image_view = next_image_view;
            m_resource_indices.release_index(current_image_view->bindless_index);
            m_deleters.image_view_delete_function(current_image_view);
            next_image_view = static_cast<Image_View_Type*>(current_image_view->next_image_view);
            m_image_views.erase(m_image_views.get_iterator(current_image_view));
        }
        auto derived_image = static_cast<Image_Type*>(base_image);
        m_deleters.image_delete_function(derived_image);
        m_images.erase(m_images.get_iterator(derived_image));
    }

    [[nodiscard]] Image_Type* acquire_proxy_image()
    {
        auto image = &*m_images.emplace();
        image->image_view = &*m_image_views.emplace();
        image->image_view->image = image;
        image->image_view_linked_list_head = image->image_view;

        return image;
    }

    void release_proxy_image(Image* base_image)
    {
        m_image_views.erase(m_image_views.get_iterator(static_cast<Image_View_Type*>(base_image->image_view)));
        m_images.erase(m_images.get_iterator(static_cast<Image_Type*>(base_image)));
    }

    [[nodiscard]] Sampler_Type* acquire_sampler(
        uint32_t bindless_resource_index = ~0u)
    {
        auto sampler = &*m_samplers.emplace();

        sampler->bindless_index = maybe_acquire_sampler_index(bindless_resource_index);

        return sampler;
    }

    void release_sampler(Sampler* base_sampler)
    {
        m_sampler_indices.release_index(base_sampler->bindless_index);
        auto derived_sampler = static_cast<Sampler_Type*>(base_sampler);
        m_samplers.erase(m_samplers.get_iterator(derived_sampler));
    }

    [[nodiscard]] Acceleration_Structure_Type* acquire_acceleration_structure(
        uint32_t bindless_resource_index = ~0u)
    {
        auto acceleration_structure = &*m_acceleration_structures.emplace();

        acceleration_structure->bindless_index = maybe_acquire_resource_index(bindless_resource_index);

        return acceleration_structure;
    }

    void release_acceleration_structure(Acceleration_Structure* base_acceleration_structure)
    {
        m_resource_indices.release_index(base_acceleration_structure->bindless_index);
        auto derived_acceleration_structure = static_cast<Acceleration_Structure_Type*>(base_acceleration_structure);
        m_acceleration_structures.erase(m_acceleration_structures.get_iterator(derived_acceleration_structure));
    }

private:
    uint32_t maybe_acquire_resource_index(
        uint32_t bindless_resource_index)
    {
        return (bindless_resource_index != NO_RESOURCE_INDEX)
            ? (bindless_resource_index * m_resource_stride)
            : m_resource_indices.acquire_index();
    }

    uint32_t maybe_acquire_sampler_index(
        uint32_t bindless_resource_index)
    {
        return (bindless_resource_index != NO_RESOURCE_INDEX)
            ? (bindless_resource_index)
            : m_sampler_indices.acquire_index();
    }

private:
    uint32_t m_resource_stride;
    Index_Free_List m_resource_indices;
    Index_Free_List m_sampler_indices;
    Deleters m_deleters;

    plf::colony<Buffer_Type> m_buffers;
    plf::colony<Buffer_View_Type> m_buffer_views;
    plf::colony<Image_Type> m_images;
    plf::colony<Image_View_Type> m_image_views;
    plf::colony<Sampler_Type> m_samplers;
    plf::colony<Acceleration_Structure_Type> m_acceleration_structures;
};
}
