#pragma once

#include "wxgui/debugger/SymbolCtrl.h"
#include <wx/event.h>
#include <wx/panel.h>

class SymbolWindow : public wxPanel
{
  public:
	SymbolWindow(wxWindow& parent);

	void OnGameLoaded();
	void OnGameLoaded(wxCommandEvent& event);

	void OnLeftDClick(wxListEvent& event);

  private:
	wxTextCtrl* m_filter;
	SymbolListCtrl* m_symbol_ctrl;

	void OnFilterChanged(wxCommandEvent& event);
};
