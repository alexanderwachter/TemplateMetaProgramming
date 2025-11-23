/*
 * Copyright (c) 2025 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "Typelist.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"
#include <array>
#include <span>
#include <limits>
#include <type_traits>
#include <concepts>
#include <utility>

namespace tmp {

template<std::unsigned_integral ADDRESS_TYPE, ADDRESS_TYPE VALUE>
struct address : std::integral_constant<ADDRESS_TYPE, VALUE> {};

template<typename T>
inline constexpr typename T::value_type address_v = T::value;

template<typename T>
using address_value_t = typename T::value_type;

template<typename>
struct is_address : std::false_type {};

template<std::unsigned_integral ADDRESS_TYPE, ADDRESS_TYPE VALUE>
struct is_address<address<ADDRESS_TYPE, VALUE>> : std::true_type {};

template<typename T>
inline constexpr bool is_address_v = is_address<T>::value;

namespace concepts
{
template<typename T>
concept address = is_address_v<T>;
} // namespace concepts


template<typename>
struct get_address;

template<typename T>
using get_address_t = typename get_address<T>::type;

template<typename T>
inline constexpr typename get_address_t<T>::value_type get_address_v = get_address_t<T>::value;

template<std::size_t WIDTH>
struct width : std::integral_constant<std::size_t, WIDTH> {};

template<typename T>
struct is_width : std::false_type {};

template<std::size_t V>
struct is_width<width<V>> : std::true_type {};

template<typename T>
inline constexpr bool is_width_v = is_width<T>::value;

namespace concepts
{
template<typename T>
concept width = is_width_v<T>;
} // namespace concepts

template<typename>
struct get_width;

template<typename T>
using get_width_t = typename get_width<T>::type;

template<typename T>
inline constexpr typename get_width_t<T>::value_type get_width_v = get_width_t<T>::value;

template<std::size_t WIDTH>
struct offset : std::integral_constant<std::size_t, WIDTH> {};

template<typename T>
struct is_offset : std::false_type {};

template<std::size_t V>
struct is_offset<offset<V>> : std::true_type {};

template<typename T>
inline constexpr bool is_offset_v = is_offset<T>::value;

namespace concepts
{
template<typename T>
concept offset = is_offset_v<T>;
} // namespace concepts

template<typename>
struct get_offset;

template<typename T>
using get_offset_t = typename get_offset<T>::type;

template<typename T>
inline constexpr typename get_offset_t<T>::value_type get_offset_v = get_offset_t<T>::value;

template<typename>
struct get_mask;

template<typename T>
inline constexpr typename get_mask<T>::value_type get_mask_v = get_mask<T>::value;

template<std::integral auto VALUE>
struct reset_value
{
    using type = reset_value;
    using value_type = decltype(VALUE);
    static constexpr value_type value = VALUE;
};

template<typename T>
inline constexpr typename T::value_type reset_value_v = T::value;

template<typename T>
using reset_value_t = typename T::value_type;

template<typename>
struct is_reset_value : std::false_type {};

template<std::integral auto VALUE>
struct is_reset_value<reset_value<VALUE>> : std::true_type {};

template<typename T>
inline constexpr bool is_reset_value_v = is_reset_value<T>::value;

namespace concepts
{
template<typename T>
concept reset_value = is_reset_value_v<T>;
} // namespace concepts

template<typename>
struct get_reset_value;

template<typename T>
using get_reset_value_t = typename get_reset_value<T>::type;

template<typename T>
inline constexpr typename get_reset_value_t<T>::value_type get_reset_value_v = get_reset_value_t<T>::value;

template<std::integral T>
struct datatype : std::type_identity<T> {};

template<typename T>
struct is_datatype : std::false_type {};

template<std::integral T>
struct is_datatype<datatype<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_datatype_v = is_datatype<T>::value;

namespace concepts
{
template<typename T>
concept datatype = is_datatype_v<T>;
} // namespace concepts

template<typename>
struct get_datatype;

template<typename T>
using get_datatype_t = typename get_datatype<T>::type;

template<typename>
struct get_register;

template<typename T>
using get_register_t = typename get_register<T>::type;

template<typename>
struct base_content;

template<typename T>
using base_content_t = typename base_content<T>::type;

template<typename T>
struct is_attribute : std::false_type {};

template<typename T>
inline constexpr bool is_attribute_v = is_attribute<T>::value;

namespace concepts
{
template<typename T>
concept attribute = is_attribute_v<T>;

template<typename T>
concept attribute_list = is_typelist_v<T> && all_of_v<T, is_attribute>;
} // namespace concepts

template<concepts::attribute, typename>
struct has_attribute : std::false_type {};

template<concepts::attribute ATTRIBUTE, typename T>
inline constexpr bool has_attribute_v = has_attribute<ATTRIBUTE, T>::value;

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, concepts::attribute_list ATTRIBUTES = typelist<>>
struct reg {
    static_assert(RESET_VALUE::value <= std::numeric_limits<DATATYPE>::max(), "Reset Value does not fit into the specified datatype");
    static_assert(RESET_VALUE::value >= 0);
};

template<typename>
struct is_reg : std::false_type {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct is_reg<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : std::true_type {};

template<typename T>
inline constexpr bool is_reg_v = is_reg<T>::value;

namespace concepts {
template<typename T>
concept reg = is_reg_v<T>;
template<typename T>
concept reg_list = is_typelist_v<T> && any_of_v<T, is_reg>;
} // namespace concepts

template<concepts::reg... REGISTERs>
using register_list = typelist<REGISTERs...>;

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct get_address<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : std::type_identity<ADDRESS> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct get_width<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : std::type_identity<width<sizeof(typename DATATYPE::type) * 8U>> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct get_mask<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : std::integral_constant<typename DATATYPE::type, std::numeric_limits<typename DATATYPE::type>::max()> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct get_datatype<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : DATATYPE {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct get_reset_value<reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : std::type_identity<reset_value<static_cast<typename DATATYPE::type>(RESET_VALUE::value)>> {};

template<concepts::attribute ATTRIBUTE, concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::datatype DATATYPE, typename ATTRIBUTES>
struct has_attribute<ATTRIBUTE, reg<ADDRESS, RESET_VALUE, DATATYPE, ATTRIBUTES>> : has_a<ATTRIBUTES, ATTRIBUTE> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES = typelist<>>
struct reg8 {
    static_assert(RESET_VALUE::value <= std::numeric_limits<std::uint8_t>::max());
    static_assert(RESET_VALUE::value >= 0);
};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct get_address<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : ADDRESS {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct get_width<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : std::type_identity<width<8U>> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct get_mask<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : std::integral_constant<std::uint8_t, 0xff> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct get_datatype<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : datatype<std::uint8_t> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct get_reset_value<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : std::type_identity<reset_value<static_cast<uint8_t>(RESET_VALUE::value)>>{};

template<concepts::attribute ATTRIBUTE, concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct has_attribute<ATTRIBUTE, reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : has_a<ATTRIBUTES, ATTRIBUTE> {};

template<concepts::address ADDRESS, concepts::reset_value RESET_VALUE, concepts::typelist ATTRIBUTES>
struct is_reg<reg8<ADDRESS, RESET_VALUE, ATTRIBUTES>> : std::true_type {};


template<concepts::reg REGISTER, concepts::offset OFFSET = offset<0>, concepts::width WIDTH = get_width_t<REGISTER>, concepts::attribute_list ATTRIBUTES = typelist<>, typename DATATYPE = datatype<width_to_uint_t<WIDTH::value>>>
struct content {
    static_assert(WIDTH::value + OFFSET::value < sizeof(DATATYPE) * 8U, "Content does not fit into datatype");
    static_assert(sizeof(DATATYPE) <= sizeof(get_datatype_t<REGISTER>), "Datatype does not fit into the register");
};

template<typename>
struct is_content : std::false_type {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE >
struct is_content<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_content_v = is_content<T>::value;

template<typename T>
using is_content_t = typename is_content<T>::type;

namespace concepts {
template<typename T>
concept content = is_content_v<T>;
} // namespace concepts

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_register<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::type_identity<REGISTER> {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_address<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : get_address_t<REGISTER> {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_width<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> :  std::type_identity<WIDTH> {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_offset<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::type_identity<OFFSET> {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_mask<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::integral_constant<get_datatype_t<REGISTER>, (((1U << WIDTH::value) - 1U) << OFFSET::value)> {};

template<concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_datatype<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : DATATYPE {};

template<concepts::attribute ATTRIBUTE, concepts::reg REGISTER, concepts::offset OFFSET, concepts::width WIDTH, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct has_attribute<ATTRIBUTE, content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> :
    std::bool_constant<has_a_v<ATTRIBUTES, ATTRIBUTE> || has_attribute_v<ATTRIBUTE, REGISTER>> {};

template<concepts::attribute ATTRIBUTE, concepts::content... CONTENTs>
struct has_attribute<ATTRIBUTE, typelist<CONTENTs...>> : std::bool_constant<(has_attribute_v<ATTRIBUTE, CONTENTs> || ...)> {};

template<concepts::typelist T>
struct is_content_list : std::bool_constant<is_typelist_v<T> && all_of_v<T, is_content>> {};

template<typename T>
inline constexpr bool is_content_list_v = is_content_list<T>::value;

namespace concepts {
template<typename T>
concept content_list = is_content_list_v<T>;
} // namespace concepts

template<concepts::content... CONTENTs>
struct get_width<typelist<CONTENTs...>> : std::type_identity<width<(get_width_v<CONTENTs> + ...)>> {};

template<concepts::content_list CONTENT_LIST, concepts::attribute_list ATTRIBUTES = typelist<>, concepts::datatype DATATYPE = datatype<width_to_uint_t<get_width_v<CONTENT_LIST>>>>
struct multireg_content {};

template<typename>
struct is_multireg_content : std::false_type {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct is_multireg_content<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_multireg_content_v = is_multireg_content<T>::value;

namespace concepts {
template<typename T>
concept multireg_content = is_multireg_content_v<T>;
template<typename T>
concept value_content = is_content_v<T> || is_multireg_content_v<T>;
} // namespace concepts

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_width<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : width<get_width_v<CONTENT_LIST>> {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_mask<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::integral_constant<typename DATATYPE::type, ((1U << get_width_v<CONTENT_LIST>) - 1U)> {};

template<concepts::content... CONTENTs>
struct get_register<typelist<CONTENTs...>> {
    using type = transform_t<typelist<CONTENTs...>, get_register>;
};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_register<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> {
    using type = get_register_t<CONTENT_LIST>;
};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct base_content<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::type_identity<CONTENT_LIST> {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct get_datatype<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : DATATYPE {};

template<concepts::attribute ATTRIBUTE, concepts::content_list CONTENT_LIST, typename ATTRIBUTES, concepts::datatype DATATYPE>
struct has_attribute<ATTRIBUTE, multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> :
    std::bool_constant<has_a_v<ATTRIBUTES, ATTRIBUTE> || has_attribute_v<ATTRIBUTE, CONTENT_LIST>> {};

template<concepts::value_content CONTENT, get_datatype_t<CONTENT> VALUE>
struct value {};

template<typename>
struct is_value : std::false_type {};

template<concepts::value_content CONTENT, auto VALUE>
struct is_value<value<CONTENT, VALUE>> : std::true_type {};

template<typename T>
inline constexpr bool is_value_v = is_value<T>::value;

namespace concepts
{
template<typename T>
concept value = is_value_v<T>;
} // namespace concepts

template<concepts::content CONTENT, auto VALUE>
struct get_width<value<CONTENT, VALUE>> : width<get_width_v<CONTENT>> {};

template<concepts::multireg_content CONTENT, auto VALUE>
struct get_width<value<CONTENT, VALUE>> : std::integral_constant<unsigned int, get_width_v<CONTENT>> {};

template<concepts::value_content CONTENT, auto VALUE>
struct base_content<value<CONTENT, VALUE>> : std::type_identity<CONTENT> {};

namespace internal
{
template<concepts::typelist LIST>
struct list_values_to_array;

template<typename... ELEMENTs>
struct list_values_to_array<typelist<ELEMENTs...>>
{
    static constexpr std::size_t size = sizeof...(ELEMENTs);
    using value_type = std::array<common_value_type_t<typelist<ELEMENTs...>>, size>;
    static constexpr value_type value = {{ELEMENTs::value...}};
};

} // namespace internal


template<concepts::content_list CONTENTS>
struct create_register_list
{
    template<concepts::reg REG1, concepts::reg REG2>
    struct by_address : std::bool_constant<get_address_v<REG1> < get_address_v<REG2>> {};

    using type = sort_t<unique_t<get_register_t<CONTENTS>>, by_address>;
};

template<concepts::content_list CONTENTS>
using create_register_list_t = typename create_register_list<CONTENTS>::type;

template<typename>
struct address_list;

template<typename T>
using address_list_t = typename address_list<T>::type;

template<concepts::reg_list REGISTERS>
struct address_list<REGISTERS>
{
    using type = transform_t<REGISTERS, get_address>;
};

template<concepts::content_list CONTENT>
struct address_list<CONTENT>
{
    using type = address_list_t<create_register_list_t<CONTENT>>;
};

namespace concepts
{
template<typename T>
concept address_list = is_typelist_v<T> && all_of_v<T, is_address>;
} // namespace concepts

namespace internal
{
template<concepts::address_list ADDRESSES>
struct address_datatype
{
    using type = typename front_t<ADDRESSES>::value_type;

private:
    template<typename T>
    struct is_same_address_data_type : std::is_same<typename T::value_type, type> {};
    static_assert(all_of_v<ADDRESSES, is_same_address_data_type>, "Registers must not have different data types");
};

template<concepts::address_list ADDRESSES>
using address_datatype_t = typename address_datatype<ADDRESSES>::type;

} // namespace internal

template<typename T>
struct is_any_content : std::bool_constant<is_content_v<T> || is_multireg_content_v<T>> {};

namespace concepts
{
template<typename T>
concept any_content = is_any_content<T>::value;

template<typename T>
concept any_content_list = is_typelist_v<T> && all_of_v<T, is_any_content>;
} // namespace concepts

template<concepts::any_content... CONTENTs>
using content_list = typelist<CONTENTs...>;


namespace internal {

template<concepts::reg_list>
struct group_contiguous_helper;

template<concepts::reg FIRST>
struct group_contiguous_helper<register_list<FIRST>>
{
    using groups = typelist<>;
    using current_group = register_list<FIRST>;
};

template<concepts::reg FIRST, concepts::reg... RESTs>
struct group_contiguous_helper<register_list<FIRST, RESTs...>>
{
    using recursive = group_contiguous_helper<register_list<RESTs...>>;
    static constexpr bool is_previous_address = get_address_v<FIRST> + 1 == get_address_v<front_t<typename recursive::current_group>>;
    
    using groups = std::conditional_t<is_previous_address, typename recursive::groups, prepend_t<typename recursive::current_group, typename recursive::groups>>;
    using current_group = std::conditional_t<is_previous_address, prepend_t<FIRST, typename recursive::current_group>, typelist<FIRST>>;
};

template<concepts::reg_list LIST>
struct group_contiguous_helper_final
{
    using groups = group_contiguous_helper<LIST>;
    using type = prepend_t<typename groups::current_group, typename groups::groups>;
};

} // namespace internal

template<concepts::reg_list LIST>
struct group_contiguous_address
{
    using type = typename internal::group_contiguous_helper_final<LIST>::type;
};

template<concepts::reg_list LIST>
using group_contiguous_address_t = typename group_contiguous_address<LIST>::type;

} // namespace tmp
