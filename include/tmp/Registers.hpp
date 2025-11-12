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

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, std::unsigned_integral ADDRESS_T, typename DATATYPE = width_to_uint_t<WIDTH>>
struct reg {
    static_assert(ADDRESS <= std::numeric_limits<ADDRESS_T>::max(), "Address value does not fit into the specified datatype");
    static_assert(RESET_VALUE <= std::numeric_limits<DATATYPE>::max(), "Reset Value does not fit into the specified datatype");
};

template<typename>
struct is_reg : std::false_type {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct is_reg<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_reg_v = is_reg<T>::value;

namespace concepts {
template<typename T>
concept reg = is_reg_v<T>;
template<typename T>
concept reg_list = is_typelist_v<T> && any_of_v<T, is_reg>;
} // namespace concepts

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct address<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::integral_constant<ADDRESS_T, ADDRESS> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct width<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::integral_constant<std::decay_t<decltype(WIDTH)>, WIDTH> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct mask<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::integral_constant<DATATYPE, ((1U << WIDTH) - 1U)> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct datatype<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::type_identity<DATATYPE> {};

template<std::unsigned_integral auto ADDRESS, std::integral auto RESET_VALUE, unsigned int WIDTH, typename ADDRESS_T, typename DATATYPE>
struct reset_value<reg<ADDRESS, RESET_VALUE, WIDTH, ADDRESS_T, DATATYPE>> : std::integral_constant<DATATYPE, RESET_VALUE> {};

template<concepts::reg REGISTER, unsigned int OFFSET = 0U, unsigned int WIDTH = width_v<REGISTER>, typename DATATYPE = width_to_uint_t<WIDTH>>
struct content {
    static_assert(WIDTH + OFFSET < sizeof(DATATYPE) * 8U, "Content does not fit into datatype");
    static_assert(sizeof(DATATYPE) <= sizeof(datatype_t<REGISTER>), "Datatype does not fit into the register");
};

template<typename>
struct is_content : std::false_type {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE >
struct is_content<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_content_v = is_content<T>::value;

template<typename T>
using is_content_t = typename is_content<T>::type;

namespace concepts {
template<typename T>
concept content = is_content_v<T>;
} // namespace concepts

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct base_register<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::type_identity<REGISTER> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct address<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::integral_constant<typename address<REGISTER>::value_type, address_v<REGISTER>> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct width<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::integral_constant<std::decay_t<decltype(WIDTH)>, WIDTH> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct offset<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::integral_constant<std::decay_t<decltype(OFFSET)>, OFFSET> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct mask<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::integral_constant<datatype_t<REGISTER>, (((1U << WIDTH) - 1U) << OFFSET)> {};

template<concepts::reg REGISTER, unsigned int OFFSET, unsigned int WIDTH, typename DATATYPE>
struct datatype<content<REGISTER, OFFSET, WIDTH, DATATYPE>> : std::type_identity<DATATYPE> {};

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

template<concepts::content_list CONTENT_LIST, typename DATATYPE = width_to_uint_t<width_v<CONTENT_LIST>>>
struct multireg_content {};

template<typename>
struct is_multireg_content : std::false_type {};

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct is_multireg_content<multireg_content<CONTENT_LIST, DATATYPE>> : std::true_type {};

template<typename T>
inline constexpr bool is_multireg_content_v = is_multireg_content<T>::value;

namespace concepts {
template<typename T>
concept multireg_content = is_multireg_content_v<T>;
template<typename T>
concept value_content = is_content_v<T> || is_multireg_content_v<T>;
} // namespace concepts

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct width<multireg_content<CONTENT_LIST, DATATYPE>> : std::integral_constant<unsigned int, width_v<CONTENT_LIST>> {};

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct mask<multireg_content<CONTENT_LIST, DATATYPE>> : std::integral_constant<DATATYPE, ((1U << width_v<CONTENT_LIST>) - 1U)> {};

template<concepts::content... CONTENTs>
struct base_register<typelist<CONTENTs...>> {
    using type = transform_t<typelist<CONTENTs...>, base_register>;
};

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct base_register<multireg_content<CONTENT_LIST, DATATYPE>> {
    using type = base_register_t<CONTENT_LIST>;
};

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct base_content<multireg_content<CONTENT_LIST, DATATYPE>> : std::type_identity<CONTENT_LIST> {};

template<concepts::content_list CONTENT_LIST, typename DATATYPE>
struct datatype<multireg_content<CONTENT_LIST, DATATYPE>> : std::type_identity<DATATYPE> {};

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
struct register_list
{
    template<concepts::reg REG1, concepts::reg REG2>
    struct by_address : std::bool_constant<address_v<REG1> < address_v<REG2>> {};

