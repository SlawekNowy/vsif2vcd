template<typename Enum>
struct EnableBitMaskOperators
{
    static const bool enable = false;
};


#define ENABLE_BITMASK_OPERATORS(x) \
template<> \
struct EnableBitMaskOperators<x> \
{ \
    static const bool enable = true; \
};\



template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &(Enum lhs, Enum rhs)
{
    return static_cast<Enum> (
        static_cast<std::underlying_type<Enum>::type>(lhs) &
        static_cast<std::underlying_type<Enum>::type>(rhs)
        );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ^(Enum lhs, Enum rhs)
{
    return static_cast<Enum> (
        static_cast<std::underlying_type<Enum>::type>(lhs) ^
        static_cast<std::underlying_type<Enum>::type>(rhs)
        );
}

template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ~(Enum rhs)
{
    return static_cast<Enum> (
        ~static_cast<std::underlying_type<Enum>::type>(rhs)
        );
}


template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |=(Enum& lhs, Enum rhs)
{
    lhs = static_cast<Enum> (
        static_cast<std::underlying_type<Enum>::type>(lhs) |
        static_cast<std::underlying_type<Enum>::type>(rhs)
        );

    return lhs;
}


template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator &=(Enum& lhs, Enum rhs)
{
    lhs = static_cast<Enum> (
        static_cast<std::underlying_type<Enum>::type>(lhs) &
        static_cast<std::underlying_type<Enum>::type>(rhs)
        );

    return lhs;
}


template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator ^=(Enum& lhs, Enum rhs)
{
    lhs = static_cast<Enum> (
        static_cast<std::underlying_type<Enum>::type>(lhs)
        static_cast<std::underlying_type<Enum>::type>(rhs)
        );

    return lhs;
}


template<typename Enum>
typename std::enable_if<EnableBitMaskOperators<Enum>::enable, Enum>::type
operator |(Enum lhs, Enum rhs)
{
    static_assert(std::is_enum<Enum>::value,
        "template parameter is not an enum type");

    using underlying = typename std::underlying_type<Enum>::type;

    return static_cast<Enum> (
        static_cast<underlying>(lhs) |
        static_cast<underlying>(rhs)
        );
}