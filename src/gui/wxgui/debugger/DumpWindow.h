#pragma once

#include "wxgui/debugger/DumpCtrl.h"

class DumpWindow : public wxPanel
{
  public:
	DumpWindow(wxWindow& parent);

	void OnGameLoaded();

  private:
	wxScrolledWindow* m_scrolled_window;
	DumpCtrl* m_dump_ctrl;
};