    using type = sort_t<unique_t<base_register_t<CONTENTS>>, by_address>;
};

template<concepts::content_list CONTENTS>
using register_list_t = typename register_list<CONTENTS>::type;

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
    using type = address_list_t<register_list_t<CONTENT>>;
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

namespace concepts
{

template<typename T>
struct is_any_content : std::bool_constant<is_content_v<T> || is_multireg_content_v<T>> {};

template<typename T>
concept any_content_list = is_typelist_v<T> && all_of_v<T, is_any_content>;
} // namespace concepts

namespace internal
{

template<typename>
struct linearize_content;

template<typename T>
using linearize_content_t = typename linearize_content<T>::type;

template<>
struct linearize_content<typelist<>>
{
    using type = typelist<>;
};

template<typename FIRST, typename... RESTs>
struct linearize_content<typelist<FIRST, RESTs...>>
{
    using type = prepend_t<FIRST, linearize_content_t<typelist<RESTs...>>>;
};

template<concepts::multireg_content FIRST, typename... RESTs>
struct linearize_content<typelist<FIRST, RESTs...>>
{
    using type = concat_t<base_content_t<FIRST>, linearize_content_t<typelist<RESTs...>>>;
};

template<concepts::typelist LIST>
struct all_datatypes_are_same
{
    using data_type = datatype_t<front_t<LIST>>;

    template<typename T> struct same_type : std::is_same<T, data_type> {};
    static constexpr bool value = all_of_v<transform_t<LIST, datatype>, same_type>;
};

template<typename T>
inline constexpr bool all_datatypes_are_same_v = all_datatypes_are_same<T>::value;

} // namespace internal

template<typename>
struct address_array;

template<concepts::reg_list REGISTERS>
struct address_array<REGISTERS> {
    using addresses = address_list_t<REGISTERS>;

    static constexpr std::size_t size = count_v<addresses>;
    using value_type = std::array<internal::address_datatype_t<addresses>, size>;
    
    static constexpr value_type value = internal::list_values_to_array<addresses>::value;
};

template<concepts::any_content_list CONTENTS>
struct address_array<CONTENTS>
    : address_array<register_list_t<std::conditional_t<any_of_v<CONTENTS, is_multireg_content>, internal::linearize_content_t<CONTENTS>, CONTENTS>>> {};


template<concepts::reg_list REGISTERS>
class ContentView {
public:
    static_assert(internal::all_datatypes_are_same_v<REGISTERS>, "registers must be of the same datatype");

    static constexpr std::size_t size = count_v<REGISTERS>;
    using view_type = std::span<const datatype_t<front_t<REGISTERS>>, size>;

    constexpr ContentView(view_type raw_data) noexcept : data(raw_data) {}

    template<concepts::content T>
    constexpr auto get() const noexcept -> datatype_t<T>
    {
        constexpr std::size_t index = index_of_v<base_register_t<T>, REGISTERS>;
        return ((data[index] & mask_v<T>) >> offset_v<T>);
    }

    template<concepts::content T>
    constexpr auto get() const noexcept -> datatype_t<T> requires (std::is_signed_v<datatype_t<T>> && width_v<T> < sizeof(datatype_t<T>) * 8U)
    {
        constexpr std::size_t index = index_of_v<base_register_t<T>, REGISTERS>;
        return fix_sign<T>((data[index] & mask_v<T>) >> offset_v<T>);
    }

    template<concepts::multireg_content T>
    constexpr auto get() const noexcept -> datatype_t<T>
    {
        return get_multireg_value<T>::get(data);
    }

    template<concepts::multireg_content T>
    constexpr auto get() const noexcept -> datatype_t<T> requires (std::is_signed_v<datatype_t<T>> && width_v<T> < sizeof(datatype_t<T>) * 8U)
    {
        return fix_sign<T>(get_multireg_value<T>::get(data));
    }

private:
    template<typename T> 
    static inline constexpr auto fix_sign(std::make_unsigned_t<datatype_t<T>> value) noexcept -> datatype_t<T> 
    {
        using datatype = datatype_t<T>;
        const bool is_negative = (value & (1 << (width_v<T> - 1)));
        constexpr datatype negative_mask = static_cast<datatype>(~mask_v<T>);
        return is_negative ? value | negative_mask : value;
    }

    template<concepts::content T, concepts::content_list LIST>
    struct value_shift;

    template<concepts::content T, concepts::content... CONTENTs>
    struct value_shift<T, typelist<T, CONTENTs...>> : std::integral_constant<unsigned int, 0> {};

    template<concepts::content T, concepts::content FIRST, concepts::content... RESTs>
    struct value_shift<T, typelist<FIRST, RESTs...>> : std::integral_constant<unsigned int, value_shift<T, typelist<RESTs...>>::value + width_v<FIRST>> {};

    template<concepts::multireg_content REG>
    struct get_multireg_value;

    template<typename DATATYPE, concepts::content... CONTENTs>
    struct get_multireg_value<multireg_content<typelist<CONTENTs...>, DATATYPE>>
    {
        template<concepts::content T, concepts::content_list LIST>
        static constexpr std::make_unsigned_t<DATATYPE> get_part(view_type data) noexcept
        {
            constexpr std::size_t index = index_of_v<base_register_t<T>, REGISTERS>;
            constexpr auto shift = value_shift<T, LIST>::value;
            return ((data[index] & mask_v<T>) >> offset_v<T>) << shift;
        }

        static constexpr std::make_unsigned_t<DATATYPE> get(view_type data) noexcept
        {
            using list = typelist<CONTENTs...>;
            return (get_part<CONTENTs, list>(data) | ...);
        }
    };

    view_type data;
};

} // namespace tmp
