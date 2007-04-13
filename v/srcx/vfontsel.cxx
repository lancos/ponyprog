//===============================================================
// vfontsel.cxx - vFontSelect class functions - X11R5
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#include <v/vfontsel.h>           // our header
#include <v/vicon.h>	// icons

// Define static data of the class

#define prompt_width 32
#define prompt_height 32
static unsigned char prompt_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0xf8, 0xff, 0xff, 0x1f, 0x04, 0x00, 0x00, 0x20,
   0xe4, 0xff, 0xff, 0x27, 0x14, 0x00, 0x00, 0x28, 0x14, 0xc0, 0x01, 0x28,
   0x14, 0xe0, 0x03, 0x28, 0x14, 0x30, 0x06, 0x28, 0x14, 0x30, 0x06, 0x28,
   0x14, 0x00, 0x03, 0x28, 0x14, 0x80, 0x01, 0x28, 0x14, 0xc0, 0x00, 0x28,
   0x14, 0xc0, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28,
   0x14, 0x00, 0x00, 0x28, 0x14, 0xc0, 0x00, 0x28, 0x14, 0x00, 0x00, 0x28,
   0xe4, 0xff, 0xff, 0x27, 0x04, 0x00, 0x00, 0x20, 0xf8, 0xff, 0xff, 0x1f,
   0xfe, 0xff, 0xff, 0x7f, 0x02, 0x00, 0x00, 0x40, 0x02, 0x00, 0x00, 0x40,
   0x02, 0x00, 0xe0, 0x47, 0x02, 0x00, 0x00, 0x40, 0xfa, 0xff, 0xff, 0x5f,
   0xae, 0xaa, 0xaa, 0x6a, 0x56, 0x55, 0x55, 0x55, 0xaa, 0xaa, 0xaa, 0x6a,
   0xfe, 0xff, 0xff, 0x7f, 0x00, 0x00, 0x00, 0x00};

#ifdef GENERICNAMES
    static char* familyList[] =
      {
	"Serif", "Serif2", "Sans Serif", "Sans Serif2",
        "Decorative", "Decorative 2",
	"Fixed", "Fixed2",
	"System Fixed", "System Variable", "System Default", 0
      };
#else
    static char* familyList[] =
      {
	"Times", "New Century Schoolbook", "Lucida", "Helvetica",
        "Symbol", "Terminal",
	"Courier", "Clean",
	"System Fixed", "System Variable", "System Default", 0
      };
