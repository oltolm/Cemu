#include "gui/wxgui.h"
#include "gui/debugger/BreakpointWindow.h"

#include <wx/clntdata.h>
#include <wx/dataview.h>
#include <wx/dvrenderers.h>
#include <wx/generic/dataview.h>
#include <wx/generic/dvrenderers.h>
#include <wx/headercol.h>
#include <wx/treelist.h>
#include <wx/types.h>
#include <wx/wupdlock.h>

#include "gui/debugger/DebuggerWindow2.h"
#include "gui/guiWrapper.h"
#include "Cafe/HW/Espresso/Debugger/Debugger.h"

#include "Cemu/ExpressionParser/ExpressionParser.h"

enum
{
	MENU_ID_CREATE_CODE_BP_EXECUTION = 1,
	MENU_ID_CREATE_CODE_BP_LOGGING,
	MENU_ID_CREATE_MEM_BP_READ,
	MENU_ID_CREATE_MEM_BP_WRITE,
	MENU_ID_DELETE_BP,
};

enum ItemColumns
{
	ColumnEnabled = 0,
	ColumnAddress,
	ColumnType,
	ColumnComment,
};

BreakpointWindow::BreakpointWindow(DebuggerWindow2& parent, const wxPoint& main_position, const wxSize& main_size)
	: wxFrame(&parent, wxID_ANY, _("Breakpoints"), wxDefaultPosition, wxSize(420, 250), wxSYSTEM_MENU | wxCAPTION | wxCLIP_CHILDREN | wxRESIZE_BORDER | wxFRAME_FLOAT_ON_PARENT)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	this->wxWindowBase::SetBackgroundColour(*wxWHITE);

	wxBoxSizer* main_sizer = new wxBoxSizer(wxVERTICAL);

	m_breakpoints = new wxDataViewListCtrl(this, wxID_ANY);

	m_breakpoints->AppendToggleColumn(_("On"), wxDATAVIEW_CELL_ACTIVATABLE, 32);
	m_breakpoints->AppendTextColumn(_("Address"), wxDATAVIEW_CELL_INERT, 75);
	m_breakpoints->AppendTextColumn(_("Type"), wxDATAVIEW_CELL_INERT, 42);
	m_breakpoints->AppendTextColumn(_("Comment"), wxDATAVIEW_CELL_EDITABLE, 250);

	main_sizer->Add(m_breakpoints, 1, wxEXPAND);

	this->SetSizer(main_sizer);
	this->wxWindowBase::Layout();

	this->Centre(wxBOTH);

	if (parent.GetConfig().data().pin_to_main)
		OnMainMove(main_position, main_size);

	m_breakpoints->Bind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &BreakpointWindow::OnBreakpointToggled, this);
	m_breakpoints->Bind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &BreakpointWindow::OnEditingDone, this);
	m_breakpoints->Bind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &BreakpointWindow::OnContextMenu, this);
	m_breakpoints->Bind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &BreakpointWindow::OnItemActivated, this);

	OnUpdateView();
}

BreakpointWindow::~BreakpointWindow()
{
	m_breakpoints->Unbind(wxEVT_DATAVIEW_ITEM_VALUE_CHANGED, &BreakpointWindow::OnBreakpointToggled, this);
	m_breakpoints->Unbind(wxEVT_DATAVIEW_ITEM_EDITING_DONE, &BreakpointWindow::OnEditingDone, this);
	m_breakpoints->Unbind(wxEVT_DATAVIEW_ITEM_CONTEXT_MENU, &BreakpointWindow::OnContextMenu, this);
	m_breakpoints->Unbind(wxEVT_DATAVIEW_ITEM_ACTIVATED, &BreakpointWindow::OnItemActivated, this);
}

void BreakpointWindow::OnMainMove(const wxPoint& main_position, const wxSize& main_size)
{
	wxSize size(420, 250);
	this->SetSize(size);

	wxPoint position = main_position;
	position.x -= 420;
	position.y += main_size.y - 250;
	this->SetPosition(position);
}

void BreakpointWindow::OnUpdateView()
{
	auto locker = wxWindowUpdateLocker(this);

	m_breakpoints->DeleteAllItems();

	if (!debuggerState.breakpoints.empty())
	{
		uint32_t i = 0;
		for (const auto bpBase : debuggerState.breakpoints)
		{
			DebuggerBreakpoint* bp = bpBase;
			while (bp)
			{
				const char* typeName = "UKN";
				if (bp->bpType == DEBUGGER_BP_T_NORMAL)
					typeName = "X";
				else if (bp->bpType == DEBUGGER_BP_T_LOGGING)
					typeName = "LOG";
				else if (bp->bpType == DEBUGGER_BP_T_ONE_SHOT)
					typeName = "XS";
				else if (bp->bpType == DEBUGGER_BP_T_MEMORY_READ)
					typeName = "R";
				else if (bp->bpType == DEBUGGER_BP_T_MEMORY_WRITE)
					typeName = "W";

				m_breakpoints->AppendItem({bp->enabled, wxString::Format("%08x", bp->address), typeName, bp->comment}, (wxUIntPtr)bp);

				bp = bp->next;
			}
		}
	}
}

void BreakpointWindow::OnGameLoaded()
{
	OnUpdateView();
}

void BreakpointWindow::OnBreakpointToggled(wxDataViewEvent& event)
{
	const wxDataViewItem item = event.GetItem();
	if (item && event.GetColumn() == ColumnEnabled)
	{
		const bool state = m_breakpoints->GetToggleValue(m_breakpoints->ItemToRow(item), event.GetColumn());
		wxString line = m_breakpoints->GetTextValue(m_breakpoints->ItemToRow(item), ColumnAddress);
		DebuggerBreakpoint* bp = (DebuggerBreakpoint*)m_breakpoints->GetItemData(item);
		const uint32 address = std::stoul(line.ToStdString(), nullptr, 16);
		debugger_toggleBreakpoint(address, state, bp);
	}
}

