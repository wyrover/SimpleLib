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
// ProfileFile.h - declaration of CProfileFile class

#ifndef __PROFILEFILE_H
#define __PROFILEFILE_H

namespace Simple
{

class CProfileEntry;
class CProfileSection;
class CProfileFile;
class CCppTokenizer;
class CContentProvider;

// Owned of profile entries and sections
class CProfileOwner
{
public:
	CProfileOwner() {};
	virtual ~CProfileOwner() {};

	virtual CProfileSection* AsSection() { return NULL; };
	virtual CProfileFile* GetOwningFile() { return NULL; };
	virtual const wchar_t* GetModeInternal()=0;
	virtual void OnModified()=0;
};

// CProfileEntry Class
class CProfileEntry
{
public:
// Construction
	CProfileEntry(CProfileSection* pOwner, const CUniString& strName, const CUniString& strValue=NULL);
	CProfileEntry(CProfileSection* pOwner, const CProfileEntry& Other);

// Operations
	const wchar_t* GetName() const;
	void SetName(const CUniString& strName);
	const wchar_t* GetValue() const;
	void SetValue(const CUniString& strValue);
	int GetIntValue(int iDefault) const;
	void SetIntValue(int iValue);
	int64_t GetInt64Value(int64_t iDefault) const;
	void SetInt64Value(int64_t iValue);
	double GetDoubleValue(double dblDefault) const;
	void SetDoubleValue(double dbl);
	size_t GetItemData() const;
	void SetItemData(size_t size_t);

	CProfileSection* GetOwningSection() { return m_pOwner; };
	CProfileFile* GetOwningFile();
	void OnModified();

protected:
	CUniString			m_strName;
	CUniString			m_strValue;
	size_t				m_lParam;
	CProfileSection*	m_pOwner;

