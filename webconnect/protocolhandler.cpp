#include <string>
#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>
#include <wx/file.h>
#include "webframe.h"
#include "nsinclude.h"
#include "protocolhandler.h"

class nsProtocolHandlerImpl: public nsIProtocolHandler
{
public:
    NS_DECL_ISUPPORTS
    NS_DECL_NSIPROTOCOLHANDLER

    nsProtocolHandlerImpl(wxString *scheme, wxProtocolHandler* handler);

private:
    ~nsProtocolHandlerImpl();
    wxProtocolHandler* m_handler;
    nsCString m_scheme;

protected:
    /* additional members */
};


NS_IMPL_ISUPPORTS1(nsProtocolHandlerImpl, nsIProtocolHandler)

nsProtocolHandlerImpl::nsProtocolHandlerImpl(wxString *scheme, wxProtocolHandler* handler)
{
    wx2ns(*scheme, m_scheme);
    m_handler = handler;
}

nsProtocolHandlerImpl::~nsProtocolHandlerImpl()
{
  /* destructor code */
}

/* readonly attribute ACString scheme; */
NS_IMETHODIMP nsProtocolHandlerImpl::GetScheme(nsACString & aScheme)
{
    aScheme = m_scheme;
    return NS_OK;
}

NS_IMETHODIMP nsProtocolHandlerImpl::GetDefaultPort(PRInt32 *aDefaultPort)
{
    *aDefaultPort = m_handler->GetDefaultPort();
    return NS_OK;
}

NS_IMETHODIMP nsProtocolHandlerImpl::GetProtocolFlags(PRUint32 *aProtocolFlags)
{
    *aProtocolFlags = m_handler->GetProtocolFlags();
    return NS_OK;
}

NS_IMETHODIMP nsProtocolHandlerImpl::NewURI(const nsACString & aSpec, const char *aOriginCharset, nsIURI *aBaseURI, nsIURI **_retval)
{
    ns_smartptr<nsIStandardURL> standard_url(nsCreateInstance("@mozilla.org/network/standard-url;1"));
    standard_url->Init(nsIStandardURL::URLTYPE_STANDARD, 80, aSpec, aOriginCharset, aBaseURI);

    ns_smartptr<nsIURI> result(standard_url);

    *_retval = result;
	NS_ADDREF(*_retval);
    return NS_OK;
}

NS_IMETHODIMP nsProtocolHandlerImpl::NewChannel(nsIURI *aURI, nsIChannel **_retval)
{
    ns_smartptr<nsIServiceManager> service_mgr;
    nsresult res;
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return res;
    
    ns_smartptr<nsIInputStreamChannel> input_stream_channel(nsCreateInstance("@mozilla.org/network/input-stream-channel;1"));
    ns_smartptr<nsIChannel> channel(input_stream_channel);

    nsCString nsspec;
    res = aURI->GetSpec(nsspec);
    if (NS_FAILED(res))
        return res;

    wxString wxSpec = ns2wx(nsspec);

    nsCString nsContentType;
    nsString nsContent;

    wxString contentType = m_handler->GetContentType(wxSpec);
    wxString content = m_handler->GetContent(wxSpec);
    wx2ns(contentType, nsContentType);
    wx2ns(content, nsContent);

    channel->SetContentType(nsContentType);
    /* XXX: Is UTF-8 the best encoding all the time? */
    channel->SetContentCharset(NS_LITERAL_CSTRING("utf-8"));
    input_stream_channel->SetURI(aURI);
    
    ns_smartptr<nsIScriptableUnicodeConverter> unicode_converter(nsCreateInstance("@mozilla.org/intl/scriptableunicodeconverter"));
    unicode_converter->SetCharset("UTF-8");
    nsIInputStream* input_stream;
    res = unicode_converter->ConvertToInputStream(nsContent, &input_stream);
    if (NS_FAILED(res))
        return res;
    input_stream_channel->SetContentStream(input_stream);
    
    *_retval = channel;
	NS_ADDREF(*_retval);
    return NS_OK;
}

