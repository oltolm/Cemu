#pragma once
#include <wx/dataview.h>
#include <wx/frame.h>

class DebuggerWindow2;

class BreakpointWindow : public wxFrame
{
  public:
	BreakpointWindow(DebuggerWindow2& parent, const wxPoint& main_position, const wxSize& main_size);
	virtual ~BreakpointWindow();

	void OnMainMove(const wxPoint& position, const wxSize& main_size);
	void OnUpdateView();
	void OnGameLoaded();

  private:
	void OnBreakpointToggled(wxDataViewEvent& event);
	void OnEditingStarted(wxDataViewEvent& event);
	void OnEditingDone(wxDataViewEvent& event);
	void OnContextMenu(wxDataViewEvent& event);
	void OnItemActivated(wxDataViewEvent& event);

	void OnContextMenuClick(wxCommandEvent& evt);
	void OnContextMenuClickSelected(wxCommandEvent& evt);

	wxDataViewListCtrl* m_breakpoints;
};
