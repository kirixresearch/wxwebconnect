/////////////////////////////////////////////////////////////////////////////
// Name:        xh_webcontrol.cpp
// Purpose:     XRC resource handler for wxWebControl
// Author:      Ben Morgan
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_XRC

#include <wx/wx.h>
#include "xh_webcontrol.h"
#include "webcontrol.h"

// Register with wxWindows' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(wxWebControlXmlHandler, wxXmlResourceHandler)

// Constructor.
wxWebControlXmlHandler::wxWebControlXmlHandler()
{
    AddWindowStyles();
}

// Creates the control and returns a pointer to it.
wxObject *wxWebControlXmlHandler::DoCreateResource()
{
    XRC_MAKE_INSTANCE(control, wxWebControl)

    control->Create(m_parentAsWindow, GetID(),
        GetPosition(), GetSize());

    if (HasParam(wxT("url")))
    {
        wxString url = GetParamValue(wxT("url"));
		control->OpenURI(url);
    }

    else if (HasParam(wxT("htmlcode")))
    {
		// Dunno what makes for a good default URI; 
		// this should do for now though...
        control->SetContent(wxT("about:blank"), GetText(wxT("htmlcode")));
    }

    SetupWindow(control);

    return control;
}

// Returns true if we know how to create a control for the given node.
bool wxWebControlXmlHandler::CanHandle(wxXmlNode *node)
{
    return IsOfClass(node, wxT("wxWebControl"));
}

#endif // wxUSE_XRC
