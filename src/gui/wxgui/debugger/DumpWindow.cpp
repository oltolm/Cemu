#include "wxgui/wxgui.h"
#include "wxgui/debugger/DumpWindow.h"

#include "wxgui/debugger/DebuggerWindow2.h"
#include "Cafe/HW/Espresso/Debugger/Debugger.h"
#include "wxgui/debugger/DumpCtrl.h"

enum
{
	// REGISTER
	REGISTER_ADDRESS_R0 = wxID_HIGHEST + 8200,
	REGISTER_LABEL_R0 = REGISTER_ADDRESS_R0 + 32,
	REGISTER_LABEL_FPR0_0 = REGISTER_LABEL_R0 + 32,
	REGISTER_LABEL_FPR1_0 = REGISTER_LABEL_R0 + 32,
};

DumpWindow::DumpWindow(wxWindow& parent)
	: wxPanel(&parent)
{
	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);
	m_dump_ctrl = new DumpCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxScrolledWindowStyle);
	main_sizer->Add(m_dump_ctrl, 1, wxEXPAND);

	this->SetSizer(main_sizer);
	this->wxWindowBase::Layout();

	this->Centre(wxBOTH);
}

void DumpWindow::OnGameLoaded()
{
	m_dump_ctrl->Init();
}
