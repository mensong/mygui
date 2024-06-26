/*!
	@file
	@author		George Evmenov
	@date		03/2011
*/

#ifndef _8ff292d0_326a_4105_8cf2_e1648d87659e_
#define _8ff292d0_326a_4105_8cf2_e1648d87659e_

#include <MyGUI_Singleton.h>
#include "sigslot.h"

namespace tools
{

	class MYGUI_EXPORT_DLL GridManager : public sigslot::has_slots<>
	{
		MYGUI_SINGLETON_DECLARATION(GridManager);

	public:
		GridManager();

		void initialise();
		void shutdown();

		enum GridLine
		{
			Previous,
			Closest,
			Next
		};
		int toGrid(int _value, GridLine _line = Closest) const;

	private:
		void notifySettingsChanged(std::string_view _path);

		int mGridStep{0};
	};

}

#endif
