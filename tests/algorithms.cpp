#include <tmp/Algorithms.hpp>
#include <type_traits>

using namespace tmp;

struct test_type {};

struct test1 : test_type {};
struct test2 : test_type {};
struct test3 : test_type {};
struct test4 : test_type {};

using empty = typelist<>;
using all = typelist<test1, test2, test3, test4>;
using first = typelist<test1>;
using second = typelist<test2>;
using firstTwo = typelist<test1, test2>;
using secondTwo = typelist<test3, test4>;
using firstFour = typelist<test1, test2, test3, test4>;

template<typename T>
struct is_test1 : std::is_same<T, test1> {};

namespace AllOf {
    static_assert(all_of_v<typelist<bool, char, int>, std::is_integral> == true);
    static_assert(all_of_v<typelist<float, char, int>, std::is_integral> == false);
    static_assert(all_of_v<typelist<bool, float, int>, std::is_integral> == false);
    static_assert(all_of_v<typelist<float, char, int>, std::is_integral> == false);
    static_assert(all_of_v<empty, std::is_integral> == true);
}

namespace AnyOf {
    static_assert(any_of_v<typelist<float, void, double>, std::is_integral> == false);
    static_assert(any_of_v<typelist<int, void, float>, std::is_integral> == true);
    static_assert(any_of_v<typelist<void, int, float>, std::is_integral> == true);
    static_assert(any_of_v<typelist<void, float, int>, std::is_integral> == true);
    static_assert(any_of_v<empty, std::is_integral> == false);
}

namespace NoneOf {
    static_assert(none_of_v<typelist<float, void, double>, std::is_integral> == true);
    static_assert(none_of_v<typelist<int, void, float>, std::is_integral> == false);
    static_assert(none_of_v<typelist<void, int, float>, std::is_integral> == false);
    static_assert(none_of_v<typelist<void, float, int>, std::is_integral> == false);
    static_assert(none_of_v<empty, std::is_integral> == true);
}

namespace HasA {
    static_assert(has_a_v<typelist<float, void, double>,char> == false);
    static_assert(has_a_v<typelist<int, void, float>, int> == true);
    static_assert(has_a_v<typelist<int, void, float>, void> == true);
    static_assert(has_a_v<typelist<int, void, float>, float> == true);
    static_assert(has_a_v<empty, void> == false);
}


namespace FindIf {
    static_assert(std::is_same_v<find_if_t<typelist<float, void, double>, std::is_integral>, nil_type>);
    static_assert(std::is_same_v<find_if_t<typelist<float, int, double>, std::is_integral>, int>);
    static_assert(std::is_same_v<find_if_t<typelist<float, void, int>, std::is_integral>, int>);
    static_assert(std::is_same_v<find_if_t<typelist<int, void, float>, std::is_integral>, int>);
    static_assert(std::is_same_v<find_if_t<typelist<void, char, int>, std::is_integral>, char>);
}

namespace Filter {
    static_assert(std::is_same_v<filter_t<typelist<>, std::is_integral>, empty>);
    static_assert(std::is_same_v<filter_t<typelist<float, void, double>, std::is_integral>, empty>);
    static_assert(std::is_same_v<filter_t<typelist<bool, void, char>, std::is_integral>, typelist<bool, char>>);
    static_assert(std::is_same_v<filter_t<typelist<bool, void, char, void>, std::is_integral>, typelist<bool, char>>);
}

namespace Filter {
    static_assert(count_if_v<typelist<>, std::is_integral> == 0U);
    static_assert(count_if_v<typelist<float, void, double>, std::is_integral> == 0U);
    static_assert(count_if_v<typelist<bool, void, char>, std::is_integral> == 2U);
    static_assert(count_if_v<typelist<void, int, void>, std::is_integral> == 1U);
}


namespace Transform {
    static_assert(std::is_same_v<transform_t<typelist<>, std::make_unsigned>, typelist<>>);
    static_assert(std::is_same_v<transform_t<typelist<char, int, long>, std::make_unsigned>, typelist<unsigned char, unsigned int, unsigned long>>);
}