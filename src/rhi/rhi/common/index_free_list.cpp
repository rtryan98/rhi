#include "rhi/common/index_free_list.hpp"

namespace rhi
{
Index_Free_List::Index_Free_List(uint32_t index_count, uint32_t stride)
    : m_max_index(stride* index_count)
{
    m_indices.reserve(index_count);
    for (auto i = static_cast<int32_t>(index_count) - 1; i >= 0; --i)
    {
        m_indices.push_back(static_cast<uint32_t>(i) * stride);
    }
}

uint32_t Index_Free_List::acquire_index()
{
    auto index = ~0u;
    index = m_indices.back();
    m_indices.pop_back();
    return index;
}

void Index_Free_List::release_index(uint32_t index)
{
    if (index < m_max_index)
    {
        m_indices.push_back(index);
    }
}
}
