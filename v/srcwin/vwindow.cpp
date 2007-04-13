//===============================================================
// vWindow.cpp - vWindow class functions - MSWindows
//
// Copyright (C) 1995,1996, 1997, 1998   Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vwin32.h>		// for Win 32 stuff
#include <v/vapp.h>		// we are a friend of App
#include <v/vwindow.h>		// our header
#include <v/vpane.h>		// we have panes
#include <v/vmenu.h>
#include <v/vkeys.h>

#include <v/vnotice.h>

// Define static data of the class

    int vWindow::_numWindows = 0;
    vWindow* vWindow::_WinList = 0;
    HWND vWindow::_curWin = 0;
    HWND vWindow::_prevWin = 0;

//======================>>> vWindow::vWindow <<<=======================
  vWindow::vWindow(VCONST char *name, int width, int height, 
	WindowType wintype ) : vBaseWindow(name)		// constructor
  {

    SysDebug2(Constructor,"vWindow::vWindow(height %d, width %d) constructor\n",
	height,width)

    _WindowMenuIndex = 0;	// haven't added windows menu yet
    // make hendles null

    _menuBar = 0;
    _paneList = 0;		// no panes yet

    // Set height and width using default if 0 supplied
    _WinHeight = height ? height : theApp->DefaultHeight();
    _WinWidth = width ? width : theApp->DefaultWidth();

    _wType = wintype;

    _NextWin = _WinList;	// link in at top
    _WinList = this;
    _canvasPane = 0;		// no canvas pane yet
    _justActivated = 0;
    _mouseDown = 0;
    _shiftKeyDown = 0;
    _ctrlKeyDown = 0;
    _createdOK = 0;

    initialize();		// set this guy up
  }
//======================>>> vWindow::vWindow <<<=======================
  vWindow::vWindow(const vWindow& w) : 		// Copy Constructor
    vBaseWindow(w)
  {
    // Copy is shallow, but we track that it is a copy                     
    vSysError("vWindow - V semantics do not support copy constructors!");
  }

//======================>>> vWindow::~vWindow <<<=======================
  vWindow::~vWindow()			// destructor
  {
    SysDebug(Destructor,"vWindow::~vWindow destructor\n")
  
    if (!_createdOK)
	return;
    _createdOK = 0;	// prevent multiple destructs
    PaneList* temp;
    for (PaneList* pl = _paneList ; pl != 0 ; )
      {
	temp = pl;
	pl = pl->nextPL;
	delete temp;		// delete the PaneLists we allocated
      }
    if (_menuBar != 0)		// clean up the menu bar
      {
 	::DestroyMenu(_menuBar);
	_menuBar = 0;
      }
  }

//======================>>> vWindow::AddPane <<<=======================
  void vWindow::AddPane(vPane* add_pane)
  {
    // Add a pane to the window

    PaneList* newPane = new PaneList;	// add a new pane to the list

    SysDebug1(Build,"vWindow::AddPane() to %s\n",_name)

    newPane->pane = add_pane;		// this is the pane we are adding

    newPane->nextPL = _paneList;	// add new pane in at top
    _paneList = newPane;

    if (add_pane->_paneType == P_Menu)	// P_Menu is special
      {
	_menuPane = add_pane;
	add_pane->initialize(this, (HWND)_menuBar);
	HMENU popup = ::GetSubMenu(_menuBar,_WindowMenuIndex); // get 1st menu item

	if (popup)		// add the window list
	  {
#ifdef VWIN16
	    BOOL refresh = 0;
#else
	    BOOL refresh = 1;
#endif
	    FORWARD_WM_MDISETMENU(theApp->_Client, refresh,
		(HMENU) _menuBar, popup, ::SendMessage);
	  }

	::DrawMenuBar(theApp->_Frame); // Make it change
      }
    else
      add_pane->initialize(this, _myHwnd); // add to _toplevel paneWidget
  }

