// SPDX-License-Identifier: Unlicense

#include <portable-endianness.h>

#include <cstring>
#include <iostream>
#include <limits>
#include <random>

#define TEST(ENDIAN, WIDTH)                                                        \
    std::cerr << "Testing load->store " #ENDIAN " handling on uint" #WIDTH "_t\n"; \
    test_load_store(load##WIDTH##_from_##ENDIAN, store##WIDTH##_to_##ENDIAN);    \
    std::cerr << "Testing store->load " #ENDIAN " handling on uint" #WIDTH "_t\n"; \
    test_store_load(load##WIDTH##_from_##ENDIAN, store##WIDTH##_to_##ENDIAN);

template <typename LoadCaller, typename StoreCaller>
void test_load_store(LoadCaller load, StoreCaller store)
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    using TestType = decltype(load(nullptr));
    std::uniform_int_distribution<TestType> dis{std::numeric_limits<TestType>::min(),
                                                std::numeric_limits<TestType>::max()};

    for (int r = 0; r < 100000; ++r) {
        const auto rand_val = dis(gen);

        unsigned char input_arr[sizeof(TestType)];
        std::memcpy(input_arr, &rand_val, sizeof(TestType));

        const auto loaded_val = load(input_arr);
        unsigned char output_arr[sizeof(TestType)];
        store(loaded_val, output_arr);

        if (std::memcmp(input_arr, output_arr, sizeof(TestType)) != 0) {
            std::cerr << "Detected broken roundtrip\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

template <typename LoadCaller, typename StoreCaller>
void test_store_load(LoadCaller load, StoreCaller store)
{
    std::random_device rd;
    std::mt19937 gen{rd()};

    using TestType = decltype(load(nullptr));
    std::uniform_int_distribution<TestType> dis{std::numeric_limits<TestType>::min(),
                                                std::numeric_limits<TestType>::max()};

    for (int r = 0; r < 100000; ++r) {
        const auto input_val = dis(gen);

        unsigned char loaded_arr[sizeof(TestType)];
        store(input_val, loaded_arr);

        const auto output_val = load(loaded_arr);

        if (std::memcmp(&input_val, &output_val, sizeof(TestType)) != 0) {
            std::cerr << "Detected broken roundtrip\n";
            std::exit(EXIT_FAILURE);
        }
    }
}

int main()
{
    TEST(LE, 16)
    TEST(LE, 32)
    TEST(LE, 64)

    TEST(BE, 16)
    TEST(BE, 32)
    TEST(BE, 64)

    return EXIT_SUCCESS;
}
