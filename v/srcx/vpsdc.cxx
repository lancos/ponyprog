//===============================================================
// vPostScriptDC - a PostScript canvas for drawing
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================


#include <math.h>

#include <v/vpsdc.h>

#include <v/vicon.h>		// for icons

#include <v/vapp.h>
#include <v/vutil.h>
#include <v/vcolor.h>
#include <v/vmodald.h>
#include <v/vnotice.h>

// define entries for font tables

#define fHelv 0
#define fHelvObl 1
#define fHelvBold 2
#define fHelvBoldObl 3
#define fTimesRoman 4
#define fTimesItal 5
#define fTimesBold 6
#define fTimesBoldItal 7
#define fSymbol 8
#define fCourier 9
#define fCourierObl 10
#define fCourierBold 11
#define fCourierBoldObl 12

    static const char *fontNames[] =
      {
	"Helvetica", "Helvetica-Oblique", "Helvetica-Bold", "Helvetica-BoldOblique",
        "Times-Roman", "Times-Italic", "Times-Bold", "Times-BoldItalic",
	"Symbol",
	"Courier", "Courier-Oblique", "Courier-Bold", "Courier-BoldOblique",
	0
      };

// These tables have the width in 1/1000's of a point for a 1 point size
// character, as determined by PostScript .afm files. Thus,
// 278 means a 1 point character is .278 points wide.
static short fontW[][256] = {
{						/*Helvetica*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 278, 278, 355, 556, 556, 889, 667, 222, 333, 333, 389, 584, 278, 333, 278, 278,
 556, 556, 556, 556, 556, 556, 556, 556, 556, 556, 278, 278, 584, 584, 584, 556,
1015, 667, 667, 722, 722, 667, 611, 778, 722, 278, 500, 667, 556, 833, 722, 778,
 667, 778, 722, 667, 611, 722, 667, 944, 667, 667, 611, 278, 278, 278, 469, 556,
 222, 556, 556, 500, 556, 556, 278, 556, 556, 222, 222, 500, 222, 833, 556, 556,
 556, 556, 333, 500, 278, 556, 500, 722, 500, 500, 500, 334, 260, 334, 584,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 556, 556, 167, 556, 556, 556, 556, 191, 333, 556, 333, 333, 500, 500,
   0, 556, 556, 556, 278,   0, 537, 350, 222, 333, 333, 556,1000,1000,   0, 611,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,1000,   0, 370,   0,   0,   0,   0, 556, 778,1000, 365,   0,   0,   0,   0,
   0, 889,   0,   0,   0, 278,   0,   0, 222, 611, 944, 611,   0,   0,   0,   0
},

{						/*Helvetica-Oblique*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 278, 278, 355, 556, 556, 889, 667, 222, 333, 333, 389, 584, 278, 333, 278, 278,
 556, 556, 556, 556, 556, 556, 556, 556, 556, 556, 278, 278, 584, 584, 584, 556,
1015, 667, 667, 722, 722, 667, 611, 778, 722, 278, 500, 667, 556, 833, 722, 778,
 667, 778, 722, 667, 611, 722, 667, 944, 667, 667, 611, 278, 278, 278, 469, 556,
 222, 556, 556, 500, 556, 556, 278, 556, 556, 222, 222, 500, 222, 833, 556, 556,
 556, 556, 333, 500, 278, 556, 500, 722, 500, 500, 500, 334, 260, 334, 584,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 556, 556, 167, 556, 556, 556, 556, 191, 333, 556, 333, 333, 500, 500,
   0, 556, 556, 556, 278,   0, 537, 350, 222, 333, 333, 556,1000,1000,   0, 611,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,1000,   0, 370,   0,   0,   0,   0, 556, 778,1000, 365,   0,   0,   0,   0,
   0, 889,   0,   0,   0, 278,   0,   0, 222, 611, 944, 611,   0,   0,   0,   0
},

{						/*Helvetica-Bold*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 278, 333, 474, 556, 556, 889, 722, 278, 333, 333, 389, 584, 278, 333, 278, 278,
 556, 556, 556, 556, 556, 556, 556, 556, 556, 556, 333, 333, 584, 584, 584, 611,
 975, 722, 722, 722, 722, 667, 611, 778, 722, 278, 556, 722, 611, 833, 722, 778,
 667, 778, 722, 667, 611, 722, 667, 944, 667, 667, 611, 333, 278, 333, 584, 556,
 278, 556, 611, 556, 611, 556, 333, 611, 611, 278, 278, 556, 278, 889, 611, 611,
 611, 611, 389, 556, 333, 611, 556, 778, 556, 556, 500, 389, 280, 389, 584,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 556, 556, 167, 556, 556, 556, 556, 238, 500, 556, 333, 333, 611, 611,
   0, 556, 556, 556, 278,   0, 556, 350, 278, 500, 500, 556,1000,1000,   0, 611,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,1000,   0, 370,   0,   0,   0,   0, 611, 778,1000, 365,   0,   0,   0,   0,
   0, 889,   0,   0,   0, 278,   0,   0, 278, 611, 944, 611,   0,   0,   0,   0
},

{						/*Helvetica-BoldOblique*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 278, 333, 474, 556, 556, 889, 722, 278, 333, 333, 389, 584, 278, 333, 278, 278,
 556, 556, 556, 556, 556, 556, 556, 556, 556, 556, 333, 333, 584, 584, 584, 611,
 975, 722, 722, 722, 722, 667, 611, 778, 722, 278, 556, 722, 611, 833, 722, 778,
 667, 778, 722, 667, 611, 722, 667, 944, 667, 667, 611, 333, 278, 333, 584, 556,
 278, 556, 611, 556, 611, 556, 333, 611, 611, 278, 278, 556, 278, 889, 611, 611,
 611, 611, 389, 556, 333, 611, 556, 778, 556, 556, 500, 389, 280, 389, 584,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 556, 556, 167, 556, 556, 556, 556, 238, 500, 556, 333, 333, 611, 611,
   0, 556, 556, 556, 278,   0, 556, 350, 278, 500, 500, 556,1000,1000,   0, 611,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,1000,   0, 370,   0,   0,   0,   0, 611, 778,1000, 365,   0,   0,   0,   0,
   0, 889,   0,   0,   0, 278,   0,   0, 278, 611, 944, 611,   0,   0,   0,   0
},

{						/*Times-Roman*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 250, 333, 408, 500, 500, 833, 778, 333, 333, 333, 500, 564, 250, 333, 250, 278,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 278, 278, 564, 564, 564, 444,
 921, 722, 667, 667, 722, 611, 556, 722, 722, 333, 389, 722, 611, 889, 722, 722,
 556, 722, 667, 556, 611, 722, 722, 944, 722, 722, 611, 333, 278, 333, 469, 500,
 333, 444, 500, 444, 500, 444, 333, 500, 500, 278, 278, 500, 278, 778, 500, 500,
 500, 500, 333, 389, 278, 500, 500, 722, 500, 500, 444, 480, 200, 480, 541,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 500, 500, 167, 500, 500, 500, 500, 180, 444, 500, 333, 333, 556, 556,
   0, 500, 500, 500, 250,   0, 453, 350, 333, 444, 444, 500,1000,1000,   0, 444,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 889,   0, 276,   0,   0,   0,   0, 611, 722, 889, 310,   0,   0,   0,   0,
   0, 667,   0,   0,   0, 278,   0,   0, 278, 500, 722, 500,   0,   0,   0,   0
}, 

{						/*Times-Italic*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 250, 333, 420, 500, 500, 833, 778, 333, 333, 333, 500, 675, 250, 333, 250, 278,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 333, 333, 675, 675, 675, 500,
 920, 611, 611, 667, 722, 611, 611, 722, 722, 333, 444, 667, 556, 833, 667, 722,
 611, 722, 611, 500, 556, 722, 611, 833, 611, 556, 556, 389, 278, 389, 422, 500,
 333, 500, 500, 444, 500, 444, 278, 500, 500, 278, 278, 444, 278, 722, 500, 500,
 500, 500, 389, 389, 278, 500, 444, 667, 444, 444, 389, 400, 275, 400, 541,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 389, 500, 500, 167, 500, 500, 500, 500, 214, 556, 500, 333, 333, 500, 500,
   0, 500, 500, 500, 250,   0, 523, 350, 333, 556, 556, 500, 889,1000,   0, 500,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
 889,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 889,   0, 276,   0,   0,   0,   0, 556, 722, 944, 310,   0,   0,   0,   0,
   0, 667,   0,   0,   0, 278,   0,   0, 278, 500, 667, 500,   0,   0,   0,   0
},

{						/*Times-Bold*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 250, 333, 555, 500, 500,1000, 833, 333, 333, 333, 500, 570, 250, 333, 250, 278,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 333, 333, 570, 570, 570, 500,
 930, 722, 667, 722, 722, 667, 611, 778, 778, 389, 500, 778, 667, 944, 722, 778,
 611, 778, 722, 556, 667, 722, 722,1000, 722, 722, 667, 333, 278, 333, 581, 500,
 333, 500, 556, 444, 556, 444, 333, 500, 556, 278, 333, 556, 278, 833, 556, 500,
 556, 556, 444, 389, 333, 556, 500, 722, 500, 500, 444, 394, 220, 394, 520,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 333, 500, 500, 167, 500, 500, 500, 500, 278, 500, 500, 333, 333, 556, 556,
   0, 500, 500, 500, 250,   0, 540, 350, 333, 500, 500, 500,1000,1000,   0, 500,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,1000,   0, 300,   0,   0,   0,   0, 667, 778,1000, 330,   0,   0,   0,   0,
   0, 722,   0,   0,   0, 278,   0,   0, 278, 500, 722, 556,   0,   0,   0,   0
},

{						/*Times-BoldItalic*/
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
 250, 389, 555, 500, 500, 833, 778, 333, 333, 333, 500, 570, 250, 333, 250, 278,
 500, 500, 500, 500, 500, 500, 500, 500, 500, 500, 333, 333, 570, 570, 570, 500,
 832, 667, 667, 667, 722, 667, 667, 722, 778, 389, 500, 667, 611, 889, 722, 722,
 611, 722, 667, 556, 611, 722, 667, 889, 667, 611, 611, 333, 278, 333, 570, 500,
 333, 500, 500, 444, 500, 444, 333, 500, 556, 278, 278, 500, 278, 778, 556, 500,
 500, 500, 389, 389, 278, 556, 444, 667, 500, 444, 389, 348, 220, 348, 570,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 389, 500, 500, 167, 500, 500, 500, 500, 278, 500, 500, 333, 333, 556, 556,
   0, 500, 500, 500, 250,   0, 500, 350, 333, 500, 500, 500,1000,1000,   0, 500,
   0, 333, 333, 333, 333, 333, 333, 333, 333,   0, 333, 333,   0, 333, 333, 333,
1000,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
   0, 944,   0, 266,   0,   0,   0,   0, 611, 722, 944, 300,   0,   0,   0,   0,
   0, 722,   0,   0,   0, 278,   0,   0, 278, 500, 722, 500,   0,   0,   0,   0
}
};
//================>>> vPostScriptDC::vPostScriptDC <<<========================
  vPostScriptDC::vPostScriptDC(const int scalePrinting)
  {
    // scalePrinting is necessary to get Ms-Windows to print narrow
    // lines on output. It is not currently used by the X version.

    SysDebug(Constructor,"vPostScriptDC::vPostScriptDC() constructor\n")

    _font = theApp->GetDefaultFont();	// get the default font
    pstream = 0;
    _fontIndex = fCourier;
    iconPM = 0;
    _curColor.Set(0,0,0);	// make black

    _physHeight = _printer.GetHeight();
    _physWidth = _printer.GetWidth();

  }

