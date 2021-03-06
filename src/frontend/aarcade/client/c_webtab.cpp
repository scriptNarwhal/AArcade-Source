#include "cbase.h"

#include "c_webtab.h"
#include "c_anarchymanager.h"
#include "pixelwriter.h"

//#include "aa_globals.h"
//#include "Filesystem.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

C_WebTab::C_WebTab(std::string url, std::string id, bool bAlpha)
{
	m_iNumImagesLoading = -1;
	m_iMaxImagesLoading = 10;

	DevMsg("WebTab: Constructor\n");
	m_iState = 0;

	m_id = id;
	m_initialUrl = url;
	m_iLastRenderFrame = -1;
	m_iState = 1;	// initializing
	m_fMouseX = 0.5;
	m_fMouseY = 0.5;

	// create the texture (each WebTab has its own texture)
	std::string textureName = "websurface_";
	textureName += m_id;

	int iWidth = g_pAnarchyManager->GetWebManager()->GetWebSurfaceWidth();
	int iHeight = g_pAnarchyManager->GetWebManager()->GetWebSurfaceHeight();

	if (m_id != "images")
	{
		if (bAlpha)
			m_pTexture = g_pMaterialSystem->CreateProceduralTexture(textureName.c_str(), TEXTURE_GROUP_VGUI, iWidth, iHeight, IMAGE_FORMAT_BGRA8888, 1);
		else
			m_pTexture = g_pMaterialSystem->CreateProceduralTexture(textureName.c_str(), TEXTURE_GROUP_VGUI, iWidth, iHeight, IMAGE_FORMAT_BGR888, 1);

		// get the regen and assign it
		CWebSurfaceRegen* pRegen = g_pAnarchyManager->GetWebManager()->GetOrCreateWebSurfaceRegen();
		m_pTexture->SetTextureRegenerator(pRegen);
	}
	else
		m_pTexture = null;

	// create the web view
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->CreateWebView(this);
}

C_WebTab::~C_WebTab()
{
	DevMsg("WebTab: Destructor\n");

	//if (m_pTexture)
	//	m_pTexture->SetTextureRegenerator(null);

	//pTexture->SetTextureRegenerator(null);

	//pTexture->DecrementReferenceCount();
	//pTexture->DeleteIfUnreferenced();

	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->ReleaseWebView(this);

	if (m_pTexture)
	{
		DevMsg("Destroying web tab texture.\n");
		//if (m_id != "images")
		//	m_pTexture->SetTextureRegenerator(null);
//		m_pTexture->DecrementReferenceCount();
//		m_pTexture->DeleteIfUnreferenced();
	}

	DevMsg("Done destroying web tab.\n");
}

void C_WebTab::OnProxyBind(C_BaseEntity* pBaseEntity)
{
	if (m_id == "images")
		return;

	/*
	if ( pBaseEntity )
		DevMsg("WebTab: OnProxyBind: %i\n", pBaseEntity->entindex());
	else
		DevMsg("WebTab: OnProxyBind\n");
	*/

	if (m_iLastRenderFrame < gpGlobals->framecount)
	{
//		if (g_pAnarchyManager->GetWebManager()->IsPriorityWebTab(this))
//			g_pAnarchyManager->GetWebManager()->IncrementVisiblePriorityWebTabsCurrentFrame();
//		else
		if (!g_pAnarchyManager->GetWebManager()->IsPriorityWebTab(this))
			g_pAnarchyManager->GetWebManager()->IncrementVisibleWebTabsCurrentFrame();

//		if ((g_pAnarchyManager->GetWebManager()->GetSelectedWebTab() != this || !g_pAnarchyManager->GetWebManager()->GetSelectedPriority()) && (g_pAnarchyManager->GetWebManager()->GetHudWebTab() != this || !g_pAnarchyManager->GetWebManager()->GetHudPriority()))
			//g_pAnarchyManager->GetWebManager()->IncrementVisibleWebTabsCurrentFrame();

		//if (g_pAnarchyManager->GetWebManager()->ShouldRender(this))
		// render the web tab to its texture
//		if ((g_pAnarchyManager->GetWebManager()->GetSelectedWebTab() == this && g_pAnarchyManager->GetWebManager()->GetSelectedPriority()) || (g_pAnarchyManager->GetWebManager()->GetHudWebTab() == this && g_pAnarchyManager->GetWebManager()->GetHudPriority()) || g_pAnarchyManager->GetWebManager()->ShouldRender(this))
		if (g_pAnarchyManager->GetWebManager()->ShouldRender(this))
			Render();
	}
}

