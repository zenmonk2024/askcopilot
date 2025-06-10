//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KWin32App.cpp
// Date:	2000.08.08
// Code:	WangWei(Daphnis)
// Desc:	KWin32App Class
//---------------------------------------------------------------------------
#include "Precompile.h"
#include "Debug.h"

//#include "KWin32Wnd.h"
#include "KWin32App.h"
#include <mmsystem.h>

static KWin32App* s_pWin32App = NULL;
static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#ifndef WM_MOUSEHOVER
	#define WM_MOUSEHOVER 0x02A1
#endif

#define	MOUSE_EVENT_NONE			0
#define	MOUSE_EVENT_HAPPEND			1
#define	MOUSE_HOVER_MSG_SENT		2
#define	MOUSE_HOVER_START_TIME_MIN	3
//m_uMouseHoverStartTime ����
//MOUSE_EVENT_NONE		--> δ��������¼�
//MOUSE_EVENT_HAPPEND	--> ������¼�
//MOUSE_HOVER_MSG_SENT	--> ������¼��ĳ���ʱ�䳬�����趨��ʱ�����ƣ��ѷ�����WM_NCMOUSEHOVER��Ϣ
//����ֵ				--> ������¼��ĳ���ʱ��δ�����趨��ʱ�����ƣ���ֵ��ʾ������Ŀ�ʼʱ��

//---------------------------------------------------------------------------
// ����:	WndProc
// ����:	���ڻص�����
// ����:	hWnd		���ھ��
//			uMsg		��Ϣ����
//			wParam		����1(32bit)
//			lParam		����2(32bit)
// ����:	LRESULT
//---------------------------------------------------------------------------
LRESULT CALLBACK WndProc(
						 HWND	hWnd,		// handle of window
						 UINT	uMsg,		// type of message
						 WPARAM	wParam, 	// first message parameter 32bit
						 LPARAM	lParam) 	// second message parameter 32bit
{
	return s_pWin32App->MsgProc(hWnd, uMsg, wParam, lParam);
}
//---------------------------------------------------------------------------
// ����:	KWin32App
// ����:	���캯��
//---------------------------------------------------------------------------
KWin32App::KWin32App()
{
	m_bActive = TRUE;
	m_dwMinimizeMainWinTime = 0;

	m_bShowMouse = TRUE;
	m_uMouseHoverTimeSetting = 0;
	m_uMouseHoverStartTime = MOUSE_EVENT_NONE;
	m_uLastMouseStatus = 0;
	m_nLastMousePos = 0;
	m_uLastMouseStatus = 0;
	s_pWin32App = this;
	m_nIconId = 0;
	m_hWnd = NULL;
}

//---------------------------------------------------------------------------
// ����:	Init
// ����:	��ʼ��
// ����:	hInstance	ʵ�����
//---------------------------------------------------------------------------
int KWin32App::Init(HINSTANCE hInstance, const char *AppName, int nIconId)
{
	m_nIconId = nIconId;

	strcpy(m_szTitle, AppName);
	strcpy(m_szClass, AppName);
	strcat(m_szClass, "_Class");

	if (!InitClass(hInstance))
		return FALSE;

	if (!InitWindow(hInstance))
		return FALSE;

	return TRUE;
}

