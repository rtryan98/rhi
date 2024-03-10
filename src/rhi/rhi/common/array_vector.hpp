#pragma once

#include <array>
#include <memory>
#include <type_traits>
#include <vector>

namespace rhi
{
template<typename T, std::size_t SIZE>
requires (
    std::is_default_constructible_v<T> &&
    !std::is_final_v<T> &&
    !std::is_fundamental_v<T>
    )
class Array_Vector
{
public:
    struct Iterator;

    Array_Vector()
        : m_data()
        , m_current_head(nullptr)
    {
        m_data.push_back(std::make_unique<Count_Array>());
    }

    Array_Vector(const Array_Vector& other) = delete;
    Array_Vector& operator=(const Array_Vector& other) = delete;

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
            m_data.push_back(std::make_unique<Count_Array>());
        }
        auto& count_array = m_data[m_data.size() - 1];
        auto* element = &(count_array->array[count_array->element_count]);
        element->alive = true;
        count_array->element_count += 1;
        return element;
    }

    void release(T* element) noexcept
    {
        auto linked_list_element = static_cast<Linked_List_Element*>(element);
        *linked_list_element = {};
        linked_list_element->next = m_current_head;
        linked_list_element->alive = false;
        m_current_head = linked_list_element;
    }

    Iterator begin() noexcept
    {
        return Iterator(m_data);
    }

    Iterator end() noexcept
    {
        return Iterator(m_data, m_data.size() - 1, SIZE);
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
        bool alive;
    };
    struct Count_Array
    {
        std::array<Linked_List_Element, SIZE> array;
        std::size_t element_count;
    };
    std::vector<std::unique_ptr<Count_Array>> m_data;
    Linked_List_Element* m_current_head;

public:
    struct Iterator
    {
    public:
        Iterator(auto& data_ref)
            : m_data(data_ref)
            , m_current_bucket(0)
            , m_current_element(0)
        {}

        Iterator(auto& data_ref, auto bucket, auto element)
            : m_data(data_ref)
            , m_current_bucket(bucket)
            , m_current_element(element)
        {}

        using iterator_category = std::random_access_iterator_tag;
        using difference_type = void;
        using value_type = T;
        using pointer = T*;
        using reference = T&;

        reference operator*() const
        {
            return m_data[m_current_bucket]->array[m_current_element];
        }

        pointer operator->() const
        {
            return &m_data[m_current_bucket]->array[m_current_element];
        }

        Iterator& operator++()
        {
            next_element();
            return *this;
        }

        Iterator operator++(int)
        {
            Iterator temp = *this;
            next_element();
            return temp;
        }

        friend bool operator==(const Iterator& a, const Iterator& b)
        {
            return a.m_current_bucket == b.m_current_bucket && a.m_current_element == b.m_current_element;
        }

        friend bool operator!=(const Iterator& a, const Iterator& b)
        {
            return !(a == b);
        }

    private:
        void next_element() noexcept
        {
            auto next_element = m_current_element;
            auto next_bucket = m_current_bucket;
            bool is_element_alive = false;

            while (next_bucket != m_data.size() && !is_element_alive)
            {
                // Check the next element
                next_element += 1;

                if (next_element == SIZE)
                {
                    if (next_bucket < m_data.size() - 1)
                    {
                        next_bucket += 1;
                        next_element = 0;
                    }
                    else
                    {
                        break;
                    }
                }

                is_element_alive = m_data[next_bucket]->array[next_element].alive;
            };

            m_current_element = next_element;
            m_current_bucket = next_bucket;
        }

    private:
        std::size_t m_current_bucket;
        std::size_t m_current_element;
        std::vector<std::unique_ptr<Count_Array>>& m_data;
    };
};
}
