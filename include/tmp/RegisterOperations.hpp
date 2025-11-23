/*
 * Copyright (c) 2025 Alexander Wachter
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "Typelist.hpp"
#include "Registers.hpp"
#include "RegisterAttributes.hpp"

#include <type_traits>

namespace tmp
{

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
    using data_type = get_datatype_t<front_t<LIST>>;

    template<typename T> struct same_type : std::is_same<T, data_type> {};
    static constexpr bool value = all_of_v<transform_t<LIST, get_datatype>, same_type>;
};

template<typename T>
inline constexpr bool all_datatypes_are_same_v = all_datatypes_are_same<T>::value;

template<concepts::content T, concepts::content_list LIST>
struct value_shift;

template<concepts::content T, concepts::content... CONTENTs>
struct value_shift<T, typelist<T, CONTENTs...>> : std::integral_constant<unsigned int, 0> {};

template<concepts::content T, concepts::content FIRST, concepts::content... RESTs>
struct value_shift<T, typelist<FIRST, RESTs...>> : std::integral_constant<unsigned int, value_shift<T, typelist<RESTs...>>::value + get_width_v<FIRST>> {};

template<concepts::reg_list REGISTERS, concepts::multireg_content REG>
struct extract_multireg;

template<concepts::reg_list REGISTERS, concepts::attribute_list ATTRIBUTES, concepts::datatype DATATYPE, concepts::content... CONTENTs>
struct extract_multireg<REGISTERS, multireg_content<typelist<CONTENTs...>, ATTRIBUTES, DATATYPE>>
{
    using contents = typelist<CONTENTs...>;
    using reg_datatype = std::remove_const_t<get_datatype_t<get_register_t<front_t<contents>>>>;
    using value_type = typename DATATYPE::type;
    template<concepts::content T, concepts::content_list LIST>
    static constexpr std::make_unsigned_t<value_type> get_part(const reg_datatype* data) noexcept
    {
        constexpr std::size_t index = index_of_v<get_register_t<T>, REGISTERS>;
        constexpr auto shift = value_shift<T, LIST>::value;
        return ((data[index] & get_mask_v<T>) >> get_offset_v<T>) << shift;
    }

    static constexpr std::make_unsigned_t<value_type> extract(const reg_datatype* data) noexcept
    {
        return (get_part<CONTENTs, contents>(data) | ...);
    }

    template<concepts::content T, concepts::content_list LIST>
    static constexpr void set_part(value_type value, reg_datatype* data) noexcept
    {
        constexpr std::size_t index = index_of_v<get_register_t<T>, REGISTERS>;
        constexpr auto shift = value_shift<T, LIST>::value;
        data[index] = (data[index] & ~get_mask_v<T>) | (((value >> shift) << get_offset_v<T>) & get_mask_v<T>);
    }

    static constexpr void set(value_type value, reg_datatype* data) noexcept
    {
        (set_part<CONTENTs, contents>(value, data), ...);
    }

};

template<typename T> 
static inline constexpr auto fix_sign(std::make_unsigned_t<get_datatype_t<T>> value) noexcept -> get_datatype_t<T> 
{
    using datatype = get_datatype_t<T>;
    const bool is_negative = (value & (1 << (get_width_v<T> - 1)));
    constexpr datatype negative_get_mask = static_cast<datatype>(~get_mask_v<T>);
    return is_negative ? value | negative_get_mask : value;
}
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
    : address_array<create_register_list_t<std::conditional_t<any_of_v<CONTENTS, is_multireg_content>, internal::linearize_content_t<CONTENTS>, CONTENTS>>> {};


template<concepts::content T>
inline constexpr get_datatype_t<T> extract(get_datatype_t<get_register_t<T>> registerValue) noexcept
requires (!std::is_signed_v<get_datatype_t<T>> || get_width_v<T> == sizeof(get_datatype_t<T>) * 8U)
{
    static_assert(!is_writeonly_v<T>, "Content cannot be read");
    return ((registerValue & get_mask_v<T>) >> get_offset_v<T>);
}

template<concepts::content T>
inline constexpr get_datatype_t<T> extract(get_datatype_t<get_register_t<T>> registerValue) noexcept
requires (std::is_signed_v<get_datatype_t<T>> && get_width_v<T> < sizeof(get_datatype_t<T>) * 8U)
{
    static_assert(!is_writeonly_v<T>, "Content cannot be read");
    return internal::fix_sign<T>((registerValue & get_mask_v<T>) >> get_offset_v<T>);
}

template<concepts::content T, concepts::reg_list REGISTERS>
inline constexpr get_datatype_t<T> extract(const get_datatype_t<get_register_t<T>> *registerValue) noexcept
{
    static_assert(internal::all_datatypes_are_same_v<REGISTERS>, "registers must be of the same datatype");
    constexpr std::size_t index = index_of_v<get_register_t<T>, REGISTERS>;
    return extract<T>(registerValue[index]);
}

template<concepts::multireg_content T, concepts::reg_list REGISTERS>
requires (!std::is_signed_v<get_datatype_t<T>> || get_width_v<T> == sizeof(get_datatype_t<T>) * 8U)
inline constexpr get_datatype_t<T> extract(const get_datatype_t<front_t<REGISTERS>> *registerValue) noexcept
{
    static_assert(!is_writeonly_v<T>, "Content cannot be read");
    return internal::extract_multireg<REGISTERS, T>::extract(registerValue);
}

template<concepts::multireg_content T, concepts::reg_list REGISTERS>
inline constexpr get_datatype_t<T> extract(const get_datatype_t<front_t<REGISTERS>> *registerValue) noexcept
requires (std::is_signed_v<get_datatype_t<T>> && get_width_v<T> < sizeof(get_datatype_t<T>) * 8U)
{
    static_assert(!is_writeonly_v<T>, "Content cannot be read");
    return internal::fix_sign<T>(internal::extract_multireg<REGISTERS, T>::extract(registerValue));
}

template<concepts::content T>
inline constexpr void set(get_datatype_t<T> value, std::remove_const_t<get_datatype_t<get_register_t<T>>>& registerValue) noexcept
{
    static_assert(!is_readonly_v<T>, "Content cannot be written");
    registerValue = (registerValue & ~get_mask_v<T>) | ((value << get_offset_v<T> ) & get_mask_v<T>);
}

template<concepts::content T, concepts::reg_list REGISTERS>
inline constexpr void set(get_datatype_t<T> value, std::remove_const_t<get_datatype_t<front_t<REGISTERS>>>* registerValue) noexcept
{
    static_assert(!is_readonly_v<T>, "Content cannot be written");
    constexpr std::size_t index = index_of_v<get_register_t<T>, REGISTERS>;
    set<T>(value, registerValue[index]);
}

template<concepts::multireg_content T, concepts::reg_list REGISTERS>
inline constexpr void set(get_datatype_t<T> value, std::remove_const_t<get_datatype_t<front_t<REGISTERS>>>* registerValue) noexcept
{
    static_assert(!is_readonly_v<T>, "Content cannot be written");
    internal::extract_multireg<REGISTERS, T>::set(value, registerValue);
}

template<concepts::reg_list REGISTERS>
class Registers {
public:
    static_assert(internal::all_datatypes_are_same_v<REGISTERS>, "registers must be of the same datatype");

    static constexpr std::size_t size = count_v<REGISTERS>;
    using value_type = get_datatype_t<front_t<REGISTERS>>;
    using view_type = std::span<const value_type, size>;

    constexpr Registers(view_type raw_data) noexcept : data(raw_data) {}

    template<concepts::any_content T>
    constexpr auto get() const noexcept
    {
        return extract<T, REGISTERS>(data.data());
    }

private:

    view_type data;
};

} // namespace tmp