//================>>> vPostScriptDC::~vPostScriptDC <<<========================
  vPostScriptDC::~vPostScriptDC()
  {

    SysDebug(Destructor,"vPostScriptDC::~vPostScriptDC() destructor\n")
  }

//================>>> vPostScriptDC::BeginPrinting <<<========================
//  int vPostScriptDC::BeginPrinting()
  int vPostScriptDC::BeginPrinting(char *filename, int ask, int isColor)
  {
    char fn[100];

    if (pstream)		// should not happen...
      {
        pstream->close();
	return 0;
      }

    // get an output stream to print to
    if (!(pstream = _printer.OpenPrinter()))
	return 0;

    // Write the PostScript header we need.

    char date[20], time[20];

    vGetLocalDate(date); vGetLocalTime(time);

    // write the required header

    char* dname = _printer.GetDocName();

    {
	char msg[300];
	vNoticeDialog note(theApp);
	strcpy(msg,"Printing to: \n");
	if (strcmp(dname,"|lpr") == 0)
	    strcat(msg,"Printer (lpr)");
	else if (dname[0] == '|')
	  {
	    strcat(msg,"pipe: "); strcat(msg,dname);
	  }
	else
	  {
	    strcat(msg,"file: "); strcat(msg,dname);
	  }

	note.Notice(msg);
    }
    
    const char* port = (_printer.GetPortrait()) ? "Portrait\n" : "Landscape\n";
    char* papername = _printer.GetPaperName();

    *pstream 	<< "%!PS-Adobe-2.0\n"
		<< "%Creator: V\n"
		<< "%%CreationDate: " << date << " - " << time << "\n"
		<< "%%Orientation: "
		<< port
		<< "%%DocumentPaperSizes: " << papername << "\n"
		<< "%%Pages: (atend)\n"
		<< "%%PageOrder: Ascend\n"
		<< "%%Title: " << dname << "\n"
		<< "%%DocumentFonts: Courier\n"
		<< "%%EndComments\n";

    // Write the ellipse def
    *pstream <<
"/ellipsedict 8 dict def\n\
ellipsedict /mtrx matrix put\n\
/ellipse\n\
{ ellipsedict begin\n\
  /endangle exch def\n\
  /startangle exch def\n\
  /yrad exch def\n\
  /xrad exch def\n\
  /y exch def\n\
  /x exch def\n\
  /savematrix mtrx currentmatrix def\n\
  x y translate\n\
  xrad yrad scale\n\
  0 0 1 startangle endangle arc\n\
  savematrix setmatrix\n\
  end\n\
  } def\n";

    // write other defs we need
    *pstream << "%EndProlog\n";

    _pages = 0;

    BeginPage();
    *pstream << "0.5 setlinewidth /Courier [10 0 0 -10 0 0] selectfont\n";	// make sure we have a font set
    return 1;
     
  }

