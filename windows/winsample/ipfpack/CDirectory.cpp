/*
 *  CDirectory
 *
 *  Copyright (C) 2016 analysisjp
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "stdafx.h"
#include "CDirectory.h"
#include <tchar.h>

#include <shlwapi.h>
#include <imagehlp.h>
#pragma comment(lib, "imagehlp.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define THIS_STR ((CString &)*this)

// m_strPath.GetLength() + siz + 1 文字分のバッファをpcharに確保するマクロ
// 失敗時はfailValを戻り値としてリターンします。
#define TOPCHAR(siz, failVal)													\
	const size_t pcharSize = (GetLength() + siz + 1) * sizeof(TCHAR);			\
	const LPTSTR pchar = (LPTSTR)_alloca(pcharSize);							\
	if (pchar == NULL) { return failVal; }										\
	_tcscpy_s(pchar, pcharSize, THIS_STR);

// m_strPathをpcharの値で置き換え、retValをリターンするマクロ
#define TOSTR(retVal)															\
	ASSERT(pchar != NULL);														\
	THIS_STR = pchar;															\
	return retVal;

void CDirectory::AddBackslash()
{
	TOPCHAR(1, void());
	PathAddBackslash(pchar);
	TOSTR(void());
}

CDirectory &CDirectory::operator += (const CDirectory &rhs)
{
	CString &rstr = *this;

	rstr += rhs;

	return *this;
}


BOOL CDirectory::AddExtension(LPCTSTR szExt)
{
	TOPCHAR(lstrlen(szExt), FALSE);
	if (!PathAddExtension(pchar, szExt)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

BOOL CDirectory::Append(LPCTSTR szMore)
{
	if(IsRelative()){
		AddBackslash();
	
		TOPCHAR(lstrlen(szMore) + 1, FALSE);
		strcat(pchar, szMore);
	
		TOSTR(TRUE);
	}else{	
		TOPCHAR(lstrlen(szMore) + 1, FALSE);

		if (!PathAppend(pchar, szMore)) {
			return FALSE;
		}
		TOSTR(TRUE);
	}
}

void CDirectory::BuildRoot(int iDrive)
{
	TOPCHAR(4, void());
	PathBuildRoot(pchar, iDrive);
	TOSTR(void());
}

void CDirectory::Canonicalize()
{
	TOPCHAR(0, void());
	const size_t pchSrcSize = (lstrlen(pchar) + 1) * sizeof(TCHAR);
	const LPTSTR pchSrc = (LPTSTR)_alloca(pchSrcSize);
	if (pchSrc == NULL) {
		return;
	}
	_tcscpy_s(pchSrc, pchSrcSize, pchar);
	if (!PathCanonicalize(pchar, pchSrc)) {
		return;
	}
	TOSTR(void());
}

void CDirectory::Combine(LPCTSTR pszDir, LPCTSTR pszFile)
{
	Empty();
	TOPCHAR(lstrlen(pszDir) + lstrlen(pszFile) + 2, void());
	PathCombine(pchar, pszDir, pszFile);
	TOSTR(void());
}

CDirectory CDirectory::CommonPrefix(LPCTSTR pszOther)
{
	TOPCHAR(0, CDirectory());
	if (PathCommonPrefix(THIS_STR, pszOther, pchar) == 0) {
		return CDirectory();
	}
	return CDirectory(pchar);
}

BOOL CDirectory::CompactPath(HDC hDC, UINT nWidth)
{
	TOPCHAR(0, FALSE);
	if (!PathCompactPath(hDC, pchar, nWidth)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

BOOL CDirectory::CompactPathEx(UINT nMaxChars, DWORD dwFlags/* = 0*/)
{
	TOPCHAR(0, FALSE);
	if (!PathCompactPathEx(pchar, THIS_STR, nMaxChars, dwFlags)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

BOOL CDirectory::FileExists() const
{
	return PathFileExists(THIS_STR);
}

int CDirectory::FindExtension() const
{
	const LPCTSTR pExt = PathFindExtension(THIS_STR);
	if (pExt == NULL) {
		return -1;
	}
	return int(pExt - (LPCTSTR)THIS_STR);
}

int CDirectory::FindFileName() const
{
	const LPCTSTR pFile = PathFindFileName(THIS_STR);
	if (pFile == NULL) {
		return -1;
	}
	return int(pFile - (LPCTSTR)THIS_STR);
}

int CDirectory::GetDriveNumber() const
{
	return PathGetDriveNumber(THIS_STR);
}

CString CDirectory::GetExtension(BOOL flag, BOOL lower) const
{
	CDirectory tmp(*this);
	CString work;

	tmp.UnquoteSpaces();

	const int iPos = tmp.FindExtension();
	if (iPos == -1) {
		return CString();
	}

	if(flag){
		work = CString((LPCTSTR)tmp + iPos);		// ドットあり
	}else{
		work = CString((LPCTSTR)tmp + iPos+1);		// ドットなし
	}

	if(lower){
		work.MakeLower();
	}
	return work;
}

CString CDirectory::GetFileName() const
{
	CDirectory tmp(*this);
	tmp.UnquoteSpaces();

	const int iPos = tmp.FindFileName();
	const int iEos = tmp.FindExtension();

	if ( (iPos == -1) ||
		 (iEos == -1) )
	{
		return CString();
	}
	return CString((LPCTSTR)tmp +iPos, iEos-iPos);
}

CString CDirectory::GetFileNameWithExtension() const
{
	CDirectory tmp(*this);
	tmp.UnquoteSpaces();

	const int iPos = tmp.FindFileName();
	if (iPos == -1) {
		return CString();
	}
	return CString((LPCTSTR)tmp + iPos);
}

CString CDirectory::GetFilePath() const
{
	CDirectory tmp(*this);
	tmp.UnquoteSpaces();

	const int iPos = tmp.FindFileName();

	if (iPos <= 0){
		return CString();
	}
	return CString((LPCTSTR)tmp, iPos-1);
}

BOOL CDirectory::IsDirectory() const
{
	return PathIsDirectory(THIS_STR);
}

BOOL CDirectory::IsFile() const
{
	WIN32_FIND_DATA FindData;

	HANDLE hFind = FindFirstFile( THIS_STR, &FindData);
	if(hFind == INVALID_HANDLE_VALUE)	return FALSE;

	FindClose(hFind);
	if(!(FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))	return TRUE;
	return FALSE;
}

BOOL CDirectory::IsFileSpec() const
{
	return PathIsFileSpec(THIS_STR);
}

BOOL CDirectory::IsPrefix(LPCTSTR pszPrefix) const
{
	return PathIsPrefix(THIS_STR, pszPrefix);
}

BOOL CDirectory::IsRelative() const
{
	return PathIsRelative(THIS_STR);
}

BOOL CDirectory::IsRoot() const
{
	return PathIsRoot(THIS_STR);
}

BOOL CDirectory::IsSameRoot(LPCTSTR pszOther) const
{
	return PathIsSameRoot(THIS_STR, pszOther);
}

BOOL CDirectory::IsUNC() const
{
	return PathIsUNC(THIS_STR);
}

BOOL CDirectory::IsUNCServer() const
{
	return PathIsUNCServer(THIS_STR);
}

BOOL CDirectory::IsUNCServerShare() const
{
	return PathIsUNCServerShare(THIS_STR);
}

BOOL CDirectory::MakePretty()
{
	TOPCHAR(0, FALSE);
	if (!PathMakePretty(pchar)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

BOOL CDirectory::MatchSpec(LPCTSTR pszSpec) const
{
	return PathMatchSpec(THIS_STR, pszSpec);
}

BOOL CDirectory::RelativePathTo(LPCTSTR pszFrom, DWORD dwAttrFrom, LPCTSTR pszTo, DWORD dwAttrTo)
{
	TOPCHAR(lstrlen(pszFrom) + lstrlen(pszTo) + 2, FALSE);
	if (!PathRelativePathTo(pchar, pszFrom, dwAttrFrom, pszTo, dwAttrTo)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

void CDirectory::QuoteSpaces()
{
	TOPCHAR(2, void());
	PathQuoteSpaces(pchar);
	TOSTR(void());
}

void CDirectory::RemoveArgs()
{
	TOPCHAR(0, void());
	PathRemoveArgs(pchar);
	TOSTR(void());
}

void CDirectory::RemoveBackslash()
{
	TOPCHAR(0, void());
	PathRemoveBackslash(pchar);
	TOSTR(void());
}

void CDirectory::RemoveBlanks()
{
	TOPCHAR(0, void());
	PathRemoveBlanks(pchar);
	TOSTR(void());
}

void CDirectory::RemoveExtension()
{
	TOPCHAR(0, void());
	PathRemoveExtension(pchar);
	TOSTR(void());
}

BOOL CDirectory::RemoveFileSpec()
{
	TOPCHAR(0, FALSE);
	if (!PathRemoveFileSpec(pchar)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

BOOL CDirectory::RenameExtension(LPCTSTR szExt)
{
	TOPCHAR(lstrlen(szExt), FALSE);
	if (!PathRenameExtension(pchar, szExt)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

int CDirectory::SkipRoot() const
{
	LPTSTR szPos = PathSkipRoot(THIS_STR);
	return int(szPos - (LPCTSTR)THIS_STR);
}

BOOL CDirectory::SplitPath(CString *pstrDrive, CString *pstrDir, CString *pstrFile, CString *pstrExt)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFile[_MAX_FNAME], szExt[_MAX_EXT];
	if (::_splitpath_s(THIS_STR, szDrive, szDir, szFile, szExt) != 0) {
		return FALSE;
	}

	if (pstrDrive != NULL) {
		*pstrDrive = szDrive;
	}

	if (pstrDir != NULL) {
		*pstrDir = szDir;
	}

	if (pstrFile != NULL) {
		*pstrFile = szFile;
	}

	if (pstrExt != NULL) {
		*pstrExt = szExt;
	}

	return TRUE;
}

BOOL CDirectory::SplitPath(CString *pstrDir, CString *pstrFile)
{
	if (pstrDir != NULL) {
		*pstrDir = GetFilePath();
	}

	if (pstrFile != NULL) {
		*pstrFile = GetFileNameWithExtension();
	}

	return TRUE;
}

void CDirectory::StripPath()
{
	TOPCHAR(0, void());
	PathStripPath(pchar);
	TOSTR(void());
}

BOOL CDirectory::StripToRoot()
{
	TOPCHAR(0, FALSE);
	if (!PathStripToRoot(pchar)) {
		return FALSE;
	}
	TOSTR(TRUE);
}

// パスを絶対パスに変換します。変換にはCurrentDirectoryを使用します。
BOOL CDirectory::ToFullPath()
{
	if (!IsRelative()) {
		return TRUE;
	}

	CDirectory pathDir;
	if (!GetCurrentDirectory(pathDir)) {
		return FALSE;
	}

	pathDir.Append(*this);
	*this = pathDir;

	return TRUE;
}

// パスを相対パスに変換します。変換にはCurrentDirectoryを使用します。
BOOL CDirectory::ToRelativePath()
{
	if (IsRelative()) {
		return TRUE;
	}

	CDirectory pathDir;
	if (!GetCurrentDirectory(pathDir)) {
		return FALSE;
	}

	DWORD dwThis = (IsDirectory() ? FILE_ATTRIBUTE_DIRECTORY : 0);	// RelativePathToは、ファイル属性についてディレクトリか否かのみに注目します。
	if (!RelativePathTo(*this, dwThis, pathDir, FILE_ATTRIBUTE_DIRECTORY)) {
		return FALSE;
	}

	return TRUE;
}

void CDirectory::UnquoteSpaces()
{
	TOPCHAR(0, void());
	PathUnquoteSpaces(pchar);
	TOSTR(void());
}

/*static*/
BOOL CDirectory::GetCurrentDirectory(CDirectory &rpath)
{
	LPTSTR szDir = (LPTSTR)_alloca((MAX_PATH) * sizeof(TCHAR));
	if (szDir == NULL) {
		return FALSE;
	}
	const DWORD dwRet = ::GetCurrentDirectory(MAX_PATH, szDir);
	if (dwRet == 0) {
		return FALSE;
	}
	else if (dwRet >= MAX_PATH) {
		// パスを格納するための領域が足りません。
		// 必要な領域の確保と、再度カレント･ディレクトリの取得を試みます。
		szDir = (LPTSTR)_alloca((dwRet + 1) * sizeof(TCHAR));
		if (szDir == NULL) {
			return FALSE;
		}
		VERIFY(::GetCurrentDirectory(dwRet + 1, szDir));
	}

	rpath = CDirectory(szDir);
	return TRUE;
}


BOOL CDirectory::IsFilePath() const
{
	CDirectory tmp(*this);
	tmp.UnquoteSpaces();

	const int iPos = tmp.FindFileName();

	if (iPos <= 0){
		return FALSE;
	}
	CString work((LPCTSTR)tmp, iPos-1);
	return work.IsEmpty() ? FALSE : TRUE;
}

int CDirectory::CompareNoCaseFileName(CDirectory path) const
{
	CString work = GetFileNameWithExtension();

	return work.CompareNoCase(path.GetFileNameWithExtension());
}

int CDirectory::CompareNoCaseFileTitle(CDirectory path) const
{
	CString work = GetFileName();

	return work.CompareNoCase(path.GetFileName());
}


int CDirectory::CompareNoCaseFilePath(CDirectory path) const
{
	return CompareNoCase(path);
}

// 拡張子を比較する
int CDirectory::CompareNoCaseExtention(const CString &ext) const
{
	CString ext1 = GetExtension(FALSE, FALSE);

	if(ext1 == ext){
		return 0;
	}
	return 1;

}

// 実在パスかどうかちゃんと調べる。FALSEはパスではないか存在しないもの。
BOOL CDirectory::IsPathStrict() const
{
	CDirectory tmp(*this);
	CFileStatus fstat;

	tmp.UnquoteSpaces();
	if(CFile::GetStatus(tmp,fstat) == FALSE){
		return FALSE;
	}
	if((fstat.m_attribute & CFile::directory) != 0){
		return TRUE;
	}
	return FALSE;
}

// 実在するファイルかどうかちゃんと調べる。FALSEはパスではないか存在しないもの。
BOOL CDirectory::IsFileExist(void) const
{
	CDirectory tmp(*this);
	CFileStatus fstat;

	tmp.UnquoteSpaces();
	return CFile::GetStatus(tmp,fstat);
}

// Linux形式の/で区切ったパスネームを返す
void CDirectory::GetLinuxPathName(CString &rstr)
{
#ifndef UNICODE
	// CString (ANSI) でやると、[hogehoge表]など0x5Cで終わっている場合は
	// 被って正しく変換できないので、その対策をUnicodeへの文字変換を使って行っている
	CStringW strw(*this);
#else
	CString strw(*this);
#endif
	strw.Replace(L"\\",L"/");
	rstr = strw;
	strw.Replace(L"\\",L"/");
	rstr = strw;
}

void CDirectory::ReplaceLinuxPathtoWindowsPath()
{
	Replace("/","\\");
}

BOOL CDirectory::MakeDirectory()
{
	if(IsDirectory())	return TRUE;
	if(IsFile())		return FALSE;

	CDirectory tmp(*this);
	tmp.AddBackslash();

	return MakeSureDirectoryPathExists(tmp);
}


#undef TOCHAR
#undef TOSTR

