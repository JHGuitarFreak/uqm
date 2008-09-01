#include <coecntrl.h>
#include <aknappui.h>
#include <aknapp.h>
#include <akndoc.h>
#include <sdlepocapi.h>
#include <aknnotewrappers.h>
#include <eikstart.h>
#include <badesca.h>
#include <stdio.h>

const TUid KUidSdlApp={ 0xA000A0C3 };


class CSDLObserver : public CBase, public MSDLObserver
{
public:
    TInt SdlEvent(TInt aEvent, TInt aParam);
    TInt SdlThreadEvent(TInt aEvent, TInt aParam);
};

class MExitWait
    {
    public:
        virtual void DoExit(TInt aErr) = 0;
    };   

class CExitWait : public CActive
    {
    public:
        CExitWait(MExitWait& aWait);
        void Start();
        ~CExitWait();
    private:
        void RunL();
        void DoCancel();
    private:
        MExitWait& iWait;
        TRequestStatus* iStatusPtr;
    };

class CSDLWin : public CCoeControl
    {
    public:
        void ConstructL(const TRect& aRect);
        RWindow& GetWindow() const;
        void SetNoDraw();
    private:
        void Draw(const TRect& aRect) const;
    };  
    
class CSdlApplication : public CAknApplication
    {
private:
    // from CApaApplication
    CApaDocument* CreateDocumentL();
    TUid AppDllUid() const;
    };
    
    
class CSdlAppDocument : public CAknDocument
    {
public:
    CSdlAppDocument(CEikApplication& aApp): CAknDocument(aApp) { }
private:
    CEikAppUi* CreateAppUiL();
    };
    
            
class CSdlAppUi : public CAknAppUi, public MExitWait
    {
public:
    void ConstructL();
    ~CSdlAppUi();
private:
    void HandleCommandL(TInt aCommand);
    void DoExit(TInt aErr);
    void HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination);
    void HandleResourceChangeL(TInt aType);
private:
    CExitWait* iWait;
    CSDLWin* iSDLWin;
    CSDL* iSdl;
    CSDLObserver* iSdlObserver;
    TBool iExit;
    };  


CExitWait::CExitWait(MExitWait& aWait) : CActive(CActive::EPriorityStandard), iWait(aWait)
    {
    CActiveScheduler::Add(this);
    }
    
CExitWait::~CExitWait()
    {
    Cancel();
    }
 
void CExitWait::RunL()
    {
    if(iStatusPtr != NULL )
        iWait.DoExit(iStatus.Int());
    }
    
void CExitWait::DoCancel()
    {
    if(iStatusPtr != NULL )
        User::RequestComplete(iStatusPtr , KErrCancel);
    }
    
void CExitWait::Start()
    {
    SetActive();
    iStatusPtr = &iStatus;
    }

void CSDLWin:: ConstructL(const TRect& aRect)   
    {
    CreateWindowL();
    SetRect(aRect);
    ActivateL();
    }

    
RWindow& CSDLWin::GetWindow() const
    {
    return Window();
    }
    

void CSDLWin::Draw(const TRect& /*aRect*/) const
    {
    CWindowGc& gc = SystemGc();
    gc.SetPenStyle(CGraphicsContext::ESolidPen);
    gc.SetPenColor(KRgbBlack);
    gc.SetBrushStyle(CGraphicsContext::ESolidBrush);
    gc.SetBrushColor(0x000000);
    gc.DrawRect(Rect());
    }   
    
void CSdlAppUi::ConstructL()    
    {
    BaseConstructL(ENoScreenFurniture | EAppOrientationLandscape);
        
    iSDLWin = new (ELeave) CSDLWin;
    iSDLWin->ConstructL(ApplicationRect());
            
    iWait = new (ELeave) CExitWait(*this);
    
    iSdl = CSDL::NewL(CSDL::ENoFlags);
    iSdlObserver = new (ELeave) CSDLObserver;
                
    iSdl->SetContainerWindowL(
                    iSDLWin->GetWindow(), 
                    iEikonEnv->WsSession(),
                    *iEikonEnv->ScreenDevice());
    
    iSdl->SetObserver(iSdlObserver);
    
    CDesC8ArrayFlat* args = new (ELeave)CDesC8ArrayFlat(10);        
    iSdl->CallMainL(iWait->iStatus, *args, 75000);
    delete args;
    
    iWait->Start();     
    }

void CSdlAppUi::HandleCommandL(TInt aCommand)
    {
    switch(aCommand)
        {
        case EAknCmdExit:
        case EAknSoftkeyExit:
        case EEikCmdExit:
            iExit = ETrue;
            if(iWait == NULL || !iWait->IsActive())
                Exit(); 
            break;
        default:
            break;
        }
    }
    
void CSdlAppUi::DoExit(TInt aErr)
    {
    delete iSdl;
    iSdl = NULL;
    
    if(iExit)
        Exit();
    }
    
void CSdlAppUi::HandleWsEventL(const TWsEvent& aEvent, CCoeControl* aDestination)
    {
    if(iSdl != NULL)
        iSdl->AppendWsEvent(aEvent);
    CAknAppUi::HandleWsEventL(aEvent, aDestination);
    }
    
void CSdlAppUi::HandleResourceChangeL(TInt aType)
    {
    CAknAppUi::HandleResourceChangeL(aType);
    
    if(aType == KEikDynamicLayoutVariantSwitch)
        {
        iSDLWin->SetRect(ApplicationRect());
        if (iSdl)
            {
            iSdl->SetContainerWindowL(
                        iSDLWin->GetWindow(),
                        iEikonEnv->WsSession(),
                        *iEikonEnv->ScreenDevice());
            }                       
        }
    }
        
CSdlAppUi::~CSdlAppUi()
    {
    if(iWait != NULL)
        iWait->Cancel();
    delete iSdl;
    delete iWait;
    delete iSDLWin;
    delete iSdlObserver;
    }

CEikAppUi* CSdlAppDocument::CreateAppUiL()
    {
    return new(ELeave) CSdlAppUi();
    }   
    
TUid CSdlApplication::AppDllUid() const
    {
    return KUidSdlApp;
    }   
    

CApaDocument* CSdlApplication::CreateDocumentL()
    {
    CSdlAppDocument* document = new (ELeave) CSdlAppDocument(*this);
    return document;
    }
  
LOCAL_C CApaApplication* NewApplication()
    {
    return new CSdlApplication;
    }

GLDEF_C TInt E32Main()
    {
    return EikStart::RunApplication(NewApplication);
    }
    
TInt CSDLObserver::SdlEvent(TInt aEvent, TInt aParam)
{
    return 0;
}

TInt CSDLObserver::SdlThreadEvent(TInt aEvent, TInt aParam)
{
    return 0;
}