//================>>> vPostScriptDC::BeginPage <<<========================
  void vPostScriptDC::BeginPage()
  {
    if (!pstream)
	return;
    ++_pages;		// bump number of pages so far

    // Put out the %%Page directive
    *pstream << "%%Page " << _pages << " " << _pages << "\n";
    *pstream << "initgraphics";
    if (!_printer.GetPortrait())
	*pstream << " 90 rotate";
    *pstream << " [1 0 0 -1 0 ";
    if (_printer.GetPortrait())
	*pstream << _physHeight;
    else
  	*pstream << "0";
    *pstream  << "] concat\n";
  }

//================>>> vPostScriptDC::EndPage <<<========================
  void vPostScriptDC::EndPage()
  {
    if (!pstream)
	return;
    *pstream << "showpage\n";
  }

//================>>> vPostScriptDC::EndPrinting <<<========================
  void vPostScriptDC::EndPrinting()
  {
    if (!pstream)
	return;

    EndPage();

    *pstream << "%%Trailer\n"
		<< "%%Pages: " << _pages << "\n"
		<< "%%EOF\n";

    _printer.ClosePrinter();	// close the printer
    pstream = 0;

  }

//=====================>>> vPostScriptDC::Clear <<<==========================
  void vPostScriptDC::Clear(void)
  {
    // no op
  }

//==================>>> vPostScriptDC::ClearRect <<<==========================
  void vPostScriptDC::ClearRect(int x, int y, int width, int height)
  {
    // no op
  }

