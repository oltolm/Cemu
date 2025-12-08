#pragma once

#include <wx/event.h>
#include <wx/panel.h>

class wxListView;

class ModuleWindow : public wxPanel
{
  public:
	ModuleWindow(wxWindow& parent);

	void OnGameLoaded();
	void OnGameLoaded(wxCommandEvent& event);

  private:
	void OnLeftDClick(wxMouseEvent& event);
	void OnNotifyModuleLoaded(wxCommandEvent& event);

	wxListView* m_modules;
};
