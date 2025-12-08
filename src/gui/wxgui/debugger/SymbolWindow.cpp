#include "wxgui/wxgui.h"
#include "wxgui/debugger/SymbolWindow.h"
#include "wxgui/debugger/DebuggerWindow2.h"
#include "Cafe/HW/Espresso/Debugger/Debugger.h"
#include "Cafe/OS/RPL/rpl_symbol_storage.h"

enum ItemColumns
{
	ColumnName = 0,
	ColumnAddress,
	ColumnModule,
};

SymbolWindow::SymbolWindow(wxWindow& parent)
	: wxPanel(&parent)
{
	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	m_symbol_ctrl = new SymbolListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
	main_sizer->Add(m_symbol_ctrl, 1, wxEXPAND);

	m_filter = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL);
	m_filter->Bind(wxEVT_TEXT, &SymbolWindow::OnFilterChanged, this);
	main_sizer->Add(m_filter, 0, wxALL | wxEXPAND, 5);

	this->SetSizer(main_sizer);
	this->wxWindowBase::Layout();

	this->Centre(wxHORIZONTAL);
	Bind(wxEVT_NOTIFY_MODULE_LOADED, &SymbolWindow::OnGameLoaded, this);
}

void SymbolWindow::OnGameLoaded()
{
	m_symbol_ctrl->OnGameLoaded();
}

void SymbolWindow::OnGameLoaded(wxCommandEvent& event)
{
	OnGameLoaded();
}

void SymbolWindow::OnFilterChanged(wxCommandEvent& event)
{
	m_symbol_ctrl->ChangeListFilter(m_filter->GetValue());
}
