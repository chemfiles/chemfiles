// Chemfiles, a modern library for chemistry file reading and writing
// Copyright (C) Guillaume Fraux and contributors -- BSD license
#include <catch.hpp>

#include "chemfiles/capi/shared_allocator.hpp"
using namespace chemfiles;

struct Dummy {
    std::vector<int> data;
};

TEST_CASE("Shared allocator") {
    SECTION("Basic usage") {
        auto ptr = shared_allocator::make_shared<Dummy>();
        ptr->data.insert(std::end(ptr->data), {1, 2, 3, 4, 5, 6, 7});

        auto shared_1 = shared_allocator::shared_ptr(ptr, &ptr->data[2]);
        auto shared_2 = shared_allocator::shared_ptr(ptr, &ptr->data[3]);

        CHECK(*shared_1 == 3);
        CHECK(*shared_2 == 4);

        shared_allocator::free(ptr);

        // We can still safely access shared pointers after ptr has been freed
        *shared_1 += 2;
        *shared_2 += 10;

        CHECK(*shared_1 == 5);
        CHECK(*shared_2 == 14);

        shared_allocator::free(shared_1);
        shared_allocator::free(shared_2);
        // Valgrind tests make sure all memory is released here
    }

    SECTION("Allocation of array type") {
        auto ptr = shared_allocator::make_shared<Dummy[]>(4);

        // just checking that we can access all items in the array
        ptr[0].data.push_back(4);
        ptr[2].data.push_back(2);
        ptr[3].data.push_back(65);

        auto shared = shared_allocator::shared_ptr(ptr, &ptr[3].data[0]);

        CHECK(*shared == 65);
        shared_allocator::free(ptr);

        *shared -= 20;
        CHECK(*shared == 45);

        shared_allocator::free(shared);
    }

    SECTION("Internal re-allocation") {
        auto ptr = shared_allocator::make_shared<Dummy>();
        ptr->data.insert(std::end(ptr->data), {1, 2, 3});

        auto old_address = reinterpret_cast<intptr_t>(ptr->data.data());
        auto shared = shared_allocator::shared_ptr(ptr, &ptr->data[1]);
        CHECK(*shared == 2);
        *shared += 2;
        CHECK(*shared == 4);

        shared_allocator::free(shared);

        ptr->data.resize(1024, 0);
        auto new_address = reinterpret_cast<intptr_t>(ptr->data.data());
        CHECK(old_address != new_address);

        shared = shared_allocator::shared_ptr(ptr, &ptr->data[1]);
        CHECK(*shared == 4);
        shared_allocator::free(shared);
        shared_allocator::free(ptr);
    }

    SECTION("Multiple shared pointers") {
        auto ptr = shared_allocator::make_shared<Dummy>();
        ptr->data.insert(std::end(ptr->data), {1, 2, 3});

        auto shared_1 = shared_allocator::shared_ptr(ptr, &ptr->data[1]);
        auto shared_2 = shared_allocator::shared_ptr(ptr, &ptr->data[1]);
        auto shared_3 = shared_allocator::shared_ptr(ptr, &ptr->data[1]);
        CHECK(*shared_1 == 2);
        *shared_1 += 2;
        CHECK(*shared_1 == 4);

        shared_allocator::free(shared_1);

        CHECK(*shared_2 == 4);
        *shared_2 += 2;
        CHECK(*shared_2 == 6);
        CHECK(*shared_3 == 6);

        shared_allocator::free(shared_2);
        shared_allocator::free(shared_3);
        shared_allocator::free(ptr);
    }

    SECTION("Errors") {
        CHECK_THROWS_AS(shared_allocator::free(reinterpret_cast<void*>(0x1)), MemoryError);
    }
}
