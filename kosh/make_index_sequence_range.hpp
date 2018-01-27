#ifndef KOSH_MAKE_INDEX_SEQUENCE_RANGE_HPP  // make_index_sequence_range
#define KOSH_MAKE_INDEX_SEQUENCE_RANGE_HPP

#include <type_traits>

namespace kosh
{
	namespace detail
	{
		template <class Ty, class N1, class N2, Ty N>
		struct accum_integer_sequence;

		template <class Ty, Ty... N1, Ty... N2, Ty N>
		struct accum_integer_sequence<Ty, std::integer_sequence<Ty, N1...>,
									  std::integer_sequence<Ty, N2...>, N>
		{
			using type = std::integer_sequence<Ty, N1..., (N + N2)...>;
		};

		template <class N>
		struct make_integer_sequence_impl
		{
			using Ty = typename N::value_type;
			static constexpr Ty n = N::value;
			static constexpr Ty m = (n / 2);
			using part1 =
				typename make_integer_sequence_impl<std::integral_constant<Ty, m>>::type;
			using part2 = typename make_integer_sequence_impl<
				std::integral_constant<Ty, n - m>>::type;
			using type = typename accum_integer_sequence<Ty, part1, part2, m>::type;
		};

		template <class Ty>
		struct make_integer_sequence_impl<std::integral_constant<Ty, 0>>
		{
			using type = std::integer_sequence<Ty>;
		};

		template <class Ty>
		struct make_integer_sequence_impl<std::integral_constant<Ty, 1>>
		{
			using type = std::integer_sequence<Ty, 0>;
		};

	}  // namespace detail

	template <std::size_t... Ns>
	using index_sequence = std::integer_sequence<std::size_t, Ns...>;

	template <class Ty, Ty N>
	using make_integer_sequence =
		typename detail::make_integer_sequence_impl<std::integral_constant<Ty, N>>::type;

	template <std::size_t N>
	using make_index_sequence = make_integer_sequence<std::size_t, N>;

	template <class... Ty>
	using index_sequence_for = make_index_sequence<sizeof...(Ty)>;

	template <class Ty, class N, Ty S1, Ty S2>
	struct help2;

	template <class Ty, Ty... N, Ty S1, Ty S2>
	struct help2<Ty, std::integer_sequence<Ty, N...>, S1, S2>
	{
		using type = std::integer_sequence<Ty, (S2 + N * S1)...>;
	};

	template <class Ty, Ty N, Ty S1, Ty S2>
	class helper
	{
		using type2 =
			make_integer_sequence<Ty, (((N - S2) % S1 == 0) ? ((N - S2) / S1)
															: ((N - S2) / S1 + 1))>;

	public:
		using type = typename help2<Ty, type2, S1, S2>::type;
	};

	template <class Ty, Ty S2, Ty N, Ty S1>
	using make_integer_sequence_range = typename helper<Ty, N, S1, S2>::type;

	template <std::size_t S2, std::size_t N, std::size_t S1>
	using make_index_sequence_range = make_integer_sequence_range<std::size_t, S2, N, S1>;

	/*template <std::size_t ... Radices>
	void fun(std::index_sequence<Radices...>)
	{
		auto dummy = {
			(std::cerr << Radices << " ", 0)...
		};
		(void)dummy;
	}

	int main()
	{
		fun(make_index_sequence_range<5, 15, 2>());  // output:5 7 9 11 13 \n
		printf("\n");
		return 0;
	}*/

}  // namespace kosh

#endif  // KOSH_MAKE_INDEX_SEQUENCE_RANGE_HPP