//==================>>> vPostScriptDC::DrawColorPoints <<<======================
  void vPostScriptDC::DrawColorPoints(int x, int y, int nPoints, vColor* pointList)
  {
    // Draw a bunch of color points. 

    char hexval[4];

    if (nPoints < 1)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    unsigned char bval;
	
    // This code adapted from 24 bit icon code

    *pstream << "\ngsave 32 dict begin\n/pix24 "
	    << nPoints * 3 << " string def\n"
	    << xx << " " << yy << " translate "
	    << nPoints << " " << 1 << " scale\n"
	    << nPoints << " " << 1 << " 8 ["
	    << nPoints << " 0 0 " << 1 << " 0 0]\n"
	    << "{currentfile pix24 readhexstring pop} false 3 colorimage\n";

    for (int c8 = 0 ; c8 < nPoints ; c8++)
      {
	bval = pointList[c8].r();	// red
	ByteToStr(bval,hexval);
	*pstream << hexval;

	bval = pointList[c8].g();	// green
	ByteToStr(bval,hexval);
	*pstream << hexval;

	bval = pointList[c8].b();	// blue
	ByteToStr(bval,hexval);
	*pstream << hexval;
      }

    *pstream << "\nend grestore\n";

  }

//====================>>> vPostScriptDC::DrawEllipse <<<==========================
  void vPostScriptDC::DrawEllipse(int x, int y, int width, int height)
  {
    static const int angle = (360 * 64);
    
    if (!pstream)
	return;

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	*pstream << "newpath\n"
		 << (xx + (w / 2)) << " " << (yy + (h / 2)) << " "
		 << (w / 2) << " " << (h / 2) << " 0 360 ellipse\n"
		 << "fill\n";
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	*pstream << "newpath\n"
		 << (xx + (w / 2)) << " " << (yy + (h / 2))  << " "
		 << (w / 2) << " " << (h / 2) << " 0 360 ellipse\n"
		 << "stroke\n";
      }
  }

//====================>>> vPostScriptDC::DrawIcon <<<==========================
  void vPostScriptDC::DrawIcon(int x, int y, vIcon& icon)
  {

    // For now, we will only deal with monochrome bitmaps
    static unsigned char Xmask[] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};
    static unsigned char Pmask[] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
    char hexval[4];

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    unsigned char* from = icon.icon;

    if (!pstream)
	return;

    // algorithm is simple: reverse bit order, then invert

    if (icon.depth == 8)
      {
	unsigned char red[256], green[256], blue[256], bval;
	
	unsigned int numColors = (unsigned int) (*from++) + 1; // how many colors

	unsigned int cx;

        for (unsigned int ix = 0 ; ix < numColors ; ++ix) // map the colors
          {
             red[ix] = *from++; green[ix] = *from++; blue[ix] = *from++;
          }      

	*pstream << "%icon8\ngsave 32 dict begin\n/pix8 "
	    << icon.width*3 << " string def\n"
	    << xx << " " << yy << " translate "
	    << icon.width << " " << icon.height << " scale\n"
	    << icon.width << " " << icon.height << " 8 ["
	    << icon.width << " 0 0 " << icon.height << " 0 0]\n"
	    << "{currentfile pix8 readhexstring pop} false 3 colorimage\n";

	for (int r8 = 0 ; r8 < icon.height ; ++r8)
	  {
            for (int c8 = 0 ; c8 < icon.width ; c8++)
              {
                cx = (unsigned int)*from++;	// fetch the color

		bval = red[cx];
		ByteToStr(bval,hexval);
		*pstream << hexval;

		bval = green[cx];
		ByteToStr(bval,hexval);
		*pstream << hexval;

		bval = blue[cx];
		ByteToStr(bval,hexval);
		*pstream << hexval;
              }
	    *pstream << "\n";		// new line per row
	  }
	*pstream << "end grestore %end icon\n";
      }
    else if (icon.depth == 24)
      {
	unsigned char bval;
	
	*pstream << "%icon24\ngsave 32 dict begin\n/pix24 "
	    << icon.width*3 << " string def\n"
	    << xx << " " << yy << " translate "
	    << icon.width << " " << icon.height << " scale\n"
	    << icon.width << " " << icon.height << " 8 ["
	    << icon.width << " 0 0 " << icon.height << " 0 0]\n"
	    << "{currentfile pix24 readhexstring pop} false 3 colorimage\n";

	for (int r8 = 0 ; r8 < icon.height ; ++r8)
	  {
            for (int c8 = 0 ; c8 < icon.width ; c8++)
              {
                bval = (unsigned int)*from++;	// red
		ByteToStr(bval,hexval);
		*pstream << hexval;

                bval = (unsigned int)*from++;	// green
		ByteToStr(bval,hexval);
		*pstream << hexval;

                bval = (unsigned int)*from++;	// blue
		ByteToStr(bval,hexval);
		*pstream << hexval;
              }
	    *pstream << "\n";		// new line per row
	  }
	*pstream << "end grestore %end icon\n";
      }
    else if (icon.depth == 1)
      {
	unsigned char X, P;
	int rowbytes = icon.width / 8;	// number of bytes
	if ((icon.width & 7) != 0)
	    ++rowbytes;		// bytes padded, need to bump

	*pstream << "%icon\ngsave 32 dict begin\n/pix "
	    << rowbytes << " string def\n"
	    << xx << " " << yy << " translate "
	    << icon.width << " " << icon.height << " scale\n"
	    << icon.width << " " << icon.height << " 1 ["
	    << icon.width << " 0 0 " << icon.height << " 0 0]\n"
	    << "{currentfile pix readhexstring pop} image\n";


	for (int row = 0 ; row < icon.height ; ++row)	// copy by row
	  {
	    for (int col = 0 ; col < rowbytes ; ++col)
	      {
		X = *from++;		// copy of X byte
		P = 0;
		for (int ix = 0 ; ix < 8 ; ++ix) // reverse bits
		  {
		    if (X & Xmask[ix])	// masks to invert bits
			P |= Pmask[ix];
		  }
		P = ~P;			// invert
		ByteToStr(P,hexval);
		*pstream << hexval;
	      }
	    *pstream << "\n";		// new line per row
	  }
	*pstream << "end grestore %end icon\n";
      }
    else		// just draw a rectangle for color icons
      {
	DrawRectangle(x,y,icon.width,icon.height);
	return;
      }
  }