//======================>>> vWindow::initialize <<<=======================
  void vWindow::initialize(void)
  {
  // All top level windows in this frame are implemented
  // as MDI child windows

    if (_numWindows > 0 && theApp->simSDI())
	return;
  
    MDICREATESTRUCT mdic;
  
    _menuBar = ::CreateMenu(); // Menu Bar

    mdic.szClass = "MdiWindowClass"; // Standard Windows setup
    mdic.szTitle = _name;
    mdic.hOwner = theApp->vHandle();
    mdic.x = mdic.y = CW_USEDEFAULT;
    int fx = ::GetSystemMetrics(SM_CXFRAME);
    int fy = ::GetSystemMetrics(SM_CYFRAME);

    int titley = ::GetSystemMetrics(SM_CYCAPTION);

    mdic.cx = _WinWidth+(2*fx);
    mdic.cy = _WinHeight + (2*fy) + titley;
    if (theApp->simSDI())
	mdic.style = WS_MAXIMIZE | WS_CHILD | WS_CLIPSIBLINGS
	  | WS_CLIPCHILDREN | WS_VISIBLE;
    else
	mdic.style = 0;
    mdic.lParam = (LPARAM)this; // send this to PMdiWindowProc

    _myHwnd = (HWND)::SendMessage(theApp->_Client,
	WM_MDICREATE,0, (long)(LPMDICREATESTRUCT) &mdic);

    // save a copy of this in the window structure
  
    ::SetWindowLong(_myHwnd, 0 , (long)this);
    ::SetWindowText(_myHwnd, _name);

    ++_numWindows;		// bump how many windows up
    _cursor = ::LoadCursor(NULL, IDC_ARROW);
    ::SetFocus(_myHwnd);
    _createdOK = 1;
  }

//============================>>> vWindow::SetWinCursor <<<==================
  void vWindow::SetWinCursor(VCursor vcursor)
  {
  
    switch (vcursor)
      {
	case VC_None:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
	case VC_Arrow:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
	case VC_CenterArrow:
	    _cursor = ::LoadCursor(NULL, IDC_UPARROW);
	    break;
	case VC_CrossHair:
	    _cursor = ::LoadCursor(NULL, IDC_CROSS);
	    break;
	case VC_EWArrows:
	    _cursor = ::LoadCursor(NULL, IDC_SIZEWE);
	    break;
	case VC_Hand:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
	case VC_IBar:
	    _cursor = ::LoadCursor(NULL, IDC_IBEAM);
	    break;
	case VC_Icon:
	    _cursor = ::LoadCursor(NULL, IDC_ICON);
	    break;
	case VC_NSArrows:
	    _cursor = ::LoadCursor(NULL, IDC_SIZENS);
	    break;
	case VC_Pencil:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
	case VC_Question:
	    _cursor = ::LoadCursor(NULL, IDC_CROSS);
	    break;
	case VC_Sizer:
	    _cursor = ::LoadCursor(NULL, IDC_SIZE);
	    break;
	case VC_Wait:
	    _cursor = ::LoadCursor(NULL, IDC_WAIT);
	    break;
	case VC_X:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
	default:
	    _cursor = ::LoadCursor(NULL, IDC_ARROW);
	    break;
      }
  }

//============================>>> vWindow::CloseWin <<<==========================
  void vWindow::CloseWin(void)
  {
  //	close button class callback
  //
  vWindow* w_list;
  
    // close base window, which cleans up some stuff
  
    SysDebug1(Build,"vWindow::CloseWin() - %s.\n",_name)

    closeBaseWindow();		// close this window
  
    --_numWindows;
    // Remove from active list

    if (_WinList == this)	// first one on list
      {
	_WinList = _WinList->_NextWin; // point to next
      }
    else
      {
	for (w_list = _WinList ; w_list != 0 ; w_list = w_list->_NextWin)
	  {
	    if (w_list->_NextWin == this)
	      {
		// remove from list
		w_list->_NextWin = (w_list->_NextWin)->_NextWin;
		break;
	      }
	  }
      }

    HWND hChild = FORWARD_WM_MDIGETACTIVE(theApp->_Client, ::SendMessage); // who caused this?
    FORWARD_WM_MDIDESTROY(theApp->_Client, hChild, ::SendMessage);
  }

//======================>>> vWindow::ShowPane <<<=======================
  void vWindow::ShowPane(vPane* wpane, int OnOrOff) VCONST
  {
    // Show or hide this pane

    // Search all panes until we find this one
    for (PaneList* pl = _paneList ; pl != 0 ; pl = pl->nextPL)
      {
	if (pl->pane == wpane)
	  {
	    (pl->pane)->ShowPane(OnOrOff); // let each pane show hide
	    return;
	  }
      }
    return;
  }

