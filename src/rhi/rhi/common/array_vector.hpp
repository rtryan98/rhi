#pragma once

#include <array>
#include <memory>
#include <type_traits>
#include <vector>

namespace rhi
{
    template<typename T, std::size_t SIZE>
    requires std::is_default_constructible_v<T>
    class Array_Vector
    {
    public:
        Array_Vector()
        {
            m_data.push_back({});
        }

        [[nodiscard]] T* acquire() noexcept
        {
            if (m_current_head != nullptr)
            {
                auto prev_head = m_current_head;
                m_current_head = prev_head->next;
                return prev_head;
            }
            if (is_no_slot_available())
            {
                m_data.push_back({});
            }
            auto& array = m_data[m_data.size() - 1];
            auto element = &array[array.element_count];
            array.element_count += 1;
            return element;
        }

        void release(T* element) noexcept
        {
            auto linked_list_element = static_cast<Linked_List_Element*>(element);
            linked_list_element->next = m_current_head;
            m_current_head = linked_list_element;
        }

    private:
        bool is_no_slot_available()
        {
            return m_data[m_data.size() - 1]->element_count >= SIZE;
        }

    private:
        struct Linked_List_Element : public T
        {
            Linked_List_Element* next;
        };
        struct Count_Array
        {
            std::array<Linked_List_Element, SIZE> array;
            std::size_t element_count;
        };
        std::vector<std::unique_ptr<Count_Array>> m_data;
        Linked_List_Element* m_current_head;
    };
}