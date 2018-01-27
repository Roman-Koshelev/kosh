#ifndef KOSH_HEAP_AND_STACK_MEMORY_HPP
#define KOSH_HEAP_AND_STACK_MEMORY_HPP

#include <memory>

namespace kosh
{
	namespace detail
	{
		template <unsigned SIZE, class Ty>
		class stack_memory
		{
			Ty mem[SIZE];

		public:
			Ty *get_mem() { return mem; }
		};

		template <unsigned SIZE, class Ty>
		class heap_memory
		{
			std::unique_ptr<Ty[]> mem;

		public:
			heap_memory() { mem = std::make_unique<Ty[]>(SIZE); }
			Ty *get_mem() { return mem.get(); }
		};
	}  // namespace detail

	template <class Ty, std::size_t SIZE>
	class memory_allocation
	{
		std::conditional_t<(SIZE * sizeof(Ty) > 1024), detail::heap_memory<SIZE, Ty>,
						   detail::stack_memory<SIZE, Ty> >
			mem_b;

	public:
		memory_allocation(Ty *&ptr) { ptr = mem_b.get_mem(); }
	};

}  // namespace kosh

#endif  // KOSH_CONSTEXPR_FOR_EACH_HPP
