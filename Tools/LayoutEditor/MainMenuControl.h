/*!
	@file
	@author		Albert Semenov
	@date		08/2010
*/

#ifndef _b4bacb08_2590_44f7_8e17_1d423ec88e6d_
#define _b4bacb08_2590_44f7_8e17_1d423ec88e6d_

#include <MyGUI.h>
#include "WidgetContainer.h"
#include "BaseLayout/BaseLayout.h"
#include "sigslot.h"

namespace tools
{

	class MainMenuControlLE : public wraps::BaseLayout, public sigslot::has_slots<>
	{
	public:
		MainMenuControlLE(MyGUI::Widget* _parent = nullptr);
		~MainMenuControlLE() override;

		void setVisible(bool _value);

	private:
		void createMainMenu();
		void notifyPopupMenuAccept(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item);

		void widgetsUpdate();
		void createWidgetPopup(
			WidgetContainer* _container,
			MyGUI::MenuControl* _parentPopup,
			bool _print_name,
			bool _print_type,
			bool _print_skin);
		void notifyWidgetsSelect(MyGUI::MenuControl* _sender, MyGUI::MenuItem* _item);
		std::string getDescriptionString(MyGUI::Widget* _widget, bool _print_name, bool _print_type, bool _print_skin);

		void notifyChangeWidgets();
		void notifySettingsChanged(std::string_view _path);

		void updateRecentFilesMenu();
		void updateRecentProjectsMenu();
		void updateMenuScale(size_t _scale);

		void CommandOnChangeScale(const MyGUI::UString& _commandName, bool& _result);

	private:
		MyGUI::MenuBar* mBar{nullptr};
		MyGUI::MenuControl* mPopupMenuWidgets{nullptr};
	};

}

#endif