//====================>>> vWindow::GetValue <<<======================
  int vWindow::GetValue(ItemVal id) VCONST
  {
    // scan all buttons in this window to retrieve the what value

    int retval = 0;		// default to 0

    // Search all panes until we find this id
    for (PaneList* pl = _paneList ; pl != 0 ; pl = pl->nextPL)
      {
	if ((pl->pane)->GetPaneValue(id, retval))
	  {
	    return retval;
	  }
      }

    return 0;
  }

//====================>>> vWindow::GetTextIn <<<======================
  int vWindow::GetTextIn(ItemVal id, char* val, int maxlen) VCONST
  {
    // scan all buttons in this window to retrieve the what value

    int retval = 0;		// default to 0

    // Search all panes until we find this id
    for (PaneList* pl = _paneList ; pl != 0 ; pl = pl->nextPL)
      {
	if ((pl->pane)->GetPaneTextIn(id, val, maxlen))
	  {
	    return 1;
	  }
      }

    return 0;
  }

//=========================>>> vWindow::KeyIn <<<==========================
  void vWindow::KeyIn(vKey keysym, unsigned int shift)
  {
    // General interface to a keyboard event

    theApp->KeyIn(this, keysym, shift); // pass on unprocessed keys to app
  }

//=======================>>> vWindow::KeyInEV <<<==========================
  void vWindow::KeyInEV(unsigned int keycode, int map, int /*extended*/)
  {
    // Local interface to a keyboard event
  
    unsigned int WinCodes[] =
      {
	VK_HOME, VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN,
	VK_PRIOR,VK_NEXT, VK_END, VK_INSERT, VK_DELETE,

	VK_F1, VK_F2, VK_F3, VK_F4, VK_F5, VK_F6, VK_F7,
	VK_F8, VK_F9, VK_F10, VK_F11, VK_F12, VK_F13,
	VK_F14, VK_F15, VK_F16, 0
      };
    unsigned int vCodes[] =
      {
	vk_Home, vk_Left, vk_Up, vk_Right, vk_Down,
	vk_Page_Up, vk_Page_Down, vk_End, vk_Insert, vk_Delete,

	vk_F1, vk_F2,vk_F3, vk_F4, vk_F5, vk_F6, vk_F7,
	vk_F8, vk_F9,vk_F10, vk_F11, vk_F12, vk_F13,
	vk_F14, vk_F15,	vk_F16, 0
      };

    ItemVal id;

    vKey vkey;
    int shift = 0;

    if (::GetKeyState(VK_SHIFT) < 0)
	shift |= VKM_Shift;
    if (::GetKeyState(VK_CONTROL) < 0)
	shift |= VKM_Ctrl;

    if (!map)
	vkey = keycode;
    else
      {
	int ix;
	for (ix = 0 ; WinCodes[ix] != 0 ; ++ix)
	  {
	    if (WinCodes[ix] == keycode)
	      {
		vkey = vCodes[ix];
		break;
	      }
	  }
	if (WinCodes[ix] == 0)	// Ignore keys with no mapping
	    return;
      }

    if (vkey == vk_Tab && (shift & VKM_Shift) != 0)
	vkey = vk_BackTab;
    else if (vkey < ' ')
	shift &= VKM_Shift;	// Kill VKM_Ctrl for real control chars

    // BEW: V:1.13
    if (_menuPane && ((vMenuPane*)_menuPane)->CheckAccel(vkey, shift, id))
	MenuCommand(id);
    else
	KeyIn(vkey,shift);	// pass on to general code
  }

//======================>>> vWindow::MenuCommand <<<==========================
  void vWindow::MenuCommand(ItemVal id)
  {
    //	Menu Commands go through here first, then the regular WindowCommand
  
    WindowCommand(id, id, C_Menu);
  }

//==================>>> vWindow::SetValue <<<========================
  void vWindow::SetValue(ItemVal id, int val, ItemSetType setType)
  {
    // Set the given item on or off
    for (PaneList* pl = /* w_list->*/ _paneList ; pl != 0 ; pl = pl->nextPL)
      {
	(pl->pane)->SetPaneValue(id, val, setType);
      }
  }

