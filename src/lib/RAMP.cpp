/**
 * @file RAMP.cpp
 * @brief Non-template RAMP member definitions.
 *
 * Implements construct, destroy, and indexed delete for @ref ramp::RAMP.
 */

#include "ramp/lib/RAMP.hpp"

namespace ramp
{
	RAMP::RAMP()
	{
		this->objs.reserve(16);
	}

	RAMP::~RAMP()
	{ this->destroy(); }

	bool RAMP::deleteElem(const std::size_t i)
	{
		if (i >= this->objs.size())
			return false;

		auto &obj = this->objs[i];

		if (!obj.first || !obj.second)
			return false;

		obj.second();

		this->objs.erase(this->objs.begin() + i);

		return true;
	}

	bool RAMP::destroy()
	{
		bool success = true;

		if (this->enableReverse)
		{
			/* Last added → first added (LIFO). */
			while (!this->objs.empty())
			{
				const std::size_t i = this->objs.size() - 1;

				if (!this->deleteElem(i))
				{
					success = false;
					break;
				}
			}
		}
		else
		{
			/* First added → last added (FIFO). */
			while (!this->objs.empty())
			{
				if (!this->deleteElem(0))
				{
					success = false;
					break;
				}
			}
		}

		return success;
	}
}
