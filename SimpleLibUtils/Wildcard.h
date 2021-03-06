//////////////////////////////////////////////////////////////////////
//
// SimpleLib Utils Version 1.0
//
// Copyright (C) 1998-2007 Topten Software.  All Rights Reserved
// http://www.toptensoftware.com
//
// This code has been released for use "as is".  Any redistribution or 
// modification however is strictly prohibited.   See the readme.txt file 
// for complete terms and conditions.
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// Wildcard.h - declaration of Wildcard functions

#ifndef __WILDCARD_H
#define __WILDCARD_H


namespace Simple
{

bool SIMPLEAPI DoesWildcardMatch(const wchar_t* pszSpec, const wchar_t* pszString);

}	// namespace Simple

#endif	// __WILDCARD_H

