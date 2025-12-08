#pragma once
#include "Cafe/HW/Espresso/Debugger/Debugger.h"
#include <wx/event.h>
#include <wx/panel.h>

class wxTextCtrl;

class RegisterWindow : public wxPanel
{
  public:
	RegisterWindow(wxWindow& parent);

	void OnUpdateView();
	void OnUpdateView(wxCommandEvent& event);

  private:
	void OnMouseDClickEvent(wxMouseEvent& event);
	void OnFPViewModePress(wxCommandEvent& event);
	void OnValueContextMenu(wxContextMenuEvent& event);
	void OnValueContextMenuSelected(wxCommandEvent& event);

	void UpdateIntegerRegister(wxTextCtrl* label, wxTextCtrl* value, uint32 registerValue, bool hasChanged);

	PPCSnapshot m_prev_snapshot;
	bool m_show_double_values;
	wxColour m_changed_color = {0xFF0000FF};

	wxTextCtrl* m_context_ctrl;
};
