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
// PathLibrary.cpp - implementation of PathLibrary

#include "StdAfx.h"
#include "SimpleLibUtilsBuild.h"

#include "PathLibrary.h"
#include "SplitString.h"

#include <sys/stat.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

namespace Simple
{

#ifdef _WIN32
#define PATH_SEPARATOR	'\\'
#else
#define PATH_SEPARATOR	'/'
#endif

bool SIMPLEAPI IsPathSeparator(wchar_t ch)
{
	return ch=='/' || ch=='\\';
}

// Remove the trailing backslash from a path (if any)
void SIMPLEAPI RemoveTrailingBackslash(wchar_t* pszPath)
{
	if (!pszPath)
		return;

	size_t iLen=CUniString::len(pszPath);
	if ((iLen>0) && IsPathSeparator(pszPath[iLen-1]))
		{
		pszPath[iLen-1]=L'\0';
		}
}

// Append two path strings, ensuring path separator character between them
CUniString SIMPLEAPI SimplePathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2)
{
	size_t iLen1=CUniString::len(pszPath1);
	size_t iLen2=CUniString::len(pszPath2);
	if (!iLen1)
		return pszPath2;
	if (!iLen2)
		return pszPath1;

	// Start with path 1
	CUniString str(pszPath1);

	// Make sure only one backslash
	RemoveTrailingBackslash(str.GetBuffer(0));
	str+=PATH_SEPARATOR;

	// Make sure path2 doesn't start with a backslash
	if (IsPathSeparator(pszPath2[0]))
		pszPath2++;

	// Append it
	str+=pszPath2;

	// Done!
	return str;
}

CUniString SIMPLEAPI CurrentDirectory()
{
#ifdef _MSC_VER
	CUniString str;
	_wgetcwd(str.GetBuffer(_MAX_PATH), _MAX_PATH);
	return str;
#else
	return a2w(get_current_dir_name());
#endif
}

// Fully qualify a path against the current working directory
CUniString SIMPLEAPI QualifyPath(const wchar_t* psz)
{
#ifdef _MSC_VER
	CUniString str;
	_wfullpath(str.GetBuffer(_MAX_PATH), psz, _MAX_PATH);
	return str;
#else
	return CanonicalPathAppend(a2w(get_current_dir_name()), psz);
#endif
}

// Find the extension of a file name
const wchar_t* SIMPLEAPI FindExtension(const wchar_t* pszPath)
{
	if (!pszPath)
		return NULL;

	// Get pointer to last character
	const wchar_t* p=pszPath+CUniString::len(pszPath)-1;

	while (p>=pszPath)
		{
		if (IsPathSeparator(p[0]))
			return NULL;
		if (p[0]=='.')
			return p;
		p--;
		}

	return NULL;
}

// Remove the extension from a file name
void SIMPLEAPI RemoveExtension(wchar_t* pszPath)
{
	wchar_t* p=const_cast<wchar_t*>(FindExtension(pszPath));
	if (p)
		p[0]=L'\0';
}



// Find the last element in a path
const wchar_t* SIMPLEAPI FindLastElement(const wchar_t* pszPath, bool bIgnoreTrailingSlash)
{
	if (!pszPath)
		return NULL;

	// Get pointer to last character
	const wchar_t* p=pszPath+CUniString::len(pszPath)-1;

	// Ignore very trailing path sep
	if (bIgnoreTrailingSlash && p>pszPath && IsPathSeparator(p[0]))
		p--;

	while (p>=pszPath)
		{
		if (IsPathSeparator(p[0]))
			return p;
		p--;
		}

	return pszPath;
}


// Remove the last element in a file name
void SIMPLEAPI RemoveLastElement(wchar_t* pszPath)
{
	wchar_t* p=const_cast<wchar_t*>(FindLastElement(pszPath, true));
	if (p)
	{
		p[0]='\0';
		if (p>pszPath && p[-1]==':')
		{
			p[0]=PATH_SEPARATOR;
			p[1]='\0';
		}
	}
}

bool SIMPLEAPI SplitPath(const wchar_t* pszPath, CUniString* pstrFolder, CUniString* pstrFileName)
{
	const wchar_t* pszLastElement=FindLastElement(pszPath, false);
	if (!pszLastElement || pszLastElement==pszPath)
	{
		if (pstrFolder)
			*pstrFolder=pszPath;
		return false;
	}

	if (pstrFileName)
		*pstrFileName=pszLastElement+1;

	if (pstrFolder)
		*pstrFolder=CUniString(pszPath, int(pszLastElement-pszPath));

	return true;
}

CUniString SIMPLEAPI ExtractFileTitle(const wchar_t* pszPath)
{
	const wchar_t* pszLastElement=FindLastElement(pszPath, false);
	if (!pszLastElement)
		return NULL;

	if (IsPathSeparator(pszLastElement[0]))
		pszLastElement++;

	CUniString strFileName=pszLastElement;
	RemoveExtension(strFileName.GetBuffer(0));
	return strFileName;
}


