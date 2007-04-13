//===============================================================
// vslist.h - The vSList class - Windows
//
// Copyright (C) 1995,1996  Bruce E. Wampler
//
// This file is part of the V C++ GUI Framework, and is covered
// under the terms of the GNU Library General Public License,
// Version 2. This library has NO WARRANTY. See the source file
// vapp.cxx for more complete information about license terms.
//===============================================================

#ifndef VSLIST_H
#define VSLIST_H

// define V_EXPORT for X version
#ifndef V_EXPORT
#define V_EXPORT
#endif

    class V_EXPORT vSList
      {

      public:		//---------------------------------------- public
	vSList(int maxsize = 128);
	~vSList();

//  	vSList& operator =(const vSList& slist);
  	void init();
	void erase();
	int size() const;
	int insert(int insAt, const char* strn) const;
	int replace(int repAt, const char* strn) const;
	int deleteItem(int delAt) const;

	int max;
	char** list;

      protected:	//--------------------------------------- protected

      private:		//--------------------------------------- private

  };

#endif