//==================>>> vWindow::SetValueAll <<<========================
  void vWindow::SetValueAll(ItemVal id, int val, ItemSetType setType)
  {
    // Set the given item on or off in all windows


    // search all windows to set all buttons
    for (vWindow* w_list = _WinList ; w_list != 0 ; w_list = w_list->_NextWin)
      {
	// in each window, search each pane list
	for (PaneList* pl = /* w_list->*/ _paneList ; pl != 0 ; pl = pl->nextPL)
	  {
	    (pl->pane)->SetPaneValue(id, val, setType);
	  }
      }
  }

//================>>> vWindow::GetPosition <<<========================
  void vWindow::GetPosition(int& left, int& top, int& width, 
	int& height) VCONST
  {
    RECT rct;
    RECT ar;

    ::GetWindowRect(_myHwnd, &rct);
    ::GetWindowRect(theApp->winClientHwnd(), &ar);

    left = (rct.left >= ar.left) ? rct.left - ar.left : rct.left;
    top = (rct.top >= ar.top) ? rct.top - ar.top : rct.top;
    width = rct.right - rct.left;
    height = rct.bottom - rct.top;
  
  }

//================>>> vWindow::SetPosition <<<========================
  void vWindow::SetPosition(int left, int top)
  {
    (void)::SetWindowPos(_myHwnd,HWND_TOP,left,top,0,0,SWP_NOSIZE);
  }

//================>>> vWindow::RaiseWindow <<<========================
  void vWindow::RaiseWindow(void)
  {
    // raise this window to the top of all other windows
    ::BringWindowToTop(_myHwnd);
  }

//================>>> vWindow::SetString <<<========================
  void vWindow::SetString(ItemVal id, VCONST char* str)
  {
    // search all buttons in this windows to set to correct value

    for (PaneList* pl =	/* w_list->*/ _paneList ; pl != 0 ; pl = pl->nextPL)
      {
	(pl->pane)->SetPaneString(id, str);
      }
  }

//================>>> vWindow::SetStringAll <<<========================
  void vWindow::SetStringAll(ItemVal id, VCONST char* str)
  {
    // search all buttons in all windows to set to correct value

    for (vWindow* w_list = _WinList ; w_list != 0 ; w_list = w_list->_NextWin)
      {
	// in each window, search each pane list
	for (PaneList* pl = /* w_list->*/ _paneList ; pl != 0 ; pl = pl->nextPL)
	  {
	    (pl->pane)->SetPaneString(id, str);
	  }
      }
  }

//======================>>> vWindow::SetTitle <<<==========================
  void vWindow::SetTitle(VCONST char* title)
  {
    // set the title in the title bar

    ::SetWindowText(_myHwnd, title);
  }

//======================>>> vWindow::UpdateView <<<========================
  void vWindow::UpdateView(vWindow* sender, int hint, void* pHint)
  {
    // This is the overridable function called by the framwork to take
    // action on changes which happened in the model (document).
    //
    //     sender: Hint on which window caused the change in model
    //     hint:   Enum defined in your derived app class.
    //             Hints about which kind of change is made, so that only
    //             appropriate action is taken on appropriate windows.
    //     pHint:  This is normally a pointer to a object representing the
    //             document, or part of it.  If the App has multiple open 
    //             documents, and multiple views for each document, a 
    //             typical use is to update only the views for the 
    //             document, except the sender's.
  }

//======================>>> vWindow::WindowCommand <<<==========================
  void vWindow::WindowCommand(ItemVal id, ItemVal retval, CmdType ctype)
  {
  
    SysDebug1(CmdEvents,"vWindow::WindowCommand(id: %d)\n",id)

    theApp->AppCommand(this, id, retval, ctype); // Pass unprocessed cmds to app
  }