// Make a path canonical
CUniString SIMPLEAPI CanonicalizePath(const wchar_t* pszPath)
{
	if (!pszPath)
		return NULL;

	const wchar_t* p=pszPath;

	CUniString str;
	while (p[0])
	{
		// Copy everything up to backslash
		while (p[0] && !IsPathSeparator(p[0]))
		{
			str+=*p++;
		}

		// Found a separator?
		if (IsPathSeparator(p[0]))
		{
			// Skip it
			p++;

			if (p[0]!='.')
			{
				str+=PATH_SEPARATOR;
				continue;
			}

			while (p[0]=='.')
			{
				// Double dots?
				if (p[1]=='.' && (!p[2] || IsPathSeparator(p[2])))
				{
					// Remove the previous element
					RemoveLastElement(str.GetBuffer(0));

					// Skip it
					p+=2;

					// Path separator found after ".."?
					if (IsPathSeparator(p[0]))
					{
						RemoveTrailingBackslash(str.GetBuffer(0));
						str+=PATH_SEPARATOR;
						p++;
					}
				}
				else if (!p[1] || IsPathSeparator(p[1]))
				{
					// Ignore it
					p++;

					if (IsPathSeparator(p[0]))
					{
						str+=PATH_SEPARATOR;
						p++;
					}
				}
				else
				{
					str+=*p++;
				}
			}
		}
	}

	RemoveTrailingBackslash(str.GetBuffer(0));

	return str;
}

CUniString SIMPLEAPI CanonicalPathAppend(const wchar_t* pszPath1, const wchar_t* pszPath2)
{
	if (IsFullyQualified(pszPath2))
		return pszPath2;
	return CanonicalizePath(SimplePathAppend(pszPath1, pszPath2));
}


// Find the end of the drive or UNC part of a path
#ifdef _WIN32
const wchar_t* SIMPLEAPI FindEndOfDrive(const wchar_t* pszPath)
{
	const wchar_t* p=pszPath;

	// Drive letter?
	if ( ((p[0]>='A' && p[0]<='Z') || (p[0]>='a' && p[0]<='z')) && p[1]==L':')
		return pszPath+2;

	// UNC Path?
	if (p[0]==L'\\' && p[1]==L'\\')
		{
		// Skip leading backslash
		p+=2;

		// Skip machine name
		while (!IsPathSeparator(p[0]) && p[0])
			p++;

		// Found it?
		if (IsPathSeparator(p[0]))
			{
			// Skip backslash
			p++;

			// Skip share name
			while (!IsPathSeparator(p[0]) && p[0])
				p++;

			// Done
			return p;
			}
		}

	// Not a drive or UNC
	return NULL;
}
#endif


// Check if path contains drive or UNC part
bool SIMPLEAPI IsFullyQualified(const wchar_t* pszPath)
{
#ifdef _WIN32
	return FindEndOfDrive(pszPath)!=NULL;
#else
	return pszPath[0]=='/' || pszPath[0]=='~';
#endif
}

CUniString SIMPLEAPI FindFileOnSearchPath(CVector<CUniString>& vecPath, const wchar_t* pszFileName)
{
	if (IsEmptyString(pszFileName))
		return NULL;

	// Already fully qualified?
	if (IsFullyQualified(pszFileName))
		return pszFileName;

	// Split the path...
	for (int i=0; i<vecPath.GetSize(); i++)
	{
		CUniString strFile=CanonicalPathAppend(vecPath[i], pszFileName);
		if (DoesFileExist(strFile))
			return strFile;
	}

	return NULL;
}

bool SIMPLEAPI DoesFileExist(const wchar_t* pszFileName)
{
	if (IsEmptyString(pszFileName))
		return false;

#ifdef _MSC_VER
	struct _stat s;
	if (_wstat(pszFileName, &s))
		return false;

	return (s.st_mode & _S_IFREG)!=0;

#else
	struct stat s;
	if (stat(w2a(pszFileName), &s))
		return false;

	return S_ISREG(s.st_mode);
#endif

}

bool SIMPLEAPI DoesPathExist(const wchar_t* pszFileName)
{
	if (IsEmptyString(pszFileName))
		return false;

#ifdef _MSC_VER
	struct _stat s;
	if (_wstat(pszFileName, &s))
		return false;
	return (s.st_mode & _S_IFDIR)!=0;
#else
	struct stat s;
	if (stat(w2a(pszFileName), &s))
		return false;

	return S_ISDIR(s.st_mode);
#endif

}


CUniString SIMPLEAPI FindFileOnSearchPath(const wchar_t* pszSearchPath, const wchar_t* pszFileName)
{
	CVector<CUniString> vecPath;
	SplitString(pszSearchPath, L";", vecPath);
	return FindFileOnSearchPath(vecPath, pszFileName);
}


CUniString SIMPLEAPI ChangeFileName(const wchar_t* pszFileName, const wchar_t* pszNewFileName)
{
	CUniString strFolder;
	SplitPath(pszFileName, &strFolder, NULL);
	return SimplePathAppend(strFolder, pszNewFileName);
}