//====================>>> vPostScriptDC::DrawLine <<<==========================
  void vPostScriptDC::DrawLine(int x, int y, int xend, int yend)
  {
    // Draw a line from x,y to xend,yend

    if (!pstream)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int xe = Scale(xend+_tx);
    int ye = Scale(yend+_ty);

    SetGCtoPen();

    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;
    if (xe < 0) xe = 0;
    if (ye < 0) ye = 0;		// X hates negative coords

    *pstream 	<< "newpath "
		<< xx << " " << yy << " moveto "
		<< xe << " " << ye << " lineto stroke\n";
  }

//====================>>> vPostScriptDC::DrawLines <<<==========================
  void vPostScriptDC::DrawLines(vLine* lineList, int count)
  {
    // Draw a line from x,y to xend,yend

    if (!pstream)
	return;

    vLine* lp = lineList;
    SetGCtoPen();
    
    for (int num = 0 ; num < count ; ++lp, ++num)
      {
	int xx = Scale(lp->x+_tx);
	int yy = Scale(lp->y+_ty);
	int xe = Scale(lp->xend+_tx);
	int ye = Scale(lp->yend+_ty);


	if (xx < 0) xx = 0;
	if (yy < 0) yy = 0;
	if (xe < 0) xe = 0;
	if (ye < 0) ye = 0;		// X hates negative coords

        *pstream << "newpath "
		<< xx << " " << yy << " moveto "
		<< xe << " " << ye << " lineto stroke\n";
      }
  }

//====================>>> vPostScriptDC::DrawLines <<<==========================
  void vPostScriptDC::DrawLines(vPoint* points, int n)
  {
    // Draw a poly line contecting each point

    if (!pstream)
	return;

    vPoint* xpoints;
    int lastX, lastY;

    if (n <= 0)
	return;

    SetGCtoPen();

    if (_hasScale || _tx != 0 || _ty != 0) // If we have to scale, then we need to copy
      {
	xpoints = new vPoint[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;	// scale
	    xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
	  }
      }
    else
      {
	xpoints = points;	// just use original values
      }


    *pstream << "newpath\n"
	     << xpoints[0].x << " " << xpoints[0].y  << " moveto\n";

    lastX = xpoints[0].x;
    lastY = xpoints[0].y;
    for (int ix = 1; ix < n ; ++ix)
      {
	int x, y;

	// don't plot identical plots
	// this can save lots of disk space for large lines

	x = xpoints[ix].x;
	y = xpoints[ix].y;

	if (x != lastX || y != lastY)
	    *pstream << x << " " << y  << " lineto\n";

	lastX = x;
	lastY = y;
      }

    *pstream << "stroke\n";

    if (_hasScale)
	delete[]xpoints;
  }

//==================>>> vPostScriptDC::DrawPoint <<<======================
  void vPostScriptDC::DrawPoint(int x, int y)
  {

    if (!pstream)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;

    SetGCtoPen();
    *pstream 	<< "newpath\n"
		<< xx << " " << yy << " moveto "
		<< xx+1 << " " << yy << " lineto\nstroke\n";

  }

//==================>>> vPostScriptDC::DrawPoints <<<======================
  void vPostScriptDC::DrawPoints(vPoint* pointList, int count)
  {


    if (!pstream)
	return;

    vPoint* pl = pointList;

    SetGCtoPen();

    for (int num = 0 ; num < count ; ++pl, ++num)
      {
	int xx = Scale(pl->x+_tx);
	int yy = Scale(pl->y+_ty);

	if (xx < 0) xx = 0;
	if (yy < 0) yy = 0;

	*pstream 	<< "newpath\n"
		<< xx << " " << yy << " moveto "
		<< xx+1 << " " << yy << " lineto\nstroke\n";
      }

  }

//==================>>> vPostScriptDC::DrawPolygon <<<======================
  void vPostScriptDC::DrawPolygon(int n, vPoint points[], int fillStyle)
  {
    // draw a complete polygon (starting point specified twice!)

    if (!pstream)
	return;

    vPoint* xpoints;

    if (n <= 0)
	return;

    if (_hasScale || _tx != 0 || _ty != 0) // If we have to scale, then we need to copy
      {
	xpoints = new vPoint[n];	// allocate space for copy
	for (int i = 0; i < n; i++)
	  {
	    xpoints[i].x = ((points[i].x+_tx) * _Mult) / _Div;	// scale
	    xpoints[i].y = ((points[i].y+_ty) * _Mult) / _Div;
	  }
      }
    else
      {
	xpoints = points;	// just use original values
      }


    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	*pstream << "newpath\n"
		<< xpoints[0].x << " " << xpoints[0].y  << " moveto\n";

 	for (int ix = 1; ix < n ; ++ix)
	  {
	    *pstream << xpoints[ix].x << " " << xpoints[ix].y  << " lineto\n";
	  }
	*pstream << "closepath fill\n";

      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
					// xpoints2
	*pstream << "newpath\n"
		<< xpoints[0].x << " " << xpoints[0].y  << " moveto\n";

 	for (int ix = 1; ix < n ; ++ix)
	  {
	    *pstream << xpoints[ix].x << " " << xpoints[ix].y  << " lineto\n";
	  }
	*pstream << "closepath stroke\n";
      }

    if (_hasScale)
	delete[]xpoints;
}

