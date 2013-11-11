/////////////////////////////////////////////////////////////////////////////
// Name:        xh_webcontrol.h
// Purpose:     XRC resource handler for wxWebControl
// Author:      Ben Morgan
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XH_WEBCONTROL_H
#define _WX_XH_WEBCONTROL_H

#include "wx/xrc/xmlres.h"

//
// XML resource handler for the wxWebControl class in wxContrib.
//

class wxWebControlXmlHandler : public wxXmlResourceHandler
{
public:

    // Constructor.
    wxWebControlXmlHandler();

    // Creates the control and returns a pointer to it.
    virtual wxObject *DoCreateResource();

    // Returns true if we know how to create a control for the given node.
    virtual bool CanHandle(wxXmlNode *node);

    // Register with wxWindows' dynamic class subsystem.
    DECLARE_DYNAMIC_CLASS(wxWebControlXmlHandler)
};

#endif