void C_WebTab::Render()
{
	if (m_id == "images")
		return;
	//DevMsg("Rendering texture: %s\n", m_pTexture->GetName());
//	DevMsg("Render Web Tab: %s\n", this->GetTexture()->Ge>GetId().c_str());
	//DevMsg("WebTab: Render: %s on %i\n", m_id.c_str(), gpGlobals->framecount);
	g_pAnarchyManager->GetWebManager()->GetOrCreateWebSurfaceRegen()->SetWebTab(this);
	m_pTexture->Download();

	m_iLastRenderFrame = gpGlobals->framecount;

	//if ((g_pAnarchyManager->GetWebManager()->GetSelectedWebTab() != this || !g_pAnarchyManager->GetWebManager()->GetSelectedPriority()) && (g_pAnarchyManager->GetWebManager()->GetHudWebTab() != this || !g_pAnarchyManager->GetWebManager()->GetHudPriority()))
	if (g_pAnarchyManager->GetWebManager()->IsPriorityWebTab(this))
		g_pAnarchyManager->GetWebManager()->SetLastPriorityRenderedFrame(gpGlobals->framecount);
	else
		g_pAnarchyManager->GetWebManager()->SetLastRenderedFrame(gpGlobals->framecount);
}

void C_WebTab::RegenerateTextureBits(ITexture *pTexture, IVTFTexture *pVTFTexture, Rect_t *pSubRect)
{
//	DevMsg("WebTab: RegenerateTextureBits\n");
	// draw the web tab
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->RegenerateTextureBits(this, pTexture, pVTFTexture, pSubRect);
	/*
	// draw the mouse cursor
	if (m_fMouseX != 0.5 ||m_fMouseY != 0.5)
	{
		// the mouse position is between 0 and 1
		// translate the mouse position to actual pixel values
		int iMouseX = m_fMouseX * g_pAnarchyManager->GetWebManager()->GetWebSurfaceWidth();
		int iMouseY = m_fMouseY * g_pAnarchyManager->GetWebManager()->GetWebSurfaceHeight();

		CPixelWriter pixelWriter;
		pixelWriter.SetPixelMemory(pVTFTexture->Format(), pVTFTexture->ImageData(0, 0, 0), pVTFTexture->RowSizeInBytes(0));

		int xmax = pSubRect->x + pSubRect->width;
		int ymax = pSubRect->y + pSubRect->height;

		int mouseHeight = 20;
		for (int y = 0; y < mouseHeight && y < ymax; y++)
		{
			for (int x = 0; x < mouseHeight - y && x < xmax; x++)
			{
				pixelWriter.Seek(pSubRect->x + x + iMouseX, pSubRect->y + y + iMouseY);
				pixelWriter.WritePixel(0, 255, 0, 255);
			}
		}
	}
	*/
}

void C_WebTab::GetMousePos(float &fMouseX, float &fMouseY)
{
	fMouseX = m_fMouseX;
	fMouseY = m_fMouseY;
}

void C_WebTab::MouseMove(float fMouseX, float fMouseY)
{
	m_fMouseX = fMouseX;
	m_fMouseY = fMouseY;
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnMouseMove(this, m_fMouseX, m_fMouseY);
}

void C_WebTab::MouseWheel(int delta)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnMouseWheel(this, delta);
}

void C_WebTab::MousePress(vgui::MouseCode code)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnMousePress(this, code);
}

void C_WebTab::MouseRelease(vgui::MouseCode code)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnMouseRelease(this, code);
}

void C_WebTab::KeyCodePress(vgui::MouseCode code, bool bShiftState, bool bCtrlState, bool bAltState)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnKeyCodePressed(this, code, bShiftState, bCtrlState, bAltState);
}