NS_IMETHODIMP nsProtocolHandlerImpl::AllowPort(PRInt32 port, const char *scheme, PRBool *_retval)
{
    *_retval = m_handler->AllowPort(port, scheme);
    return NS_OK;
}


///////////////////////////////////////////////////////////////////////////////
//  ProtocolHandlerFactory class implementation
///////////////////////////////////////////////////////////////////////////////


class ProtocolHandlerFactory : public nsIFactory
{
public:
    NS_DECL_ISUPPORTS
    
    ProtocolHandlerFactory(const wxString &scheme, wxProtocolHandler *handler)
    {
        NS_INIT_ISUPPORTS();

        m_scheme = new wxString(scheme);
        m_handler = handler;
    }
    
    NS_IMETHOD CreateInstance(nsISupports* outer,
                              const nsIID& iid,
                              void** result)
    {
        nsresult res;
        
        if (!result)
            return NS_ERROR_NULL_POINTER;
            
        if (outer)
            return NS_ERROR_NO_AGGREGATION;
        
        nsProtocolHandlerImpl *obj = new nsProtocolHandlerImpl(m_scheme, m_handler);
        if (!obj)
            return NS_ERROR_OUT_OF_MEMORY;
            
        obj->AddRef();
        res = obj->QueryInterface(iid, result);
        // XXX: What does it mean to release this?
        obj->Release();
        
        return res;
    }
    
    NS_IMETHOD LockFactory(PRBool lock)
    {
        return NS_OK;
    }

private:
    wxProtocolHandler *m_handler;
    wxString *m_scheme;
};

NS_IMPL_ISUPPORTS1(ProtocolHandlerFactory, nsIFactory);


void CreateProtocolHandlerFactory(const wxString &scheme, wxProtocolHandler *handler, nsIFactory** result)
{
    ProtocolHandlerFactory* obj = new ProtocolHandlerFactory(scheme, handler);
    obj->AddRef();
    *result = obj;
}


bool RegisterProtocol(const wxString &scheme, wxProtocolHandler *handler) {
    nsresult res;
    ns_smartptr<nsIComponentRegistrar> comp_reg;
    ns_smartptr<nsIServiceManager> service_mgr;
    ns_smartptr<nsIFactory> protocol_handler_factory;

    res = NS_GetComponentRegistrar(&comp_reg.p);
    if (NS_FAILED(res))
        return false;

    // XXX: Memory leak?
    wxString *contractID = new wxString(wxT(NS_NETWORK_PROTOCOL_CONTRACTID_PREFIX) + scheme);
    wxString *className = new wxString(wxT("WebConnect Protocol - ") + scheme);
    res = NS_GetServiceManager(&service_mgr.p);
    if (NS_FAILED(res))
        return false;
    
    ns_smartptr<nsIUUIDGenerator> uuid_generator(nsCreateInstance("@mozilla.org/uuid-generator;1"));
    // Generate a new UUID.
    nsCID *protocol_cid;
    res = uuid_generator->GenerateUUID(&protocol_cid);
    if (NS_FAILED(res))
        return false;

    // Create a factory.
    CreateProtocolHandlerFactory(scheme, handler, &protocol_handler_factory.p);
    res = comp_reg->RegisterFactory(*protocol_cid,
                                    className->ToAscii(),
                                    contractID->ToAscii(),
                                    protocol_handler_factory);

    return true;
}

wxProtocolHandler::wxProtocolHandler()  {
}

wxProtocolHandler::~wxProtocolHandler()  {
}

int wxProtocolHandler::GetDefaultPort()    {
    return -1;
}

int wxProtocolHandler::GetProtocolFlags()  {
    return URI_NOAUTH | URI_NORELATIVE | URI_IS_UI_RESOURCE;
}

/* It makes sense to be false by default as this support is intended more in application protocols. */
bool wxProtocolHandler::AllowPort(int port, const char* scheme)    {
    return false;
}

wxString wxProtocolHandler::GetContent(const wxString& url) {
    return wxT("");
}

wxString wxProtocolHandler::GetContentType(const wxString& url) {
    return wxT("text/html");
}
