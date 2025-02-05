// by Marius Versteegen, 2023

#pragma once
#include <crt_CleanRTOS.h>

namespace crt
{
	class CounterForTestHandler2 : public IHandlerListener
	{
	private:
		const char* counterName;
		int32_t count;
	public:
		CounterForTestHandler2(const char *counterName, IHandler& counterHandler) : counterName(counterName), count(0)
		{
			counterHandler.addHandlerListener(this);
		}

		/*override keyword not supported in current compiler*/
		void update()
		{
			count++;
		}
	}; // end class CounterForTestHandler2
};// end namespace crt
