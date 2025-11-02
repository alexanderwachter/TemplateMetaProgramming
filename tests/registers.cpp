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

namespace Register {
    using reg32 = reg<1U, 42, 32U, uint8_t>;
    using reg7 = reg<1U, 42, 7U, uint8_t>;

    static_assert(is_reg_v<reg32>);
    static_assert(!is_reg_v<content<reg32>>);
    static_assert(address_v<reg32> == 1U);
    static_assert(std::is_same_v<decltype(address_v<reg32>), const uint8_t>);
    static_assert(reset_value_v<reg32> == 42U);
    static_assert(width_v<reg32> == 32U);
    static_assert(mask_v<reg7> == 0b01111111);
    static_assert(std::is_same_v<datatype_t<reg32>, std::uint32_t>);
    static_assert(std::is_same_v<datatype_t<reg7>, std::uint8_t>);
} // Register

namespace Content {
    using reg8 = reg<1U, 42, 8U, uint8_t>;
    using contentBit = content<reg8, 3, 1>;

    static_assert(is_content_v<contentBit>);
    static_assert(!is_content_v<reg8>);
    static_assert(std::is_same_v<base_register_t<contentBit>, reg8>);
    static_assert(std::is_same_v<datatype_t<contentBit>, bool>);
    static_assert(width_v<contentBit> == 1);
    static_assert(offset_v<contentBit> == 3);
    static_assert(mask_v<contentBit> == (1U << 3));
} // Content

namespace MultiregContent {
    using reg1 = reg<1U, 0, 8U, uint8_t>;
    using reg2 = reg<2U, 0, 8U, uint8_t>;
    using multireg = multireg_content<content_list<content<reg1>, content<reg2>>>;

    static_assert(is_multireg_content_v<multireg>);
    static_assert(!is_multireg_content_v<content<reg1>>);
    static_assert(width_v<multireg> == 16);
    static_assert(mask_v<multireg> == 0xffff);
    static_assert(std::is_same_v<base_content_t<multireg>, content_list<content<reg1>, content<reg2>>>);
    static_assert(std::is_same_v<datatype_t<multireg>, std::uint16_t>);
    static_assert(std::is_same_v<base_register_t<multireg>, register_list<reg1,reg2>>);
} // namespace MultiregContent


namespace Value {
    using reg1 = reg<1U, 0, 8U, uint8_t>;
    using reg2 = reg<2U, 0, 8U, uint8_t>;
    using content1 = content<reg1>;
    using content2 = content<reg2>;
    using multireg = multireg_content<content_list<content1, content2>>;
    using value1 = value<content1, 1>;
    using value2 = value<multireg, 0x1ff>;

    static_assert(is_value_v<value1>);
    static_assert(!is_value_v<content1>);
    static_assert(width_v<value1> == 8);
    static_assert(width_v<value2> == 16);
    static_assert(std::is_same_v<base_content_t<value1>, content1>);
    static_assert(std::is_same_v<base_content_t<value2>, multireg>);
} // namespace Value

namespace AddressList
{
    using reg1 = reg<1U, 0, 8U, uint8_t>;
    using reg2 = reg<2U, 0, 8U, uint8_t>;
    using reg3 = reg<3U, 0, 8U, uint8_t>;
    using reg4 = reg<4U, 0, 8U, uint8_t>;

    using content1 = content<reg1>;
    using content2 = content<reg2>;
    using content3 = content<reg3>;
    using content4 = content<reg4>;
    using multireg = multireg_content<content_list<content4, content3>>;

    using contents = content_list<content1, content3, multireg, content2>;

    using registers = address_array<contents>;
    static constexpr std::array<datatype_t<reg1>, count_v<contents>> addr_list = {{1, 2, 3, 4}};
    static_assert(std::equal(addr_list.begin(), addr_list.end(), registers::value.begin()));
    static_assert(std::is_same_v<std::decay_t<decltype(address_v<reg1>)>, std::decay_t<decltype(registers::value.front())>>);
} // namespace ExtracRegisters

namespace View {
    using reg1 = reg<1U, 0, 8U, uint8_t>;
    using reg2 = reg<2U, 0, 8U, uint8_t>;

    using content1 = content<reg1>;
    using content2 = content<reg2, 0, 4>;
    using content3 = content<reg2, 4, 2>;
    using multireg_content1 = multireg_content<content_list<content1, content3>>;
    using multireg_content2 = multireg_content<content_list<content2, content1>>;

    constexpr std::uint8_t data[] = {0xaa,0x6f};
    constexpr Registers<register_list<reg1, reg2>> view(data);
    static_assert(view.get<content1>() == 0xaa);
    static_assert(view.get<content2>() == 0xf);
    static_assert(view.get<content3>() == 2);
    static_assert(view.get<multireg_content1>() == 0x2aa);
    static_assert(view.get<multireg_content2>() == 0xaaf);

    using content_neg1 = content<reg2, 0, 4, no_attributes, width_to_int_t<4>>;
    using content_neg2 = content<reg1, 0, 8, no_attributes, width_to_int_t<8>>;
    using content_neg3 = content<reg2, 4, 4, no_attributes, width_to_int_t<4>>;
    using multireg_content_neg = multireg_content<content_list<content1, content3>, no_attributes, int16_t>;

    static_assert(view.get<content_neg1>() == -1);
    static_assert(view.get<content_neg2>() == -86);
    static_assert(view.get<content_neg3>() == 6);
    static_assert(view.get<multireg_content_neg>() == -342);
    
} // namespace View

namespace Set
{
    using reg1 = reg<1U, 0, 8U, uint8_t>;
    using reg2 = reg<2U, 0, 8U, uint8_t>;

    using registers = register_list<reg1, reg2>;

    using content1 = content<reg1>;
    using content2 = content<reg2, 0, 4>;
    using content3 = content<reg2, 4, 2>;
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
    using readonly_reg = reg<1U, 0, 8U, std::uint8_t, std::uint8_t, attributes<readonly>>;
    using writeonly_reg = reg<1U, 0, 8U, std::uint8_t, std::uint8_t, attributes<writeonly>>;
    using some_reg = reg<2U, 0, 8U, std::uint8_t>;
    using readonly_content = content<readonly_reg>;
    using some_content = content<some_reg>;
    using multireg_ro_from_reg = multireg_content<content_list<readonly_content, some_content>>;
    using multiregto_from_content = multireg_content<content_list<content<some_reg, 0, 1, attributes<readonly>>>>;
    using multireg_ro = multireg_content<content_list<content<some_reg>>, attributes<readonly>>;

    static_assert(is_readonly_v<readonly_reg>);
    static_assert(!is_writeonly_v<readonly_reg>);
    static_assert(!is_readonly_v<writeonly_reg>);
    static_assert(is_writeonly_v<writeonly_reg>);
    static_assert(is_reserved_v<reg<1U, 0, 8U, std::uint8_t, std::uint8_t, attributes<reserved>>>);
    static_assert(is_readonly_v<multireg_ro_from_reg>);
    static_assert(is_readonly_v<multiregto_from_content>);
    static_assert(is_readonly_v<multireg_ro>);
} // namespace Attributes