//---------------------------------------------------------------------------
// ����:	InitClass
// ����:	��ʼ�����ڳ�����
// ����:	hInstance	ʵ�����
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------
int KWin32App::InitClass(HINSTANCE hInstance)
{
	WNDCLASS wc;
	
	wc.style			= CS_DBLCLKS;
	wc.lpfnWndProc		= WndProc;
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= LoadIcon(hInstance, MAKEINTATOM(m_nIconId));
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName 	= NULL;
	wc.lpszClassName	= m_szClass;

	return RegisterClass(&wc);
}
//---------------------------------------------------------------------------
// ����:	InitWindow
// ����:	��ʼ������
// ����:	hInstance	ʵ�����
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------
int KWin32App::InitWindow(HINSTANCE hInstance)
{
	m_hWnd = CreateWindow(
		m_szClass,
		m_szTitle,
        WS_CAPTION | WS_EX_TOOLWINDOW | WS_SYSMENU | WS_MINIMIZEBOX | WS_CLIPCHILDREN, 
		0,
		0,
		0,
		0,
		NULL,
		NULL,
		hInstance,		NULL);

	if (!m_hWnd)
	{
		return FALSE;
	}

	UpdateWindow(m_hWnd);
	SetFocus(m_hWnd);
	return TRUE;
}
//---------------------------------------------------------------------------
// ����:	Run
// ����:	����Ϣѭ��
//---------------------------------------------------------------------------
void KWin32App::Run()
{
	MSG	Msg;

	while (TRUE)
	{
		if (PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (Msg.message == WM_QUIT)
                break;

			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
		else
		{
			GenerateMsgHoverMsg();
			if (!GameLoop())
			{
				PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			}
		}
	}
	m_hWnd = NULL;
}

//---------------------------------------------------------------------------
// ����:	GameLoop
// ����:	��Ϸ��ѭ��
// ����:	void
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------
int KWin32App::GameLoop()
{
	WaitMessage();
	return TRUE;
}

//---------------------------------------------------------------------------
// ����:	MsgProc
// ����:	���ڻص�����
// ����:	hWnd		���ھ��
//			uMsg		��Ϣ����
//			wParam		����1(32bit)
//			lParam		����2(32bit)
// ����:	TRUE		�ɹ�
//			FALSE		ʧ��
//---------------------------------------------------------------------------
LRESULT KWin32App::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		if (HandleInput(uMsg, wParam, lParam))
			return 0;
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SETCURSOR:
		if (m_bShowMouse == FALSE && m_bActive &&
			LOWORD(lParam) == HTCLIENT)
		{
			SetCursor(NULL);
			return TRUE;
		}
        else if (LOWORD(lParam) == HTCLIENT)
        {
			HandleInput(uMsg, wParam, lParam);

            // ����ⲿʹ�������Լ�����꣬���û�з���TRUE��
            // ��ô�ͻᵼ����걻���û���
            return TRUE;
        }
		break;

	case WM_IME_CHAR:
		 return HandleInput(uMsg, wParam, lParam);
	case WM_INPUTLANGCHANGEREQUEST:
	case WM_IME_NOTIFY:
        if (HandleInput(uMsg, wParam, lParam))
            return 0;
        break;
		//if (g_pIme && 
		//	g_pIme->WndMsg(hWnd,uMsg,wParam,lParam))
		//	return 0;
		//break;

    case WM_NCPAINT:
    case WM_NCCALCSIZE:
        if (HandleInput(uMsg, wParam, lParam))
            return 0;
        break;


	case WM_ACTIVATEAPP:
		m_bActive = (int)wParam;
//		#ifndef NO_DIRECT_X
//		if (g_pDirectDraw && FULLSCREEN == g_pDirectDraw->GetScreenMode())
//		{
//			if (FALSE == m_bActive)
//			{
//				if (0 == m_dwMinimizeMainWinTime)
//					m_dwMinimizeMainWinTime = GetTickCount();
//			}
//			else
//				m_dwMinimizeMainWinTime = 0;
//		}
	//#ifndef NO_DIRECT_X
//		if (m_bActive && g_pDirectDraw)
//			g_pDirectDraw->RestoreSurface();
//		#endif
   	    HandleInput(uMsg, wParam, lParam);
		break;

    case WM_SYSCOMMAND:
        if (wParam == SC_KEYMENU)
        {
            return 0;
        }
	case WM_COPYDATA:
		HandleInput(uMsg, wParam, lParam);
		break;

    case WM_NCACTIVATE:
    case WM_NCHITTEST:
		HandleInput(uMsg, wParam, lParam);
        break;

    case WM_MOVE:
    case WM_MOVING:
    case WM_SIZE:
		if (wParam == SIZE_MINIMIZED)
		{
			m_dwMinimizeMainWinTime = GetTickCount();
		}
		else
		{
			m_dwMinimizeMainWinTime = 0;
		}
    case WM_SIZING:
    case WM_WINDOWPOSCHANGED:
    case WM_WINDOWPOSCHANGING:
	case WM_MOUSELEAVE:
		HandleInput(uMsg, wParam, lParam);
		break;

	case WM_SETFOCUS:
	case WM_KILLFOCUS:
		HandleInput(uMsg, wParam, lParam);
		break;

	default:
		if (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST)
		{
			m_uLastMouseStatus = wParam;
			m_nLastMousePos = lParam;
			m_uMouseHoverStartTime = MOUSE_EVENT_HAPPEND;
		}
		HandleInput(uMsg, wParam, lParam);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//---------------------------------------------------------------------------
// ����:	ShowMouse
// ����:	�������ɼ�
// ����:	bShow		int
// ����:	void
//---------------------------------------------------------------------------
void KWin32App::ShowMouse(int bShow)
{
	m_bShowMouse = bShow;
}

void KWin32App::SetMouseHoverTime(unsigned int nHoverTime)
{
	m_uMouseHoverTimeSetting = nHoverTime;
}

void KWin32App::GenerateMsgHoverMsg()
{
	if (m_uMouseHoverStartTime == MOUSE_HOVER_MSG_SENT)
	{
		static char cCounter = 0;
		if ((++cCounter) == 7)
		{
			cCounter = 0;
			HandleInput(WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos);
		}
	}
	else if (m_uMouseHoverStartTime >= MOUSE_HOVER_START_TIME_MIN)
	{
		unsigned int	nCurrentTime = timeGetTime();
		if ((nCurrentTime - m_uMouseHoverStartTime) >= m_uMouseHoverTimeSetting)
		{
			HandleInput(WM_MOUSEHOVER, m_uLastMouseStatus, m_nLastMousePos);
			m_uMouseHoverStartTime = MOUSE_HOVER_MSG_SENT;
		}
	}
	else if (m_uMouseHoverStartTime == MOUSE_EVENT_HAPPEND)
	{
		m_uMouseHoverStartTime = MOUSE_EVENT_NONE;
	}
	else if (m_uMouseHoverTimeSetting)
	{
		//��ʱ(m_uMouseHoverStartTime == MOUSE_EVENT_NONE)
		m_uMouseHoverStartTime = timeGetTime();
		if (m_uMouseHoverStartTime < MOUSE_HOVER_START_TIME_MIN)
			m_uMouseHoverStartTime = MOUSE_HOVER_START_TIME_MIN;
	}
}

HWND KWin32App::GetMainWnd()
{
	return (s_pWin32App ? s_pWin32App->m_hWnd : NULL);
}