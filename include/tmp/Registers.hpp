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

template<typename>
struct address;

template<typename T>
inline constexpr typename address<T>::value_type address_v = address<T>::value;

template<typename>
struct is_address : std::false_type {};

template<typename T>
struct is_address<address<T>> : std::true_type {};

template<typename T>
inline constexpr bool is_address_v = is_address<T>::value;

template<typename>
struct width;

template<typename T>
inline constexpr typename width<T>::value_type width_v = width<T>::value;

template<typename>
struct offset;

template<typename T>
inline constexpr typename offset<T>::value_type offset_v = offset<T>::value;

template<typename>
struct mask;

template<typename T>
inline constexpr typename mask<T>::value_type mask_v = mask<T>::value;

template<typename>
struct reset_value;

template<typename T>
inline constexpr typename reset_value<T>::value_type reset_value_v = reset_value<T>::value;

template<typename>
struct datatype;

template<typename T>
using datatype_t = typename datatype<T>::type;

template<typename>
struct base_register;

template<typename T>
using base_register_t = typename base_register<T>::type;

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

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, std::unsigned_integral ADDRESS_T, typename DATATYPE = width_to_uint_t<WIDTH>, concepts::attribute_list ATTRIBUTES = typelist<>>
struct reg {
    static_assert(ADDRESS <= std::numeric_limits<ADDRESS_T>::max(), "Address value does not fit into the specified datatype");
    static_assert(RESET_VALUE <= std::numeric_limits<DATATYPE>::max(), "Reset Value does not fit into the specified datatype");
};

template<typename>
struct is_reg : std::false_type {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct is_reg<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::true_type {};

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

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct address<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::integral_constant<ADDRESS_T, ADDRESS> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct width<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::integral_constant<std::decay_t<decltype(WIDTH)>, WIDTH> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct mask<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::integral_constant<DATATYPE, ((1U << WIDTH) - 1U)> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct datatype<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::type_identity<DATATYPE> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct reset_value<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : std::integral_constant<DATATYPE, RESET_VALUE> {};

template<concepts::attribute ATTRIBUTE, std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE, typename ATTRIBUTES>
struct has_attribute<ATTRIBUTE, reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE, ATTRIBUTES>> : has_a<ATTRIBUTES, ATTRIBUTE> {};

template<concepts::reg REGISTER, unsigned int OFFSET = 0U, unsigned int WIDTH = width_v<REGISTER>, concepts::attribute_list ATTRIBUTES = typelist<>, typename DATATYPE = width_to_uint_t<WIDTH>>
struct content {
    static_assert(WIDTH + OFFSET < sizeof(DATATYPE) * 8U, "Content does not fit into datatype");
    static_assert(sizeof(DATATYPE) <= sizeof(datatype_t<REGISTER>), "Datatype does not fit into the register");
};

template<typename>
struct is_content : std::false_type {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE >
struct is_content<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_content_v = is_content<T>::value;

template<typename T>
using is_content_t = typename is_content<T>::type;

namespace concepts {
template<typename T>
concept content = is_content_v<T>;
} // namespace concepts

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct base_register<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::type_identity<REGISTER> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct address<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::integral_constant<typename address<REGISTER>::value_type, address_v<REGISTER>> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct width<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::integral_constant<std::decay_t<decltype(WIDTH)>, WIDTH> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct offset<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::integral_constant<std::decay_t<decltype(OFFSET)>, OFFSET> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct mask<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::integral_constant<datatype_t<REGISTER>, (((1U << WIDTH) - 1U) << OFFSET)> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
struct datatype<content<REGISTER, OFFSET, WIDTH, ATTRIBUTES, DATATYPE>> : std::type_identity<DATATYPE> {};

template<concepts::attribute ATTRIBUTE, concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename ATTRIBUTES, typename DATATYPE>
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
struct width<typelist<CONTENTs...>> : std::integral_constant<unsigned int, (width_v<CONTENTs> + ...)> {};

template<concepts::content_list CONTENT_LIST, concepts::attribute_list ATTRIBUTES = typelist<>, typename DATATYPE = width_to_uint_t<width_v<CONTENT_LIST>>>
struct multireg_content {};

template<typename>
struct is_multireg_content : std::false_type {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct is_multireg_content<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_multireg_content_v = is_multireg_content<T>::value;

namespace concepts {
template<typename T>
concept multireg_content = is_multireg_content_v<T>;
template<typename T>
concept value_content = is_content_v<T> || is_multireg_content_v<T>;
} // namespace concepts

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct width<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::integral_constant<unsigned int, width_v<CONTENT_LIST>> {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct mask<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::integral_constant<DATATYPE, ((1U << width_v<CONTENT_LIST>) - 1U)> {};

template<concepts::content... CONTENTs>
struct base_register<typelist<CONTENTs...>> {
    using type = transform_t<typelist<CONTENTs...>, base_register>;
};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct base_register<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> {
    using type = base_register_t<CONTENT_LIST>;
};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct base_content<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::type_identity<CONTENT_LIST> {};

template<concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct datatype<multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> : std::type_identity<DATATYPE> {};

template<concepts::attribute ATTRIBUTE, concepts::content_list CONTENT_LIST, typename ATTRIBUTES, typename DATATYPE>
struct has_attribute<ATTRIBUTE, multireg_content<CONTENT_LIST, ATTRIBUTES, DATATYPE>> :
    std::bool_constant<has_a_v<ATTRIBUTES, ATTRIBUTE> || has_attribute_v<ATTRIBUTE, CONTENT_LIST>> {};

template<concepts::value_content CONTENT, datatype_t<CONTENT> VALUE>
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
struct width<value<CONTENT, VALUE>> : std::integral_constant<typename width<CONTENT>::value_type, width_v<CONTENT>> {};

template<concepts::multireg_content CONTENT, auto VALUE>
struct width<value<CONTENT, VALUE>> : std::integral_constant<unsigned int, width_v<CONTENT>> {};

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
    struct by_address : std::bool_constant<address_v<REG1> < address_v<REG2>> {};

    using type = sort_t<unique_t<base_register_t<CONTENTS>>, by_address>;
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
    template<typename T>
    struct to_address : std::type_identity<address<T>> {};

    using type = transform_t<REGISTERS, to_address>;
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
    static constexpr bool is_previous_address = address_v<FIRST> + 1 == address_v<front_t<typename recursive::current_group>>;
    
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
