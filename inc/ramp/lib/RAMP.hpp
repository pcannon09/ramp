#ifndef INCLUDE_LIB_RAMP_H_
#define INCLUDE_LIB_RAMP_H_

/**
 * @file RAMP.hpp
 * @brief Resource Acquisition && Management Primitives
 *
 * Tracks heap pointers and runs a destructor for each when removed
 * or when the RAMP object is destroyed.
 */

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include <vector>

#include "ramp/RAMPpredefines.h"

namespace ramp
{
	/**
	 * @brief Stored entry pairing a raw pointer with its cleanup callable.
	 *
	 * First element is the registered resource address; second is the
	 * destructor invoked when the entry is removed or destroyed.
	 */
	using RAMP_Object = std::pair<void *, std::function<void()>>;

	/**
	 * @brief Tracks heap pointers and runs their destructors on remove or scope exit.
	 *
	 * Register resources with #add, release one early with #remove / #deleteElem,
	 * or free all remaining via #destroy / the destructor. Cleanup order is
	 * controlled by #enableReverse.
	 */
	class RAMP
	{
	private:
		std::vector<RAMP_Object> objs; /**< Registered resources and their destructors. */

		/**
		 * @brief Default cleanup used when #add is called without a destructor.
		 *
		 * - `void*` or trivially destructible `T*` → `free()` via #RAMP_FREE
		 * - non-trivial `T*` → `delete` via #RAMP_DEL
		 *
		 * @tparam T Pointed-to type of the registered resource.
		 * @param resource Pointer to free or delete.
		 */
		template<typename T>
		static void defaultDestroy(T *resource)
		{
			if constexpr (std::is_void_v<T>)
			{ RAMP_FREE(resource); }

			else if constexpr (!std::is_trivially_destructible_v<T>)
			{ RAMP_DEL(resource); }

			else RAMP_FREE(resource);
		}

	public:
		/**
		 * @brief Controls destroy/remove traversal order.
		 *
		 * - `false`: first-added → last-added (FIFO)
		 * - `true`: last-added → first-added (LIFO)
		 */
		bool enableReverse = false;

		/**
		 * @brief Constructs an empty RAMP with a small reserved capacity.
		 */
		RAMP();

		/**
		 * @brief Destroys all remaining registered resources.
		 *
		 * Calls #destroy(); order follows #enableReverse.
		 */
		~RAMP();

		/**
		 * @brief Run the destructor for `objs[i]` and erase the entry.
		 *
		 * @param i Index into the internal resource list.
		 * @return `true` on success; `false` if @p i is out of range or the
		 *         entry is invalid (null pointer or empty destructor).
		 */
		bool deleteElem(std::size_t i);

		/**
		 * @brief Register a pointer with an explicit destructor callable.
		 *
		 * The destructor is invoked as `destructor(resource)` when the entry
		 * is removed or destroyed.
		 *
		 * @tparam Resource Pointer type of the resource.
		 * @tparam Destructor Callable taking @p Resource (or compatible).
		 * @param resource Non-null pointer to manage.
		 * @param destructor Cleanup callable; moved into the stored entry.
		 * @return `true` if registered; `false` if @p resource is null.
		 *
		 * @note @p Resource must be a pointer type (`static_assert`).
		 */
		template<typename Resource, typename Destructor>
		bool add(Resource resource, Destructor destructor)
		{
			static_assert(std::is_pointer_v<Resource>, "RAMP::add expects a pointer");

			if (!resource)
				return false;

			auto deleter = [resource, destructor = std::move(destructor)]() mutable
			{
				destructor(resource);
			};

			this->objs.emplace_back(
				static_cast<void *>(resource),
				std::move(deleter)
			);

			return true;
		}

		/**
		 * @brief Register a pointer using #defaultDestroy (`free` or `delete` by type).
		 *
		 * @tparam Resource Pointer type of the resource.
		 * @param resource Non-null pointer to manage.
		 * @return `true` if registered; `false` if @p resource is null.
		 *
		 * @note @p Resource must be a pointer type (`static_assert`).
		 * @note If you allocate a trivially destructible type with `new`, pass
		 *       an explicit destructor to the two-argument #add overload.
		 */
		template<typename Resource>
		bool add(Resource resource)
		{
			static_assert(std::is_pointer_v<Resource>, "RAMP::add expects a pointer");

			using T = std::remove_pointer_t<Resource>;

			return this->add(resource, &RAMP::defaultDestroy<T>);
		}

		/**
		 * @brief Find a resource by pointer, run its destructor, and erase it.
		 *
		 * Search order follows #enableReverse (FIFO or LIFO).
		 *
		 * @tparam Resource Pointer type of the resource.
		 * @param resource Pointer previously registered with #add.
		 * @return `true` if found and destroyed; `false` if null or not found.
		 *
		 * @note @p Resource must be a pointer type (`static_assert`).
		 */
		template<typename Resource>
		bool remove(Resource resource)
		{
			static_assert(std::is_pointer_v<Resource>, "RAMP::remove expects a pointer");

			if (!resource)
				return false;

			void *const ptr = static_cast<void *>(resource);

			if (this->enableReverse)
			{
				for (std::size_t i = this->objs.size(); i > 0; --i)
				{
					if (this->objs[i - 1].first == ptr)
						return this->deleteElem(i - 1);
				}
			}

			else
			{
				for (std::size_t i = 0; i < this->objs.size(); ++i)
				{
					if (this->objs[i].first == ptr)
						return this->deleteElem(i);
				}
			}

			return false;
		}

		/**
		 * @brief Destroy every registered resource.
		 *
		 * Order follows #enableReverse. Stops early if an entry fails to destroy.
		 *
		 * @return `true` if all entries were destroyed; `false` if any
		 *         #deleteElem call failed.
		 */
		bool destroy();
	};
}

#endif  // INCLUDE_LIB_RAMP_H_
