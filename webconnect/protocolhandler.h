#ifndef __WXWEBCONNECT_PROTOCOL_HANDLER_H
#define __WXWEBCONNECT_PROTOCOL_HANDLER_H

/* This enumeration comes (in slightly changed form) from nsIProtocolHandler.idl.  Comments explaining these options are there. */
enum wxProtocolHandlerURIOptions    {
    URI_STD = 0,
    URI_NORELATIVE = (1<<0),
    URI_NOAUTH = (1<<1),
    URI_INHERITS_SECURITY_CONTEXT = (1<<4),
    URI_FORBIDS_AUTOMATIC_DOCUMENT_REPLACEMENT = (1<<5),
    URI_LOADABLE_BY_ANYONE = (1<<6),
    URI_DANGEROUS_TO_LOAD = (1<<7),
    URI_IS_UI_RESOURCE = (1<<8),
    URI_IS_LOCAL_FILE = (1<<9),
    URI_NON_PERSISTABLE = (1<<10),
    URI_DOES_NOT_RETURN_DATA = (1<<11),
    URI_IS_LOCAL_RESOURCE = (1<<12),
    URI_OPENING_EXECUTES_SCRIPT = (1<<13)
};

class wxProtocolHandler {
public:
    wxProtocolHandler();
    virtual ~wxProtocolHandler();

    /* XXX: Is there a better way to handle Int32s than this? */
    virtual int GetDefaultPort();

    virtual int GetProtocolFlags();

    virtual bool AllowPort(int port, const char* scheme);

    virtual wxString GetContent(const wxString& url);

    virtual wxString GetContentType(const wxString& url);
};

bool RegisterProtocol(const wxString &scheme, wxProtocolHandler *handler);

#endif
