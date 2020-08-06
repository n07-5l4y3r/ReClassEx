#pragma once

#if _WIN32
	typedef unsigned __int32 ptruint;
#else
	typedef unsigned __int64 ptruint;
#endif

namespace mod
{

	class cPointerScanner
	{

		// ====================================================================================================================================== //
		//	https://github.com/cheat-engine/cheat-engine/blob/416fcdc36f7e2dc7c44a58abf3e95e336d417038/Cheat%20Engine/pointerscanworker.pas#L612  //
		// ====================================================================================================================================== //
		static void rscan(void* valuetofind, signed level);
		// ====================================================================================================================================== //



	};

}