//==================>>> vPostScriptDC::DrawRectangle <<<======================
  void vPostScriptDC::DrawRectangle(int x, int y, int width, int height)
  {

    if (!pstream)
	return;

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	*pstream << "newpath\n"
		<< xx << " " << yy << " moveto\n"
		<< (xx+w) << " " << yy << " lineto\n"
		<< (xx+w) << " " << (yy+h) << " lineto\n"
		<< xx << " " << (yy+h) << " lineto\n"
		<< "closepath fill\n";
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	*pstream << "newpath\n"
		<< xx << " " << yy << " moveto\n"
		<< (xx+w) << " " << yy << " lineto\n"
		<< (xx+w) << " " << (yy+h) << " lineto\n"
		<< xx << " " << (yy+h) << " lineto\n"
		<< "closepath stroke\n";
      }
  }

//==================>>> vPostScriptDC::DrawRectangles <<<======================
  void vPostScriptDC::DrawRectangles(vRect* rectList, int count)
  {

    if (!pstream)
	return;

    if (_pen.penStyle != vTransparent)
	SetGCtoPen();
    if (_brush.brushStyle != vTransparent)
	SetGCtoBrush();

    vRect* rp = rectList;

    for (int num = 0 ; num < count ; ++count, ++rp, ++num)
      {
	int xx = Scale(rp->x+_tx);
	int yy = Scale(rp->y+_ty);
	int w = Scale(rp->w);
	int h = Scale(rp->h);

	if (h == 0 || w == 0)	// ignore 0 h/w
	    continue;

	if (w < 0)		// convert to something we like
	  {
	    w = -w;		// make a positive width
	    xx = xx - w;	// translate x
	  }
	if (h < 0)
	  {
	    h = -h;		// positive h
	    yy = yy - h;	// translate x
	  }

	if (_brush.brushStyle != vTransparent)
	  {
	    *pstream << "newpath\n"
	    << xx << " " << yy << " moveto\n"
	    << (xx+w) << " " << yy << " lineto\n"
	    << (xx+w) << " " << (yy+h) << " lineto\n"
	    << xx << " " << (yy+h) << " lineto\n"
	    << "closepath fill\n";
	  }

	if (_pen.penStyle != vTransparent)
	  {
	    *pstream << "newpath\n"
	    << xx << " " << yy << " moveto\n"
	    << (xx+w) << " " << yy << " lineto\n"
	    << (xx+w) << " " << (yy+h) << " lineto\n"
	    << xx << " " << (yy+h) << " lineto\n"
	    << "closepath stroke\n";
	  }
      }
  }

//================>>> vPostScriptDC::DrawRoundedRectangle <<<===================
  void vPostScriptDC::DrawRoundedRectangle(int x, int y,
	int width, int height, int radius)
  {
    if (!pstream)
	return;

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);
    int w = Scale(width);
    int h = Scale(height);
    int r;

    if (w < 0)		// convert to something we like
      {
	w = -w;		// make a positive width
	xx = xx - w;	// translate x
      }
    if (h < 0)
      {
	h = -h;		// positive h
	yy = yy - h;	// translate x
      }

    if (radius < 0)
      {
	// Negative radius means divide average of hight and width
	// by this
	r = ((w+h)/(-2 * radius));
      }
    else
	r = Scale(radius);

    int cd = r + r;

    if (_brush.brushStyle != vTransparent)
      {
	SetGCtoBrush();
	// Because we reflect ourselves, this ends up clockwise...

	*pstream << "newpath\n";
	*pstream << xx + r << " " << yy + r << " " << r << " 180 270 arc\n";
	*pstream << xx + w - r << " " << yy  << " lineto\n";

	*pstream << xx + w - r << " " << yy + r << " " << r << "  270 0 arc\n";
	*pstream << xx + w  << " " << yy + h - r << " lineto\n";

	*pstream << xx + w - r << " " << yy + h - r << " " << r << " 0 90 arc\n";
	*pstream << xx + r  << " " << yy + h << " lineto\n";

	*pstream << xx + r << " " << yy + h - r << " " << r << " 90 180 arc\n";
	*pstream << "closepath fill\n";
      }

    if (_pen.penStyle != vTransparent)
      {
	SetGCtoPen();
	*pstream << "newpath\n";
	*pstream << xx + r << " " << yy + r << " " << r << " 180 270 arc\n";
	*pstream << xx + w - r << " " << yy  << " lineto\n";

	*pstream << xx + w - r << " " << yy + r << " " << r << "  270 0 arc\n";
	*pstream << xx + w  << " " << yy + h - r << " lineto\n";

	*pstream << xx + w - r << " " << yy + h - r << " " << r << " 0 90 arc\n";
	*pstream << xx + r  << " " << yy + h << " lineto\n";

	*pstream << xx + r << " " << yy + h - r << " " << r << " 90 180 arc\n";
	*pstream << "closepath stroke\n";
      }
  }

//==================>>> vPostScriptDC::DrawRubberEllipse <<<===================
  void vPostScriptDC::DrawRubberEllipse(int x, int y, int width, int height)
  {
    if (!pstream)
	return;

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;
    DrawEllipse(x, y, width, height);

  }

//==================>>> vPostScriptDC::DrawRubberLine <<<======================
  void vPostScriptDC::DrawRubberLine(int x, int y, int xend, int yend)
  {
    // Draw a rubber-band line from x,y to xend,yend. Redrawing
    // over the same with will erase it.

    if (!pstream)
	return;

    DrawLine(x, y, xend, yend);
  }