//===================>>> vWindow::MdiWindowProc <<<=============================
  long vWindow::MdiWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
  {
  
    if (message == WM_SETFONT)
	return 0;
    else if (message == WM_INITDIALOG)
	return TRUE;


    switch (message)
      {
	case WM_ACTIVATE:
	  {
	    break;
	  }

	case WM_SETFOCUS:
	  {
            if (theApp->ClipboardCheckText())
                theApp->SetValueAll(M_Paste,1,Sensitive);
            else
                theApp->SetValueAll(M_Paste,1,Sensitive);
            if (_canvasPane)
                _canvasPane->EnterEV();
	    return 0;
	  }
      
	case WM_KILLFOCUS:
	  {
            if (_canvasPane)
                _canvasPane->LeaveEV();
            return 0;
	  }

	case WM_CREATE:
	  {
	    return 0;
	  }

	case WM_PAINT:
	  {
	    if (_canvasPane)
              {
		_canvasPane->ExposeEV();
              }
	    break;
	  }
        case WM_ERASEBKGND:	// when resized
          {
            return 1;
          }

	case WM_SIZE:
	  {

	    int width = LOWORD(lParam);
	    int height = HIWORD(lParam);
	    if (_canvasPane)
		_canvasPane->ResizeEV(width,height);
	    break;			// MUST also do default processing!
	  }
      
	case WM_RBUTTONDOWN:	// Right mouse down
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    if (_canvasPane)
		_canvasPane->MouseDown(x, y, 3);
	    return 0;
	  }
      
	case WM_RBUTTONUP:	// Right mouse up
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    if (_canvasPane)
		_canvasPane->MouseUp(x, y, 3);
	    return 0;
	  }
      
	case WM_RBUTTONDBLCLK:	// Right double click
	  {
	    //	    int x = (int)LOWORD(lParam);
	    //	    int y = (int)HIWORD(lParam);
	    break;
	  }
      
	case WM_MBUTTONDOWN:	// middle button
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    if (_canvasPane)
		_canvasPane->MouseDown(x, y, 2);
	    return 0;
	  }
      
	case WM_MBUTTONUP:
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    if (_canvasPane)
		_canvasPane->MouseUp(x, y, 2);
	    return 0;
	  }
      
	case WM_MBUTTONDBLCLK:
	  {
	    //	    int x = (int)LOWORD(lParam);
	    //	    int y = (int)HIWORD(lParam);
	    break;
	  }
      
	case WM_LBUTTONDOWN:	// Left mouse down
	  {
	    // We want this to work like X. The first click in an MDI window
	    // without focus changes focus, AND sends the LBUTTONDOWN message.
	    // We don't want the focus message to go to the application, so
	    // this kludgy code prevents the focus click from heading to the
	    // canvas...
	    if (_justActivated  && _prevWin != 0)
	      {
		_justActivated = 0;
		break;
	      }
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    _mouseDown = 1;
	    if (_canvasPane)
		_canvasPane->MouseDown(x, y, 1);
	    return 0;
	  }
      
	case WM_LBUTTONUP:
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    if (_mouseDown && _canvasPane)
		_canvasPane->MouseUp(x, y, 1);
	    _mouseDown = 0;
	    return 0;
	  }
      
	case WM_LBUTTONDBLCLK:
	  {
	    //	    int x = (int)LOWORD(lParam);
	    //	    int y = (int)HIWORD(lParam);
	    break;
	  }

	case WM_MOUSEMOVE:	// Mouse move
	  {
	    int x = (int)LOWORD(lParam);
	    int y = (int)HIWORD(lParam);
	    ::SetCursor(_cursor);
	    int button = -1;
	    if (wParam & MK_LBUTTON)
	      {
		if (_mouseDown)		// can get a move if focus change
		    button = 1;
	      }
	    else if (wParam & MK_MBUTTON)
		button = 2;
	    else if (wParam & MK_RBUTTON)
		button = 3;
	    // To make this work like the X version, we don't want any mouse
	    // motion unless a button is down.
	    if (button < 0)
	      {
		if (_canvasPane)
		    _canvasPane->MouseMotion(x,y);
		else
		    break;
	      }
	    else if (_canvasPane)
		_canvasPane->MouseMove(x, y, button);
	    return 0;
	  }

	case WM_DESTROY:
	  {
	    break;
	  }

	case WM_COMMAND:
	  {
#ifndef VWIN16
	    WORD id = LOWORD(wParam);
//	    HWND hwnd = (HWND)lParam;
//	    WORD cmd = HIWORD(wParam);
#else
	    WORD id = wParam;
//	    HWND hwnd = (HWND)LOWORD(lParam) ;
//	    WORD cmd = HIWORD(lParam);
#endif
	    MenuCommand((ItemVal)id);
	    return 0;
	  }
      
	case WM_KEYDOWN:
	  {
	    if (wParam == VK_SHIFT || wParam == VK_CONTROL)
		break;
	    // Handle only keystrokes that won't generate a WM_CHAR
	    else if ( (wParam <= VK_HELP && wParam != VK_CANCEL &&
		wParam != VK_ESCAPE && wParam != VK_SPACE &&
	    wParam != VK_RETURN && wParam != VK_BACK &&
	    wParam != VK_TAB) ||
	    (wParam >= VK_F1 && wParam <= VK_F16) )
	      {
		KeyInEV((unsigned int)wParam, 1, 0);
		return 0;
	      }
	    break;
	  }
      
	case WM_CHAR:		// Always an ASCII character
	  {
	    KeyInEV((unsigned int)wParam, 0, (HIWORD(lParam) & KF_EXTENDED) != 0);
	    return 0;
	  }
      
	case WM_HSCROLL:
	  {
	    if (_canvasPane)
		_canvasPane->HScrollEV(GET_WM_HSCROLL_CODE(wParam, lParam),
	    GET_WM_HSCROLL_POS(wParam, lParam),
	    GET_WM_HSCROLL_HWND (wParam, lParam));

	    return 0;
	  }
      
	case WM_VSCROLL:
	  {

	    if (_canvasPane)
		_canvasPane->VScrollEV(GET_WM_VSCROLL_CODE(wParam, lParam),
		GET_WM_VSCROLL_POS(wParam, lParam),
	    	GET_WM_VSCROLL_HWND (wParam, lParam));
	    return 0;
	  }
      
	case WM_MDIACTIVATE:
	  {
	    // First, we have to change the main menu to ours
	    int activate;
#ifndef VWIN16
	    /* This could be done with GET_WM_MDIACTIVATE_FACTIVATE,
	    but it doesn't seem to be in the windows 3.1 windowsx.h */
	    activate = lParam == (LPARAM)hWnd;
#else
	    activate = wParam != 0;
#endif

	    if (activate)		// we are gaining focus!
	      {
		if (theApp->InExit())
		    break;

		HMENU popup = ::GetSubMenu(_menuBar,_WindowMenuIndex); // get 1st menu item


		if (popup)		// add the window list
		  {
#ifdef VWIN16
		    BOOL refresh = 0;
#else
		    BOOL refresh = 1;
#endif
		    FORWARD_WM_MDISETMENU(theApp->_Client, refresh,
		    (HMENU) _menuBar, popup, ::SendMessage);
		  }

		_prevWin = _curWin;
		_curWin = hWnd;
		_justActivated = 1;
		::DrawMenuBar(theApp->_Frame); // Make menu change
		theApp->selectCmdPanes(this);  // Select our cmd bars
		// now resize to be sure they are drawn. This only matters
		// when the different child windows have different menus
                // or menus set on or off
		FORWARD_vWM_ResizeMDIClient(theApp->_Frame,SendMessage);
	      }
	    else			// we are losing focus
	      {
		HMENU appframe = theApp->AppFrameMenu();
		HMENU popup = GetSubMenu(appframe,0);
#ifdef VWIN16
		BOOL refresh = 0;
#else
		BOOL refresh = 1;
#endif
		FORWARD_WM_MDISETMENU(theApp->_Client, refresh,
		  appframe, popup, ::SendMessage);  // Change to our menu
		theApp->selectCmdPanes(this);  // Need to select command bars
              }

	    return 0;
	  }

	case WM_DROPFILES:
	  {
	    break;
	  }

	case WM_QUERYENDSESSION:
	  {
//	    CloseWin();
	    theApp->CloseAppWin(this);
	    return 0;
	  }

	case WM_CLOSE:
	  {
//	    CloseWin();
	    theApp->CloseAppWin(this);
	    return 0;
	  }

      }
    if (theApp->InExit()) return 0;	// BEW: fix for 1.17
    return ::DefMDIChildProc(hWnd, message, wParam, lParam );
  }
//------------------------------------------------------------------------