#endif
    static vFontID familyMap[] =
      {
	vfSerif, vfSerif2, vfSansSerif,vfSansSerif2,
	vfDecorative, vfDecorative2,
	vfFixed, vfFixed2,
	vfDefaultFixed, vfDefaultVariable, vfDefaultSystem,
	vfEndOfList
      };

    static char* styleList[] = { "Normal", "Italic", 0 };
    static vFontID styleMap[] = { vfNormal, vfItalic, vfEndOfList};

    static char* weightList[] = { "Normal", "Bold", 0};
    static vFontID weightMap[] = { vfNormal, vfBold, vfEndOfList};

    static char* sizeList[] = { "8", "9", "10", "11", "12", "13", "14",
	"16", "18", "20", "24", "25", "30", "35", "40", 0};
    static int sizeMap[] = { 8, 9, 10, 11, 12, 13, 14,
	16, 18, 20, 24, 25,  30, 35, 40, 0};

    static int underline = 0;

    static vIcon prompt((unsigned char*)&prompt_bits[0],prompt_height,
       prompt_width);


    const int famIX = 4;
    const int styIX = 7;
    const int wtIX = 10;
    const int szIX = 13;
    const int undIX = 14;

    static CommandObject SelFontDialog[] =
      {
	{C_Icon,  10, 0, "SelFont?", (void*)&prompt, CA_None, isSens,NoFrame,0,0},
	{C_Label, 1, 1, "Select Font",NoList,
		CA_MainMsg ,isSens,NoFrame, 10,0},


	{C_Frame, 20, 0 , "",NoList,CA_None,isSens,NoFrame, 0, 10},
	{C_Label, 21, 21, "Family:",NoList,CA_None,isSens, 20, 0, 0},

	{C_ComboBox,22,2, "", (void*)familyList, CA_NoNotify, isSens, 20,0,21},

	{C_Frame, 30, 0 , "",NoList,CA_None,isSens,NoFrame, 20, 10},
	{C_Label, 31, 31, "Style:",NoList,CA_None,isSens,30, 0, 0},
	{C_ComboBox,32,0, "", (void*)styleList, CA_NoNotify, isSens, 30,0,31},

	{C_Frame, 40, 0 , "",NoList,CA_None,isSens,NoFrame, 30, 10},
	{C_Label, 41, 41, "Weight:",NoList,CA_None,isSens,40, 0, 0},
	{C_ComboBox,42,1, "", (void*)weightList, CA_NoNotify, isSens, 40,0,41},

	{C_Frame, 50, 0 , "",NoList,CA_None,isSens,NoFrame, 40, 10},
	{C_Label, 51, 51, "Size:",NoList,CA_None,isSens,50, 0, 0},
	{C_ComboBox,52,1, "", (void*)sizeList, CA_NoNotify, isSens, 50,0,51},


	{C_CheckBox, 61, 0, "Underlined",NoList,CA_None,isSens,NoFrame,0,20},

	{C_Button, M_Cancel, M_Cancel, " Cancel ", NoList,CA_None,
		isSens,NoFrame,20,61},
	{C_Button, M_OK, M_OK, " OK ", NoList, CA_DefaultButton,
		isSens,NoFrame,M_Cancel,61},
	{C_EndOfList,0,0,0,0,CA_None,0,0,0}
      };

//======================>>> vFontSelect::FontSelect <<<=======================
  int vFontSelect::FontSelect(vFont& font,const char* msg, int fixedOnly)
  {
    // Select a font
    // @@@ need to add support for fixedOnly

    int ans, ix;

    // Set to values of the supplied font
    vFontID vfam = font.GetFamily();
    int ptsz = font.GetPointSize();
    vFontID vsty = font.GetStyle();
    vFontID vwt = font.GetWeight();
    int vund = font.GetUnderlined();

    for (ix = 0 ; familyMap[ix] != vfEndOfList ; ++ix)
	if (familyMap[ix] == vfam)
	  {
	    SelFontDialog[famIX].retVal = ix;
	    break;
	  }

    for (ix = 0 ; sizeMap[ix] != 0 ; ++ix)
	if (sizeMap[ix] == ptsz)
	  {
	    SelFontDialog[szIX].retVal = ix;
	    break;
	  }
    for (ix = 0 ; styleMap[ix] != vfEndOfList ; ++ix)
	if (styleMap[ix] == vsty)
	  {
	    SelFontDialog[styIX].retVal = ix;
	    break;
	  }
    for (ix = 0 ; weightMap[ix] != vfEndOfList ; ++ix)
	if (weightMap[ix] == vwt)
	  {
	    SelFontDialog[wtIX].retVal = ix;
	    break;
	  }
    SelFontDialog[undIX].retVal = vund;

    if (!_added)
      {
	AddDialogCmds(SelFontDialog);		// Set up standard dialog
	_added = 1;
      }

    (void) ShowModalDialog((char*)msg, ans);	// show and wait

    if (ans == M_Cancel)
	return 0;
    int fam = GetValue(22);		// get family value
    int sty = GetValue(32);
    int wt = GetValue(42);
    int sz = GetValue(52);
    int und = GetValue(61);

    font.SetFontValues(familyMap[fam], sizeMap[sz], styleMap[sty],
	weightMap[wt], und);

    return ans == M_OK;
  }

//====================>>> vFontSelect::DialogCommand <<<=======================
  void vFontSelect::DialogCommand(ItemVal id, ItemVal val, CmdType ctype)
  {
    vModalDialog::DialogCommand(id,val,ctype);
    if (id == M_OK || id == M_Cancel)
	CloseDialog();
  }
// --------------------------------------------------------------------- 