//==================>>> vPostScriptDC::DrawRubberPoint <<<======================
  void vPostScriptDC::DrawRubberPoint(int x, int y)
  {
    if (!pstream)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    if (xx < 0) xx = 0;
    if (yy < 0) yy = 0;

    DrawPoint(x, y);
  }

//==================>>> vPostScriptDC::DrawRubberRectangle <<<==================
  void vPostScriptDC::DrawRubberRectangle(int x, int y, int width, int height)
  {
    if (!pstream)
	return;

    if (height == 0 || width == 0)	// ignore 0 h/w
	return;

    DrawRectangle(x, y, width, height);
  }

//=====================>>> vPostScriptDC::DrawAttrText <<<==========================
  void vPostScriptDC::DrawAttrText(int x, int y, VCONST char* text, const ChrAttr attr)
  {
    // Draw text with attributes at given x, y.

    // The fonts may not be equal because we are using a shared GC,
    // and another window might change the font in the GC.  We
    // need to check the current font and change to our font if
    // it is not the same

    if (!pstream)
	return;

    static int mapColor[] =
      {
	vC_Black, vC_Blue, vC_Green, vC_Cyan,
        vC_Red, vC_Magenta, vC_Yellow, vC_White,
        vC_DarkGray, vC_DimBlue, vC_DimGreen, vC_DimCyan,
	vC_DimRed, vC_DimMagenta, vC_DimYellow, vC_MedGray
      };

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    // @@@ Note: Attributes are IGNORED!!!! This should be
    // fixed some day.

    DrawText(x,y,text);
  }

//=====================>>> vPostScriptDC::DrawText <<<==========================
  void vPostScriptDC::DrawText(int x, int y, VCONST char* text)
  {
    // simple draw text at given x, y

    // The fonts may not be equal because we are using a shared GC,
    // and another window might change the font in the GC.  We
    // need to check the current font and change to our font if
    // it is not the same

    if (!pstream || !text || !*text)
	return;

    int xx = Scale(x+_tx);
    int yy = Scale(y+_ty);

    SetGCtoPen();

    // @@ Note: This text output is the equivalent to drawin
    // in transparent mode - the background is not set
    // to the current background! Thus usually doesn't
    // matter, but should be fixed someday!

    if (_font.GetAngle() == 90)
	*pstream << xx << " " << yy << " moveto -90 rotate (";
    else
	*pstream << xx << " " << yy << " moveto (";

    for (VCONST char *cp = text ; *cp ; ++cp)
      {
	if (*cp == ')' || *cp == '(' || *cp == '\\')
	  {
	    *pstream << "\\";
	  }
	*pstream << *cp;
      }
    if (_font.GetAngle() == 90)
	*pstream << ") show 90 rotate\n";
    else
	*pstream << ") show\n";
    if (_font.GetUnderlined())
      {
        int width = TextWidth(text);
	if (_font.GetAngle() == 90)
	  {
	     *pstream  << "newpath "
                << xx + 1 << " " << yy << " moveto "
                << xx + 1 << " " << yy + width << " lineto stroke\n";
	  }
	else
	  {
	    *pstream  << "newpath "
		<< xx << " " << yy + 1 << " moveto "
		<< xx + width << " " << yy + 1 << " lineto stroke\n";
	  }
      }        
  }

//================>>> vPostScriptDC::SetBackground <<<==========================
  void vPostScriptDC::SetBackground(vColor& color)
  {
    if (!pstream)
	return;
	// @@
  }

//=====================>>> vPostScriptDC::SetBrush <<<============================
 void vPostScriptDC::SetBrush(vBrush& brush)
  {
    _brush = brush;
  }

//=====================>>> vPostScriptDC::SetPrinter <<<============================
 void vPostScriptDC::SetPrinter(vPrinter& printer)
  {
    _printer = printer;
    if (_printer.GetPortrait())
      {
	_physHeight = _printer.GetHeight();
	_physWidth = _printer.GetWidth();
      }
    else
      {
	_physWidth = _printer.GetHeight();
	_physHeight = _printer.GetWidth();
      }
  }

//=====================>>> vPostScriptDC::SetPSColor <<<========================
  void vPostScriptDC::SetPSColor(vColor& bc)
  {

    if (bc == _curColor)	// don't need to change colors?
	return;

    _curColor = bc;

    int red = bc.r();		// retrieve color components
    int green = bc.g();
    int blue = bc.b();

    float psRed = (float)(red / 255.0);	// convert to 0 to 1.0
    float psGreen = (float)(green / 255.0);	// convert to 0 to 1.0
    float psBlue = (float)(blue / 255.0);	// convert to 0 to 1.0

    if (_printer.GetUseColors())		// is it a color printer?
      {
	*pstream << psRed << " " << psGreen << " " << psBlue << " setrgbcolor\n";
      }
    else			// use DeviceRGB to DeviceGray conversion
      {				// as described in PostScript Red book
	float gray = (.3*psRed) + (.59*psGreen) + (.11*psBlue);
	*pstream << gray << " setgray\n";
      }
  }

