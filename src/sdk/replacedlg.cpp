/*
* This file is part of Code::Blocks Studio, an open-source cross-platform IDE
* Copyright (C) 2003  Yiannis An. Mandravellos
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
* Contact e-mail: Yiannis An. Mandravellos <mandrav@codeblocks.org>
* Program URL   : http://www.codeblocks.org
*
* $Revision$
* $Id$
* $HeadURL$
*/

#ifdef CB_PRECOMP
    #include "sdk_precomp.h"
#else
    #include "globals.h"
    #include "configmanager.h"

    #include <wx/xrc/xmlres.h>
    #include <wx/intl.h>
    #include <wx/combobox.h>
    #include <wx/checkbox.h>
    #include <wx/button.h>
#endif

#include "replacedlg.h"

#include <wx/radiobox.h>

#define CONF_GROUP _T("/replace_options")

BEGIN_EVENT_TABLE(ReplaceDlg, wxDialog)
	EVT_CHECKBOX(XRCID("chkRegEx"), 	ReplaceDlg::OnRegEx)
END_EVENT_TABLE()

ReplaceDlg::ReplaceDlg(wxWindow* parent, const wxString& initial, bool hasSelection)
	: FindReplaceBase(parent, initial, hasSelection)
{
	wxXmlResource::Get()->LoadDialog(this, parent, _T("dlgReplace"));
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	// load last searches
	FillComboWithLastValues(XRCCTRL(*this, "cmbFind", wxComboBox), CONF_GROUP _T("/last"));
	FillComboWithLastValues(XRCCTRL(*this, "cmbReplace", wxComboBox), CONF_GROUP _T("/lastReplace"));

	// find options
	XRCCTRL(*this, "cmbFind", wxComboBox)->SetValue(initial);
	XRCCTRL(*this, "chkWholeWord", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_word"), false));
	XRCCTRL(*this, "chkStartWord", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/start_word"), false));
	XRCCTRL(*this, "chkMatchCase", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/match_case"), false));
	XRCCTRL(*this, "chkRegEx", wxCheckBox)->SetValue(cfg->ReadBool(CONF_GROUP _T("/regex"), false));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/direction"), 1));
	XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx", wxCheckBox)->GetValue()); // if regex, only forward searches
	XRCCTRL(*this, "rbOrigin", wxRadioBox)->SetSelection(cfg->ReadInt(CONF_GROUP _T("/origin"), 0));
	XRCCTRL(*this, "rbScope", wxRadioBox)->SetSelection(hasSelection);
	XRCCTRL(*this, "rbScope", wxRadioBox)->Enable(hasSelection);

	XRCCTRL(*this, "cmbFind", wxComboBox)->SetFocus();
}

ReplaceDlg::~ReplaceDlg()
{
    ConfigManager* cfg = Manager::Get()->GetConfigManager(_T("editor"));

	// save last searches (up to 10)
	SaveComboValues(XRCCTRL(*this, "cmbFind", wxComboBox), CONF_GROUP _T("/last"));
	SaveComboValues(XRCCTRL(*this, "cmbReplace", wxComboBox), CONF_GROUP _T("/lastReplace"));

	// find options
	cfg->Write(CONF_GROUP _T("/match_word"), XRCCTRL(*this, "chkWholeWord", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/start_word"), XRCCTRL(*this, "chkStartWord", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/match_case"), XRCCTRL(*this, "chkMatchCase", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/regex"), XRCCTRL(*this, "chkRegEx", wxCheckBox)->GetValue());
	cfg->Write(CONF_GROUP _T("/direction"), XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection());
	cfg->Write(CONF_GROUP _T("/origin"), XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection());
}

void ReplaceDlg::FillComboWithLastValues(wxComboBox* combo, const wxString& configKey)
{
	wxArrayString values = GetArrayFromString(Manager::Get()->GetConfigManager(_T("editor"))->Read(configKey, wxEmptyString), DEFAULT_ARRAY_SEP, false);
	for (unsigned int i = 0; i < values.GetCount(); ++i)
	{
		if (!values[i].IsEmpty())
			combo->Append(values[i]);
	}
}

void ReplaceDlg::SaveComboValues(wxComboBox* combo, const wxString& configKey)
{
	wxArrayString values;
	for (int i = 0; (i < combo->GetCount()) && (i < 10); ++i)
	{
		if (!combo->GetString(i).IsEmpty())
			values.Add(combo->GetString(i));
	}
	wxString find = combo->GetValue();
	if (combo->FindString(find) == -1)
		values.Insert(find, 0);
	wxString last = GetStringFromArray(values);
	Manager::Get()->GetConfigManager(_T("editor"))->Write(configKey, last);
}

wxString ReplaceDlg::GetFindString() const
{
	return XRCCTRL(*this, "cmbFind", wxComboBox)->GetValue();
}

wxString ReplaceDlg::GetReplaceString() const
{
	return XRCCTRL(*this, "cmbReplace", wxComboBox)->GetValue();
}

bool ReplaceDlg::GetDeleteOldSearches() const
{
	return true;
}

bool ReplaceDlg::GetMatchWord() const
{
	return XRCCTRL(*this, "chkWholeWord", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetStartWord() const
{
	return XRCCTRL(*this, "chkStartWord", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetMatchCase() const
{
	return XRCCTRL(*this, "chkMatchCase", wxCheckBox)->GetValue();
}

bool ReplaceDlg::GetRegEx() const
{
	return XRCCTRL(*this, "chkRegEx", wxCheckBox)->GetValue();
}

int ReplaceDlg::GetDirection() const
{
	return XRCCTRL(*this, "rbDirection", wxRadioBox)->GetSelection();
}

int ReplaceDlg::GetOrigin() const
{
	return XRCCTRL(*this, "rbOrigin", wxRadioBox)->GetSelection();
}

int ReplaceDlg::GetScope() const
{
	return XRCCTRL(*this, "rbScope", wxRadioBox)->GetSelection();
}

// events

void ReplaceDlg::OnRegEx(wxCommandEvent& event)
{
	XRCCTRL(*this, "rbDirection", wxRadioBox)->Enable(!XRCCTRL(*this, "chkRegEx", wxCheckBox)->GetValue());
}