	friend class CProfileFile;
	friend class CProfileSection;

private:
	CProfileEntry(const CProfileEntry& Other);
};

// CProfileSection Class
class CProfileSection :
	public CVector<CProfileEntry*, SOwnedPtr>,
	public CProfileOwner
{
public:
// Construction
	CProfileSection(CProfileOwner* pOwner, const CUniString& strName, bool bFlat);
	CProfileSection(CProfileOwner* pOwner, const CProfileSection& Other);

// Operations
	const wchar_t* GetName() const;
	void SetName(const CUniString& strName);
	int FindEntryIndexRaw(const wchar_t* pszName, int iStartAfter=-1) const;
	int FindEntryIndex(const wchar_t* pszName, int iStartAfter=-1) const;
	CProfileEntry* FindEntry(const wchar_t* pszName) const;
	size_t GetItemData() const;
	void SetItemData(size_t size_t);

	CProfileEntry* CreateEntry(const CUniString& pszName, const CUniString& strValue=NULL);

	bool DeleteValue(const wchar_t* pszName);
	void SetValue(const CUniString& strName, const CUniString& strValue);
	const wchar_t* GetValue(const wchar_t* pszName, const wchar_t* pszDefault) const;

	int GetIntValue(const wchar_t* pszName, int iDefault) const;
	void SetIntValue(const wchar_t* pszName, int iValue);
	int64_t GetInt64Value(const wchar_t* pszName, int64_t iDefault) const;
	void SetInt64Value(const wchar_t* pszName, int64_t iValue);
	double GetDoubleValue(const wchar_t* pszName, double dblDefault) const;
	void SetDoubleValue(const wchar_t* pszName, double dblValue);

// Sub-sections
	int FindSectionIndexRaw(const wchar_t* pszName, int iStartAfter=-1) const;
	int FindSectionIndex(const wchar_t* pszName, int iStartAfter=-1) const;
	bool DeleteSection(const wchar_t* pszName);
	CProfileSection* FindSection(const wchar_t* pszName) const;
	CProfileSection* CreateSection(const wchar_t* pszName);
	int GetSubSectionCount() const;
	CProfileSection* GetSubSection(int iIndex) const;

	void CopyFrom(const CProfileSection* pSection);
	void AddSection(const CProfileSection* pSection);

	CProfileSection* GetOwningSection() { return m_pOwner->AsSection(); };
	CProfileFile* GetOwningFile() { return m_pOwner ? m_pOwner->GetOwningFile() : NULL; }

	virtual CProfileSection* AsSection() { return this; };
	virtual const wchar_t* GetModeInternal() { return m_pOwner->GetModeInternal(); };
	virtual void OnModified() { if (m_pOwner) m_pOwner->OnModified(); }

protected:
	CProfileOwner*	m_pOwner;
	bool			m_bFlat;
	CUniString		m_strName;
	size_t			m_lParam;
	CVector<CProfileSection*, SOwnedPtr>
					m_Sections;

	friend class CProfileFile;

private:
	CProfileSection(const CProfileSection& Other);
};

// CProfileFile Class
class CProfileFile :
		public CVector<CProfileSection*, SOwnedPtr>,
		public CProfileOwner
{
public:
// Construction
			CProfileFile();
	virtual ~CProfileFile();

// Operations
	void Define(const wchar_t* pszName, const wchar_t* pszValue);
	void Reset(bool bFlat);
	bool Parse(const wchar_t* pszContent, const wchar_t* pszFileName=NULL, CContentProvider* pContentProvider=NULL);
	bool ParseClassic(const wchar_t* pszContent);
	bool ParseStructured(const wchar_t* pszContent, const wchar_t* pszFileName, CContentProvider* pContentProvider=NULL);
	CUniString GetParseError() const;
	bool Load(const wchar_t* pszFileName, CContentProvider* pContentProvider=NULL);
	bool Save(const wchar_t* pszFileName=NULL, bool bUnicode=false, bool bHeirarchial=false);
	bool SaveToString(CUniString& buf, bool bHeirarchial=false) const;
	void Merge(CProfileFile* pOther);
	void Merge(CProfileSection* pSection);
	int FindSectionIndexRaw(const wchar_t* pszName, int iStartAfter=-1) const;
	int FindSectionIndex(const wchar_t* pszName, int iStartAfter=-1) const;
	bool DeleteSection(const wchar_t* pszName);
	CProfileSection* FindSection(const wchar_t* pszName) const;
	CProfileSection* CreateSection(const CUniString& strName);
	CProfileEntry* FindEntry(const wchar_t* pszSection, const wchar_t* pszEntry) const;
	CProfileEntry* CreateEntry(const CUniString& strSection, const CUniString& strEntry);

	bool DeleteValue(const wchar_t* pszSection, const wchar_t* pszName);
	void SetValue(const CUniString& strSection, const CUniString& strName, const CUniString& strValue);
	const wchar_t* GetValue(const wchar_t* pszSection, const wchar_t* pszName, const wchar_t* pszDefault=NULL) const;
	void SetIntValue(const CUniString& strSection, const CUniString& strName, int iValue);
	int GetIntValue(const wchar_t* pszSection, const wchar_t* pszName, int iDefault=0) const;
	void SetInt64Value(const CUniString& strSection, const CUniString& strName, int64_t iValue);
	int64_t GetInt64Value(const wchar_t* pszSection, const wchar_t* pszName, int64_t iDefault=0) const;
	void SetDoubleValue(const CUniString& strSection, const CUniString& strName, double dblValue);
	double GetDoubleValue(const wchar_t* pszSection, const wchar_t* strName, double dblDefault=0) const;

	void SetMode(const wchar_t* pszMode);
	CUniString GetMode();
	CUniString GetFileName() { return m_strFileName; };
	void SetFileName(const wchar_t* psz) { m_strFileName=psz; };

	// Overrideable for creating/opening streams
	virtual bool CreateOrOpenStream(CProfileEntry* pEntry, bool bCreate, const wchar_t* pszSpec, struct IStream** pVal) { return false; };

protected:
	CUniString		m_strFileName;
	CUniString		m_strParseError;
	CUniString		m_strMode;
	bool			m_bFlat;
	CMap<CUniString, CUniString>	m_ExternalDefines;

	void WriteEntry(CUniString& buf, CProfileEntry* pEntry) const;
	void WriteSection(CUniString& buf, CProfileSection* pSection, const wchar_t* pszParent) const;
	void WriteSectionHeirarchial(CUniString& buf, CProfileSection* pSection, int iIndent) const;

	bool ParseStructuredContent(CCppTokenizer& tokens);

	virtual CProfileFile* GetOwningFile()
	{
		return this;
	}

	virtual const wchar_t* GetModeInternal()
	{
		return m_strMode;
	}

	virtual void OnModified()
	{
	}
};

}	// namespace Simple

#endif	// __PROFILEFILE_H