//=====================>>> vPostScriptDC::SetGCtoBrush <<<========================
  void vPostScriptDC::SetGCtoBrush()
  {

    // Must check color first since PS "brush and pen" share color

    
    vColor bc = _brush.GetColor();
    SetPSColor(bc);		// set up color

    if (_curBrush == _brush)			// no-op if no change
	return;

    switch (_brush.brushStyle)
      {
	case vSolid:
	    break;

	case vTransparent:
	    break;

	case vHorizontalHatch:
	  {
	    break;
	  }

	case vVerticleHatch:
	  {
	    break;
	  }

	case vLeftDiagonalHatch:
	  {
	    break;
	  }

	case vRightDiagonalHatch:
	  {
	    break;
	  }

	case vCrossHatch:
	  {
	    break;
	  }

	case vDiagonalCrossHatch:
	  {
	    break;
	  }
      }
    _curBrush = _brush;
  }

//=====================>>> vPostScriptDC::SetGCtoPen <<<========================
  void vPostScriptDC::SetGCtoPen()
  {

    vColor pc = _pen.GetColor();
    SetPSColor(pc);

    if (_curPen == _pen && _Mult == 1 && _Div == 1)	// no-op if no change
	return;						// and not scaled

    if (_curPen.GetWidth() != _pen.GetWidth()	   	// if change width
	  || _Mult !=1 || _Div != 1)			// or scaled
      {
	// Using standard PS points, a width of 1 looks too
	// big. A width of .5 looks better.

	float penWid = .5;				// default case
	if (_pen.GetWidth() != 1)
	    penWid = (float) _pen.GetWidth();
	
	*pstream << penWid << " setlinewidth\n";
      }

    if (_curPen.GetStyle() != _pen.GetStyle())	// change style
      {
	switch (_pen.GetStyle())
	  {
	    case vSolid:
	    case vTransparent:
		*pstream << "[] 0 setdash\n";
		break;

	    case vDash:
		*pstream << "[6] 0 setdash\n";
		break;

	    case vDot:
		*pstream << "[2 6] 0 setdash\n";
		break;

	    case vDashDot:
		*pstream << "[6 6 2 6] 0 setdash\n";
		break;

	    default:
		*pstream << "[] 0 setdash\n";
		break;
         }
      }

    _curPen = _pen;

  }

//=====================>>> vPostScriptDC::SetPen <<<============================
 void vPostScriptDC::SetPen(vPen& pen)
  {
    _pen = pen;
  }

//======================>>> vPostScriptDC::SetFont <<<===========================
  void vPostScriptDC::SetFont(vFont& vf)
  {
    // Change the font associated with this window.

    _font = vf;

   // now emit PostScript Level 2 font selection command:
   //  "/Fontname" "pointsize" selectfont

    if (!pstream)
	return;

    vFontID style = _font.GetStyle();
    vFontID weight = _font.GetWeight();
    vFontID family = _font.GetFamily();

    int fontIndex = fCourier;	// default

    switch (family)
      {
	case vfDefaultSystem:
	case vfDefaultFixed:
	    fontIndex = fCourier;
	    break;

	case vfDefaultVariable:
	    fontIndex = fHelv;
	    break;

	case vfSerif:                   // serifed font - TimesRoman
	case vfSerif2:                   // serifed font - TimesRoman
	    if (style == vfItalic)
	      {
		if (weight == vfBold)
		    fontIndex = fTimesBoldItal;
		else
		    fontIndex = fTimesItal;
		    
	      }
	    else if (weight == vfBold)
		fontIndex = fTimesBold;
	    else
		fontIndex = fTimesRoman;
            break;

        case vfSansSerif:               // SansSerif - Swiss or Helvetica
        case vfSansSerif2:               // SansSerif - Swiss or Helvetica
	    if (style == vfItalic)
	      {
		if (weight == vfBold)
		    fontIndex = fHelvBoldObl;
		else
		    fontIndex = fHelvObl;
	      }
	    else if (weight == vfBold)
		fontIndex = fHelvBold;
	    else
		fontIndex = fHelv;
            break;

        case vfFixed:           // fixed font - Courier
        case vfFixed2:           // fixed font - Courier
	    if (style == vfItalic)
	      {
		if (weight == vfBold)
		    fontIndex = fCourierBoldObl;
		else
		    fontIndex = fCourierObl;
	      }
	    else if (weight == vfBold)
		fontIndex = fCourierBold;
	    else
		fontIndex = fCourier;
            break;

        case vfDecorative:              // decorative - dingbat
        case vfDecorative2:              // decorative - dingbat
	    fontIndex = fSymbol;
            break;

        default:
	    fontIndex = fCourier;
            break; 
      }

    int ptsz = _font.GetPointSize();

    _fontIndex = fontIndex;		// remember current font index
    *pstream << "/" << fontNames[fontIndex]
	     << " [" << ptsz << " 0 0 -" << ptsz << " 0 0] selectfont\n";
  }

//====================>>> vPostScriptDC::TextHeight <<<=============================
  int vPostScriptDC::TextHeight(int& asc, int& des)
  {
    // Return total height of this font. V will use total height, which
    // is most often made up of ascent + descent.  This is too much
    // detail for the kind of apps V will support.

    int points = _font.GetPointSize();
    asc = points - 1; des = 1;
    return points;
  }

//========================>>> vPostScriptDC::TextWidth <<<==========================
  int vPostScriptDC::TextWidth(VCONST char* str)
  {
    int points = _font.GetPointSize();
    long width;

    if (_fontIndex >= fCourier)	// fixed point font
      {
	width = (long) ((points * strlen(str)) * .6);
	return (int) (width > 0) ? width : 1;
      }
    if (_fontIndex < 0)
	return 0;
    
    width = 0;
    for (unsigned char* cp = (unsigned char*) str ; *cp ; ++cp)
      {
	width += fontW[_fontIndex][(short)*cp];
      }

    return (points * width) / 1000;
  }