CUniString SIMPLEAPI ChangeFileExtension(const wchar_t* pszFileName, const wchar_t* pszNewExtension)
{
	const wchar_t* pszExt=FindExtension(pszFileName);
	if (!pszExt)
		pszExt=pszFileName+wcslen(pszFileName);

	CUniString str=CUniString(pszFileName, int(pszExt-pszFileName));
	str+=L".";
	str+=pszNewExtension;

	return str;
}

// Extract the drive letter or UNC machine/share part of a path
#ifdef _WIN32
CUniString SIMPLEAPI ExtractDrive(const wchar_t* pszPath)
{
	// Find end of drive
	const wchar_t* pszEndOfDrive=FindEndOfDrive(pszPath);
	if (!pszEndOfDrive)
		return NULL;

	return CUniString(pszPath, int(pszEndOfDrive-pszPath));
}
#endif


bool LenICompare(const wchar_t* psz1, int iLen1, const wchar_t* psz2, int iLen2)
{
	// Same length?
	if (iLen1!=iLen2)
		return false;

	return _wcsnicmp(psz1, psz2, iLen1)==0;
}

bool LenCompare(const wchar_t* psz1, int iLen1, const wchar_t* psz2, int iLen2)
{
	// Same length?
	if (iLen1!=iLen2)
		return false;

	return wcsncmp(psz1, psz2, iLen1)==0;
}

const wchar_t* SIMPLEAPI FindEndOfElement(const wchar_t* p)
{
	// Find end of part
	while (p[0] && !IsPathSeparator(p[0]))
		p++;

	return p;
}

// Find the relative path between two full paths
// Both pszPath1, and pszPath2 should be fully qualified paths
// Does support c:path	must be c:\path
// pszPath1 must be a folder, pszPath2 can be file or folder.
CUniString SIMPLEAPI FindRelativePath(const wchar_t* pszPath1, const wchar_t* pszPath2)
{
	if (!IsFullyQualified(pszPath1) || !IsFullyQualified(pszPath2))
		return pszPath2;

	// Make sure both paths are in canonical form
	CUniString strPath1=CanonicalizePath(pszPath1);
	CUniString strPath2=CanonicalizePath(pszPath2);
	pszPath1=strPath1;
	pszPath2=strPath2;

#ifdef _WIN32
	// Extract drives and ensure the same
	CUniString strDrive1=ExtractDrive(pszPath1);
	CUniString strDrive2=ExtractDrive(pszPath2);
	if (strDrive1.IsEmpty() || strDrive2.IsEmpty() || !IsEqualStringI(strDrive1, strDrive2))
		return pszPath2;

	// Skip drive part of each path
	pszPath1+=strDrive1.GetLength()+1;
	pszPath2+=strDrive2.GetLength()+1;
#endif

	// Skip common prefix
	while (true)
		{
		// Find end of next element in each path
		const wchar_t* pszNextElem1=FindEndOfElement(pszPath1);
		const wchar_t* pszNextElem2=FindEndOfElement(pszPath2);

		// Different folder name?
		#ifdef _WIN32
		if (!LenICompare(pszPath1, int(pszNextElem1-pszPath1), pszPath2, int(pszNextElem2-pszPath2)))
			break;
		#else
		if (!LenCompare(pszPath1, int(pszNextElem1-pszPath1), pszPath2, int(pszNextElem2-pszPath2)))
			break;
		#endif

		// Move to next element
		pszPath1=pszNextElem1;
		pszPath2=pszNextElem2;

		// Skip backslashes
		if (IsPathSeparator(pszPath1[0])) pszPath1++;
		if (IsPathSeparator(pszPath2[0])) pszPath2++;

		// Quit if end of either path
		if (pszPath1[0]==L'\0')
			break;
		if (pszPath2[0]==L'\0')
			break;
		}

	// If nothing left on path 1, path2 is relative automatically
	if (pszPath1[0]==L'\0')
		return pszPath2;

	// For each folder on path1, add a "..\"
	CUniString strRelative;
	while (pszPath1[0])
		{
		// Append "..\"
		strRelative.Append(Format(L"..%c", PATH_SEPARATOR));

		// Find end of next element in each path
		pszPath1=FindEndOfElement(pszPath1);

		// Skip backslash
		if (IsPathSeparator(pszPath1[0])) pszPath1++;
		}

	// Append rest of path2
	strRelative.Append(pszPath2);
	return strRelative;
}

CUniString SIMPLEAPI ExtractFolder(const wchar_t* pszFileName)
{
	CUniString strFolder;
	SplitPath(pszFileName, &strFolder, NULL);
	return strFolder;
}

CUniString SIMPLEAPI ExtractFileName(const wchar_t* pszFileName)
{
	CUniString strFileName;
	SplitPath(pszFileName, NULL, &strFileName);
	return strFileName;
}

}	// namespace Simple


