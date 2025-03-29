#include <tmp/Typelist.hpp>
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

namespace TestCount {
    static_assert(count_v<empty> == 0);
    static_assert(count_v<first> == 1);
    static_assert(count_v<firstFour> == 4);
} // namespace TestCount

namespace TestAppend {
    static_assert(std::is_same_v<append_t<test1, empty>, first>);
    static_assert(std::is_same_v<append_t<test2, first>, firstTwo>);
    static_assert(std::is_same_v<append_t<test4, append_t<test3, firstTwo>>, firstFour>);
    static_assert(std::is_same_v<append_t<test1, first>, typelist<test1, test1>>);
    static_assert(std::is_same_v<append_t<firstTwo, secondTwo>, typelist<test3, test4, firstTwo>>);
} // namespace TestCount

namespace TestPrepend {
    static_assert(std::is_same_v<prepend_t<test1, empty>, first>);
    static_assert(std::is_same_v<prepend_t<test2, first>, typelist<test2, test1>>);
    static_assert(std::is_same_v<prepend_t<test4, prepend_t<test3, firstTwo>>, typelist<test4, test3, test1, test2>>);
    static_assert(std::is_same_v<prepend_t<test1, first>, typelist<test1, test1>>);   
}

namespace TestConcat {
    static_assert(std::is_same_v<concat_t<empty, empty>, empty>);
    static_assert(std::is_same_v<concat_t<first, empty>, first>);
    static_assert(std::is_same_v<concat_t<empty, first>, first>);
    static_assert(std::is_same_v<concat_t<first, second>, firstTwo>);
    static_assert(std::is_same_v<concat_t<second, first>, typelist<test2, test1>>);
    static_assert(std::is_same_v<concat_t<firstTwo, secondTwo>, firstFour>);
}

namespace TestRemoveFront {
    static_assert(std::is_same_v<remove_front_t<empty>, empty>);
    static_assert(std::is_same_v<remove_front_t<first>, empty>);
    static_assert(std::is_same_v<remove_front_t<firstTwo>, second>);
    static_assert(std::is_same_v<remove_front_t<remove_front_t<firstFour>>, secondTwo>);
}

namespace TestRemoveBack {
    static_assert(std::is_same_v<remove_back_t<empty>, empty>);
    static_assert(std::is_same_v<remove_back_t<first>, empty>);
    static_assert(std::is_same_v<remove_back_t<firstTwo>, first>);
    static_assert(std::is_same_v<remove_back_t<remove_back_t<firstFour>>, firstTwo>);
}

namespace TestRemoveAt {
    static_assert(std::is_same_v<remove_at_t<0, first>, empty>);
    static_assert(std::is_same_v<remove_at_t<0, firstTwo>, second>);
    static_assert(std::is_same_v<remove_at_t<1, firstTwo>, first>);
    static_assert(std::is_same_v<remove_at_t<2, firstFour>, typelist<test1, test2, test4>>);
}

namespace TestFrontBackAt {
    static_assert(std::is_same_v<front_t<first>, test1>);
    static_assert(std::is_same_v<front_t<all>, test1>);

    static_assert(std::is_same_v<back_t<first>, test1>);
    static_assert(std::is_same_v<back_t<all>, test4>);

    static_assert(std::is_same_v<at_t<0, first>, test1>);
    static_assert(std::is_same_v<at_t<1, firstTwo>, test2>);
    static_assert(std::is_same_v<at_t<2, firstFour>, test3>);
} // namespace TestCount
