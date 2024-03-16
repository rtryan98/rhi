#include <catch2/catch_test_macros.hpp>

#include <rhi/common/array_vector.hpp>

struct Dummy
{
    bool foo;
};

TEST_CASE("Array_Vector<Dummy>: Acquire, single bucket", "[Array_Vector]")
{
    auto array_vector = rhi::Array_Vector<Dummy, 2>();
    auto dummy = array_vector.acquire();
    REQUIRE(dummy);
}

TEST_CASE("Array_Vector<Dummy>: Acquire 4 elements, generate two buckets", "[Array_Vector]")
{
    auto array_vector = rhi::Array_Vector<Dummy, 2>();

    for (auto i = 0; i < 4; ++i)
    {
        auto dummy = array_vector.acquire();
        REQUIRE(dummy);
    }
}

TEST_CASE("Array_Vector<Dummy>: Acquire and release element, single bucket", "[Array_Vector]")
{
    auto array_vector = rhi::Array_Vector<Dummy, 2>();
    auto dummy = array_vector.acquire();
    dummy->foo = true;
    array_vector.release(dummy);

    // Array_Vector<T, SIZE> should replace removed objects with their default initialized state.
    // The pointer should stay valid.
    REQUIRE(dummy->foo == false);
}

TEST_CASE("Array_Vector<Dummy>: Internal linked list - pointer stability", "[Array_Vector]")
{
    auto array_vector = rhi::Array_Vector<Dummy, 2>();
    auto dummy_1 = array_vector.acquire();
    array_vector.release(dummy_1);
    auto dummy_2 = array_vector.acquire();

    // Array_Vector<T, SIZE> should fill up recently removed elements
    REQUIRE(dummy_1 == dummy_2);
}

TEST_CASE("Array_Vector<Dummy>: Internal linked list - pointer stability, multiple buckets", "[Array_Vector]")
{
    auto array_vector = rhi::Array_Vector<Dummy, 2>();
    auto dummy_1 = array_vector.acquire();

    for (auto i = 0; i < 8; ++i)
    {
        auto dummy_to_ignore = array_vector.acquire();
    }

    dummy_1->foo = true;
    array_vector.release(dummy_1);

    // Array_Vector<T, SIZE> should replace removed objects with their default initialized state.
    // The pointer should stay valid.
    REQUIRE(dummy_1->foo == false);

    auto dummy_2 = array_vector.acquire();

    // Array_Vector<T, SIZE> should fill up recently removed elements
    REQUIRE(dummy_1 == dummy_2);
}

TEST_CASE("Array_Vector<Dummy>::Iterator:, single bucket, less than max elements", "[Array_Vector]")
{
    constexpr auto SIZE = 16;

    auto array_vector = rhi::Array_Vector<Dummy, SIZE * 4>();

    std::array<Dummy*, SIZE * 2> array_of_dummies = {};
    for (auto i = 0; i < SIZE * 2; ++i)
    {
        auto dummy = array_vector.acquire();
        dummy->foo = true;
        array_of_dummies[i] = dummy;
    }
    for (auto i = 0; i < SIZE; ++i)
    {
        array_vector.release(array_of_dummies[i]);
    }

    auto count = 0;
    for (auto& dummy : array_vector)
    {
        count += dummy.foo;
    }

    REQUIRE(count == SIZE);
}

TEST_CASE("Array_Vector<Dummy>::Iterator:, single bucket, max elements", "[Array_Vector]")
{
    constexpr auto SIZE = 16;

    auto array_vector = rhi::Array_Vector<Dummy, SIZE * 2>();

    std::array<Dummy*, SIZE * 2> array_of_dummies = {};
    for (auto i = 0; i < SIZE * 2; ++i)
    {
        auto dummy = array_vector.acquire();
        dummy->foo = true;
        array_of_dummies[i] = dummy;
    }
    for (auto i = 0; i < SIZE; ++i)
    {
        array_vector.release(array_of_dummies[i]);
    }

    auto count = 0;
    for (auto& dummy : array_vector)
    {
        count += dummy.foo;
    }

    REQUIRE(count == SIZE);
}

TEST_CASE("Array_Vector<Dummy>::Iterator:, multiple buckets, less than max elements", "[Array_Vector]")
{
    constexpr auto BUCKET_SIZE = 16;
    constexpr auto TOTAL_SIZE = BUCKET_SIZE * 2;
    constexpr auto ADDED_ELEMENT_COUNT = BUCKET_SIZE + BUCKET_SIZE / 2;
    constexpr auto REMOVED_ELEMENT_COUNT = BUCKET_SIZE;
    constexpr auto FINAL_COUNT = ADDED_ELEMENT_COUNT - REMOVED_ELEMENT_COUNT;

    auto array_vector = rhi::Array_Vector<Dummy, BUCKET_SIZE>();

    std::array<Dummy*, TOTAL_SIZE> array_of_dummies = {};
    for (auto i = 0; i < ADDED_ELEMENT_COUNT; ++i)
    {
        auto dummy = array_vector.acquire();
        dummy->foo = true;
        array_of_dummies[i] = dummy;
    }
    for (auto i = 0; i < REMOVED_ELEMENT_COUNT; ++i)
    {
        array_vector.release(array_of_dummies[i]);
    }

    auto count = 0;
    for (auto& dummy : array_vector)
    {
        count += dummy.foo;
    }

    REQUIRE(count == FINAL_COUNT);
}

TEST_CASE("Array_Vector<Dummy>::Iterator:, multiple buckets, max elements", "[Array_Vector]")
{
    constexpr auto BUCKET_SIZE = 16;
    constexpr auto TOTAL_SIZE = BUCKET_SIZE * 2;
    constexpr auto ADDED_ELEMENT_COUNT = TOTAL_SIZE;
    constexpr auto REMOVED_ELEMENT_COUNT = BUCKET_SIZE;
    constexpr auto FINAL_COUNT = ADDED_ELEMENT_COUNT - REMOVED_ELEMENT_COUNT;

    auto array_vector = rhi::Array_Vector<Dummy, BUCKET_SIZE>();

    std::array<Dummy*, TOTAL_SIZE> array_of_dummies = {};
    for (auto i = 0; i < ADDED_ELEMENT_COUNT; ++i)
    {
        auto dummy = array_vector.acquire();
        dummy->foo = true;
        array_of_dummies[i] = dummy;
    }
    for (auto i = 0; i < REMOVED_ELEMENT_COUNT; ++i)
    {
        array_vector.release(array_of_dummies[i]);
    }

    auto count = 0;
    for (auto& dummy : array_vector)
    {
        count += dummy.foo;
    }

    REQUIRE(count == FINAL_COUNT);
}