void C_WebTab::KeyCodeRelease(vgui::MouseCode code, bool bShiftState, bool bCtrlState, bool bAltState)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->OnKeyCodeReleased(this, code, bShiftState, bCtrlState, bAltState);
}

void C_WebTab::SetUrl(std::string url)
{
	WebView* pWebView = g_pAnarchyManager->GetWebManager()->GetWebBrowser()->FindWebView(this);
	if (pWebView)
		pWebView->LoadURL(WebURL(WSLit(url.c_str())));
	else
		DevMsg("ERROR: cOULD NOT FIND WEB VIEW!\n\n");
}

bool C_WebTab::RequestLoadSimpleImage(std::string channel, std::string itemId)
{
	if (m_id != "images" || m_iNumImagesLoading == -1 || m_iNumImagesLoading >= m_iMaxImagesLoading)
		return false;

	// check if we are ready to accept a new image request
	std::vector<std::string> args;
	args.push_back(channel);
	args.push_back(itemId);	// these should also be remembered locally too, so we can load entire websites as images too.

	m_iNumImagesLoading++;
	DispatchJavaScriptMethod("imageLoader", "loadImage", args);
	return true;
}

void C_WebTab::OnSimpleImageReady(std::string channel, std::string itemId, std::string field, ITexture* pTexture)
{
	if (m_id != "images" )
		return;

	CWebSurfaceProxy::OnSimpleImageRendered(channel, itemId, field, pTexture);
	m_iNumImagesLoading--;

	if (pTexture)
	{
		std::vector<std::string> args;
		this->DispatchJavaScriptMethod("imageLoader", "onImageRender", args);
	}
	//m_bReadyForNextSimpleImage = true;
}

void C_WebTab::DispatchJavaScriptMethod(std::string objectName, std::string objectMethod, std::vector<std::string> methodArguments)
{
	g_pAnarchyManager->GetWebManager()->GetWebBrowser()->DispatchJavaScriptMethod(this, objectName, objectMethod, methodArguments);
}

void C_WebTab::DispatchJavaScriptMethodCalls()
{
	// do it in batches to reduce sync calls to the web view to O(1) instead of O(N) (assuming every method using the same JS object.)
	g_pAnarchyManager->GetWebManager()->DispatchJavaScriptMethods(this);
	m_javaScriptMethodCalls.clear();
}

void C_WebTab::SetHudTitle(std::string title)
{
	if (g_pAnarchyManager->GetWebManager()->GetHudWebTab() != this)
		return;

	JavaScriptMethodCall_t* pJavaScriptMethodCall = new JavaScriptMethodCall_t;
	pJavaScriptMethodCall->objectName = "arcadeHud";
	pJavaScriptMethodCall->methodName = "setHudeTitle";
	pJavaScriptMethodCall->methodArguments.push_back(title);
	m_javaScriptMethodCalls.push_back(pJavaScriptMethodCall);

	if (m_iState >= 2)
		this->DispatchJavaScriptMethodCalls();
}

void C_WebTab::AddHudLoadingMessage(std::string type, std::string text, std::string title, std::string id, std::string min, std::string max, std::string current, std::string callbackMethod)
{
	if (g_pAnarchyManager->GetWebManager()->GetHudWebTab() != this)
		return;

	JavaScriptMethodCall_t* pJavaScriptMethodCall = new JavaScriptMethodCall_t;
	pJavaScriptMethodCall->objectName = "arcadeHud";
	pJavaScriptMethodCall->methodName = "addHudLoadingMessage";
	pJavaScriptMethodCall->methodArguments.push_back(type);
	pJavaScriptMethodCall->methodArguments.push_back(text);
	pJavaScriptMethodCall->methodArguments.push_back(title);
	pJavaScriptMethodCall->methodArguments.push_back(id);
	pJavaScriptMethodCall->methodArguments.push_back(min);
	pJavaScriptMethodCall->methodArguments.push_back(max);
	pJavaScriptMethodCall->methodArguments.push_back(current);
	pJavaScriptMethodCall->methodArguments.push_back(callbackMethod);
	m_javaScriptMethodCalls.push_back(pJavaScriptMethodCall);

	if (m_iState >= 2)
		this->DispatchJavaScriptMethodCalls();
}