void BreakpointWindow::OnItemActivated(wxDataViewEvent& event)
{
	auto item = event.GetItem();
	if (!item)
		return;

	if (event.GetColumn() == ColumnAddress)
	{
		const auto text = m_breakpoints->GetTextValue(m_breakpoints->ItemToRow(item), event.GetColumn());
		const auto address = std::stoul(text.ToStdString(), nullptr, 16);
		debuggerState.debugSession.instructionPointer = address;
		debuggerWindow_moveIP();
	}
	else if (false && event.GetColumn() == ColumnComment)
	{
		auto bp = (DebuggerBreakpoint*)m_breakpoints->GetItemData(item);

		const wxString dialogTitle = bp->bpType == DEBUGGER_BP_T_LOGGING ? _("Enter a new logging message") : _("Enter a new comment");
		const wxString dialogMessage = bp->bpType == DEBUGGER_BP_T_LOGGING ? _("Set logging message when code at address %08x is ran.\nUse placeholders like {r3} or {f3} to log register values") : _("Set comment for breakpoint at address %08x");
		wxTextEntryDialog set_comment_dialog(this, wxString::Format(dialogMessage, bp->address), dialogTitle, bp->comment);
		if (set_comment_dialog.ShowModal() == wxID_OK)
		{
			bp->comment = set_comment_dialog.GetValue();
			m_breakpoints->SetTextValue(set_comment_dialog.GetValue(), m_breakpoints->ItemToRow(item), ColumnComment);
		}
	}
}

void BreakpointWindow::OnEditingDone(wxDataViewEvent& event)
{
	wxDataViewItem item = event.GetItem();

	if (!item || event.IsEditCancelled())
		return;

	auto bp = (DebuggerBreakpoint*)m_breakpoints->GetItemData(item);
	bp->comment = event.GetValue().GetString();
}

void BreakpointWindow::OnContextMenu(wxDataViewEvent& event)
{
	wxDataViewItem item = event.GetItem();
	if (!item)
	{
		wxMenu menu;
		menu.Append(MENU_ID_CREATE_CODE_BP_EXECUTION, _("Create execution breakpoint"));
		menu.Append(MENU_ID_CREATE_CODE_BP_LOGGING, _("Create logging breakpoint"));
		menu.Append(MENU_ID_CREATE_MEM_BP_READ, _("Create memory breakpoint (read)"));
		menu.Append(MENU_ID_CREATE_MEM_BP_WRITE, _("Create memory breakpoint (write)"));

		menu.Bind(wxEVT_COMMAND_MENU_SELECTED, &BreakpointWindow::OnContextMenuClick, this);
		PopupMenu(&menu);
	}
	else
	{
		m_breakpoints->SelectRow(m_breakpoints->ItemToRow(item));

		wxMenu menu;
		menu.Append(MENU_ID_DELETE_BP, _("Delete breakpoint"));

		menu.Bind(wxEVT_COMMAND_MENU_SELECTED, &BreakpointWindow::OnContextMenuClickSelected, this);
		PopupMenu(&menu);
	}
}

void BreakpointWindow::OnContextMenuClickSelected(wxCommandEvent& evt)
{
	if (evt.GetId() == MENU_ID_DELETE_BP)
	{
		auto item = m_breakpoints->GetSelection();
		if (item)
		{
			auto bp = (DebuggerBreakpoint*)m_breakpoints->GetItemData(item);
			debugger_deleteBreakpoint(bp);

			wxCommandEvent evt(wxEVT_BREAKPOINT_CHANGE);
			wxPostEvent(this->m_parent, evt);
		}
	}
}

void BreakpointWindow::OnContextMenuClick(wxCommandEvent& evt)
{
	wxTextEntryDialog goto_dialog(this, _("Enter a memory address"), _("Set breakpoint"), wxEmptyString);
	if (goto_dialog.ShowModal() == wxID_OK)
	{
		ExpressionParser parser;

		auto value = goto_dialog.GetValue().ToStdString();
		std::transform(value.begin(), value.end(), value.begin(), tolower);

		uint32_t newBreakpointAddress = 0;
		try
		{
			debugger_addParserSymbols(parser);
			newBreakpointAddress = parser.IsConstantExpression("0x" + value) ? (uint32)parser.Evaluate("0x" + value) : (uint32)parser.Evaluate(value);
		} catch (const std::exception& ex)
		{
			wxMessageBox(ex.what(), _("Error"), wxOK | wxCENTRE | wxICON_ERROR, this);
			return;
		}

		switch (evt.GetId())
		{
		case MENU_ID_CREATE_CODE_BP_EXECUTION:
			debugger_createCodeBreakpoint(newBreakpointAddress, DEBUGGER_BP_T_NORMAL);
			break;
		case MENU_ID_CREATE_CODE_BP_LOGGING:
			debugger_createCodeBreakpoint(newBreakpointAddress, DEBUGGER_BP_T_LOGGING);
			break;
		case MENU_ID_CREATE_MEM_BP_READ:
			debugger_createMemoryBreakpoint(newBreakpointAddress, true, false);
			break;
		case MENU_ID_CREATE_MEM_BP_WRITE:
			debugger_createMemoryBreakpoint(newBreakpointAddress, false, true);
			break;
		}

		this->OnUpdateView();
	}
}
