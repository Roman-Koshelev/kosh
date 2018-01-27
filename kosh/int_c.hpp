#ifndef KOSH_INT_C_HPP
#define KOSH_INT_C_HPP

#include <type_traits>

namespace kosh
{
	template <unsigned i>
	using uint_c = std::integral_constant<unsigned, i>;

	namespace detail
	{
		constexpr unsigned to_int(const char c)
		{
			return (c >= 'A' && c <= 'F')
					   ? (c - 'A' + 10)
					   : (c >= 'a' && c <= 'f') ? (c - 'a' + 10) : (c - '0');
		}

		template <std::size_t N>
		constexpr unsigned
		parse2(const char (&arr)[N], int base, int offset, int multiplier, int i)
		{
			return (std::size_t(i) >= (N - offset))
					   ? 0
					   : (arr[N - 1 - i] == '\''
							  ? parse2(arr, base, offset, multiplier, i + 1)
							  : to_int(arr[N - 1 - i]) * multiplier +
									parse2(arr, base, offset, multiplier * base, i + 1));
		}

		template <std::size_t N>
		constexpr unsigned parse1(const char(&arr)[N])
		{
			return (arr[1] == 'x') ? parse2(arr, 16, 2, 1, 0)
				: (arr[1] == 'b') ? parse2(arr, 2, 2, 1, 0)
				: parse2(arr, 8, 1, 1, 0);
		}

		template <std::size_t N>
		constexpr unsigned parse(const char (&arr)[N])
		{
			return (N > 2 && arr[0] == '0') ? parse1(arr) : parse2(arr, 10, 0, 1, 0);
		}

	}  // namespace detail

	inline namespace literals
	{
		template <char... digits>
		constexpr auto operator"" _c()
		{
			return uint_c<detail::parse<sizeof...(digits)>({digits...})>{};
		}

	}  // namespace literals

}  // namespace kosh

#endif  // KOSH_INT_C_HPP
