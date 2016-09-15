/*
 *  CDirectory
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#pragma once

class CDirectory : public CString {
public:
	CDirectory() { NULL; }
	explicit
	CDirectory(const CString &strPath)
		:CString(strPath) { NULL; }
	explicit
	CDirectory(LPCTSTR szPath)
		:CString(szPath) { NULL; }

	CDirectory &operator += (const CDirectory &rhs);

	
	void AddBackslash();
	BOOL AddExtension(LPCTSTR szExt);
	BOOL Append(LPCTSTR szMore);
	void BuildRoot(int iDrive);
	void Canonicalize();
	void Combine(LPCTSTR pszDir, LPCTSTR pszFile);
	CDirectory CommonPrefix(LPCTSTR pszOther);
	BOOL CompactPath(HDC hDC, UINT nWidth);
	BOOL CompactPathEx(UINT nMaxChars, DWORD dwFlags = 0);
	BOOL FileExists() const;
	int FindExtension() const;
	int FindFileName() const;
	int GetDriveNumber() const;
	
	CString GetExtension(BOOL flag = TRUE, BOOL custom=FALSE) const;
	CString GetFileName() const;
	CString GetFileNameWithExtension() const;
	CString GetFilePath() const;
	BOOL IsDirectory() const;
	BOOL IsFile() const;
	BOOL IsFileSpec() const;
	BOOL IsPrefix(LPCTSTR pszPrefix) const;
	BOOL IsRelative() const;
	BOOL IsRoot() const;
	BOOL IsSameRoot(LPCTSTR pszOther) const;
	BOOL IsUNC() const;
	BOOL IsUNCServer() const;
	BOOL IsUNCServerShare() const;
	BOOL MakePretty();
	BOOL MatchSpec(LPCTSTR pszSpec) const;
	BOOL RelativePathTo(LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo);
	void QuoteSpaces();
	void RemoveArgs();
	void RemoveBackslash();
	void RemoveBlanks();
	void RemoveExtension();
	BOOL RemoveFileSpec();
	BOOL RenameExtension(LPCTSTR szExt);
	int SkipRoot() const;
	BOOL SplitPath(CString *pstrDrive, CString *pstrDir, CString *pstrFile, CString *pstrExt);
	BOOL SplitPath(CString *pstrDir, CString *pstrFile);
	void StripPath();
	BOOL StripToRoot();
	BOOL ToFullPath();
	BOOL ToRelativePath();
	void UnquoteSpaces();

	static BOOL GetCurrentDirectory(CDirectory &rpath);

	BOOL IsFilePath() const;

	int CompareNoCaseFileName(CDirectory path) const;

	int CompareNoCaseFileTitle(CDirectory path) const;

	int CompareNoCaseFilePath(CDirectory path) const;

	int CompareNoCaseExtention(const CString &path) const;

	BOOL IsPathStrict() const;
	BOOL IsFileExist(void) const;

	void GetLinuxPathName(CString &rstr);
	void ReplaceLinuxPathtoWindowsPath();
	BOOL MakeDirectory();


};

inline
const CDirectory operator + (const CDirectory &lhs, const CDirectory &rhs)
{
	CDirectory path(lhs);
	return path += rhs;
}
