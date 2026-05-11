#pragma once

#include <vector>

namespace rhi
{
class Index_Free_List
{
public:
    Index_Free_List(uint32_t index_count, uint32_t stride = 1);

    [[nodiscard]] uint32_t acquire_index();
    void release_index(uint32_t index);

private:
    const uint32_t m_max_index;
    std::vector<uint32_t> m_indices;
};
}
