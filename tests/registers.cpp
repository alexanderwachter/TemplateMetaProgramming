/*
 * Copyright (c) 2025 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <tmp/RegisterOperations.hpp>
#include <tmp/RegisterAttributes.hpp>
#include <tmp/Registers.hpp>

#include <type_traits>
#include <algorithm>
#include <array>

using namespace tmp;

template<std::uint8_t ADDRESS>
using address_t = address<std::uint8_t, ADDRESS>;

namespace Register {
    
    using regu32 = reg<address_t<1U>, reset_value<42>, datatype<std::uint32_t>>;
    using regu8 = reg<address_t<1U>, reset_value<42>, datatype<std::uint8_t>>;

    static_assert(is_reg_v<regu32>);
    static_assert(!is_reg_v<content<regu32>>);
    static_assert(get_address_v<regu32> == 1U);
    static_assert(std::is_same_v<decltype(get_address_v<regu32>), const uint8_t>);
    static_assert(get_reset_value_v<regu32> == 42U);
    static_assert(get_width_v<regu32> == 32U);
    static_assert(get_width_v<regu8> == 8U);
    static_assert(get_mask_v<regu32> == 0xffffffff);
    static_assert(get_mask_v<regu8> == 0xff);
    static_assert(std::is_same_v<get_datatype_t<regu32>, std::uint32_t>);
    static_assert(std::is_same_v<get_datatype_t<regu8>, std::uint8_t>);

    using reg8_t = reg8<address_t<1U>, reset_value<42U>, attributes<readonly>>;
    static_assert(is_reg_v<reg8_t>);
    static_assert(get_address_v<reg8_t> == 1U);
    static_assert(get_reset_value_v<reg8_t> == 42U);
    static_assert(get_width_v<reg8_t> == 8U);
    static_assert(get_mask_v<reg8_t> == 0xff);
    static_assert(std::is_same_v<get_datatype_t<reg8_t>, std::uint8_t>);
    static_assert(has_attribute_v<readonly, reg8_t>);

} // Register

namespace Content {
    using reg8 = reg<address_t<1U>, reset_value<42>, datatype<std::uint8_t>>;
    using contentBit = content<reg8, offset<3>, width<1>>;

    static_assert(is_content_v<contentBit>);
    static_assert(!is_content_v<reg8>);
    static_assert(std::is_same_v<get_register_t<contentBit>, reg8>);
    static_assert(std::is_same_v<get_datatype_t<contentBit>, bool>);
    static_assert(get_width_v<contentBit> == 1);
    static_assert(get_offset_v<contentBit> == 3);
    static_assert(get_mask_v<contentBit> == (1U << 3));
} // Content

namespace MultiregContent {
    using reg1 = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg2 = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;
    using multireg = multireg_content<content_list<content<reg1>, content<reg2>>>;

    static_assert(is_multireg_content_v<multireg>);
    static_assert(!is_multireg_content_v<content<reg1>>);
    static_assert(get_width_v<multireg> == 16);
    static_assert(get_mask_v<multireg> == 0xffff);
    static_assert(std::is_same_v<base_content_t<multireg>, content_list<content<reg1>, content<reg2>>>);
    static_assert(std::is_same_v<get_datatype_t<multireg>, std::uint16_t>);
    static_assert(std::is_same_v<get_register_t<multireg>, register_list<reg1,reg2>>);
} // namespace MultiregContent


namespace Value {
    using reg1 = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg2 = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;
    using content1 = content<reg1>;
    using content2 = content<reg2>;
    using multireg = multireg_content<content_list<content1, content2>>;
    using value1 = value<content1, 1>;
    using value2 = value<multireg, 0x1ff>;

    static_assert(is_value_v<value1>);
    static_assert(!is_value_v<content1>);
    static_assert(get_width_v<value1> == 8);
    static_assert(get_width_v<value2> == 16);
    static_assert(std::is_same_v<base_content_t<value1>, content1>);
    static_assert(std::is_same_v<base_content_t<value2>, multireg>);
} // namespace Value

namespace AddressList
{
    using reg1 = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg2 = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg3 = reg<address_t<3U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg4 = reg<address_t<4U>, reset_value<0>, datatype<std::uint8_t>>;

    using content1 = content<reg1>;
    using content2 = content<reg2>;
    using content3 = content<reg3>;
    using content4 = content<reg4>;
    using multireg = multireg_content<content_list<content4, content3>>;

    using contents = content_list<content1, content3, multireg, content2>;

    using registers = address_array<contents>;
    static constexpr std::array<get_datatype_t<reg1>, count_v<contents>> addr_list = {{1, 2, 3, 4}};
    static_assert(std::equal(addr_list.begin(), addr_list.end(), registers::value.begin()));
    static_assert(std::is_same_v<std::uint8_t, std::decay_t<decltype(registers::value.front())>>);
} // namespace ExtracRegisters

namespace View {
    using reg1 = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg2 = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;

    using content1 = content<reg1>;
    using content2 = content<reg2, offset<0>, width<4>>;
    using content3 = content<reg2, offset<4>, width<2>>;
    using multireg_content1 = multireg_content<content_list<content1, content3>>;
    using multireg_content2 = multireg_content<content_list<content2, content1>>;

    constexpr std::uint8_t data[] = {0xaa,0x6f};
    constexpr Registers<register_list<reg1, reg2>> view(data);
    static_assert(view.get<content1>() == 0xaa);
    static_assert(view.get<content2>() == 0xf);
    static_assert(view.get<content3>() == 2);
    static_assert(view.get<multireg_content1>() == 0x2aa);
    static_assert(view.get<multireg_content2>() == 0xaaf);

    using content_neg1 = content<reg2, offset<0>, width<4>, no_attributes, datatype<width_to_int_t<4>>>;
    using content_neg2 = content<reg1, offset<0>, width<8>, no_attributes, datatype<width_to_int_t<8>>>;
    using content_neg3 = content<reg2, offset<4>, width<4>, no_attributes, datatype<width_to_int_t<4>>>;
    using multireg_content_neg = multireg_content<content_list<content1, content3>, no_attributes, datatype<int16_t>>;

    static_assert(view.get<content_neg1>() == -1);
    static_assert(view.get<content_neg2>() == -86);
    static_assert(view.get<content_neg3>() == 6);
    static_assert(view.get<multireg_content_neg>() == -342);
    
} // namespace View

namespace Set
{
    using reg1 = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>>;
    using reg2 = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;

    using registers = register_list<reg1, reg2>;

    using content1 = content<reg1>;
    using content2 = content<reg2, offset<0>, width<4>>;
    using content3 = content<reg2, offset<4>, width<2>>;
    using multireg_content1 = multireg_content<content_list<content1, content3>>;
    using multireg_content2 = multireg_content<content_list<content2, content1>>;

    static constexpr std::array<std::uint8_t, 2> data = [](){
        std::array<std::uint8_t, 2> data {};
        set<content1, registers>(0xff, data.data());
        set<content3, registers>(2, data.data());
        return data;
    }();

    static_assert(data[0] == 0xff);
    static_assert(data[1] == (2 << 4));

    static constexpr std::array<std::uint8_t, 2> data_multireg = [](){
        std::array<std::uint8_t, 2> data {};
        set<multireg_content1, registers>(0x2aa, data.data());
        return data;
    }();

    static_assert(data_multireg[0] == 0xaa);
    static_assert(data_multireg[1] == 0x20);
} // namespace Set

namespace Attributes
{
    using readonly_reg = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>, attributes<readonly>>;
    using writeonly_reg = reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>, attributes<writeonly>>;
    using some_reg = reg<address_t<2U>, reset_value<0>, datatype<std::uint8_t>>;
    using readonly_content = content<readonly_reg>;
    using some_content = content<some_reg>;
    using multireg_ro_from_reg = multireg_content<content_list<readonly_content, some_content>>;
    using multiregto_from_content = multireg_content<content_list<content<some_reg, offset<0>, width<1>, attributes<readonly>>>>;
    using multireg_ro = multireg_content<content_list<content<some_reg>>, attributes<readonly>>;

    static_assert(is_readonly_v<readonly_reg>);
    static_assert(!is_writeonly_v<readonly_reg>);
    static_assert(!is_readonly_v<writeonly_reg>);
    static_assert(is_writeonly_v<writeonly_reg>);
    static_assert(is_reserved_v<reg<address_t<1U>, reset_value<0>, datatype<std::uint8_t>, attributes<reserved>>>);
    static_assert(is_readonly_v<multireg_ro_from_reg>);
    static_assert(is_readonly_v<multiregto_from_content>);
    static_assert(is_readonly_v<multireg_ro>);
} // namespace Attributes

namespace Group {
    template<concepts::address ADDRESS>
    using my_reg = reg<ADDRESS, reset_value<0U>, datatype<unsigned int>>;
    using reg1 = my_reg<address_t<1U>>;
    using reg2 = my_reg<address_t<2U>>;
    using reg4 = my_reg<address_t<4U>>;
    using reg5 = my_reg<address_t<5U>>;
    using reg7 = my_reg<address_t<7U>>;

    using all_reg = register_list<reg1, reg2, reg4, reg5, reg7>;
    using expected = typelist<register_list<reg1, reg2>, register_list<reg4, reg5>, register_list<reg7>>;

    static_assert(std::is_same_v<group_contiguous_address_t<all_reg>, expected>);
};
