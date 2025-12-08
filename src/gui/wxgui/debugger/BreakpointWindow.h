#pragma once

#include <wx/panel.h>

class wxCommandEvent;
class wxListEvent;
class wxListView;

class BreakpointWindow : public wxPanel
{
  public:
	BreakpointWindow(wxWindow& parent);
	virtual ~BreakpointWindow();

	void OnUpdateView();
	void OnUpdateView(wxCommandEvent& event);
	void OnGameLoaded();

  private:
	void OnBreakpointToggled(wxListEvent& event);
	void OnLeftDClick(wxMouseEvent& event);
	void OnRightDown(wxMouseEvent& event);

	void OnContextMenuClick(wxCommandEvent& evt);
	void OnContextMenuClickSelected(wxCommandEvent& evt);

	wxListView* m_breakpoints;
};
