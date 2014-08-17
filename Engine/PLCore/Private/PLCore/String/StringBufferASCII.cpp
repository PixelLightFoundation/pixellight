/*********************************************************\
 *  File: StringBufferASCII.cpp                          *
 *
 *  Copyright (C) 2002-2013 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 *  and associated documentation files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or
 *  substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/


//[-------------------------------------------------------]
//[ Includes                                              ]
//[-------------------------------------------------------]
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#ifdef LINUX
	#include <wchar.h>
	#include "PLCore/+Linux/PLCoreLinuxWrapper.h"
#endif
#include "PLCore/String/String.h"
#include "PLCore/String/UTF8Tools.h"
#include "PLCore/String/StringBufferUnicode.h"
#include "PLCore/String/StringBufferUTF8.h"
#include "PLCore/String/StringBufferASCII.h"


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ Private functions                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Constructor
*/
StringBufferASCII::StringBufferASCII(char szString[], uint32 nLength, uint32 nMaxLength) : StringBuffer(nLength, nMaxLength, String::ASCII),
	m_pszString(szString),
	m_pUnicode(nullptr),
	m_pUTF8(nullptr)
{
}

/**
*  @brief
*    Constructor
*/
StringBufferASCII::StringBufferASCII(const wchar_t szString[], uint32 nLength, uint32 nMaxLength) : StringBuffer(nLength, nMaxLength, String::ASCII),
	m_pszString(new char[nLength + 1]),
	m_pUnicode(nullptr),
	m_pUTF8(nullptr)
{
	// We need to convert the given Unicode string into an ASCII one
	wcstombs(m_pszString, szString, nLength + 1);
}

/**
*  @brief
*    Destructor
*/
StringBufferASCII::~StringBufferASCII()
{
	if (m_pUnicode)
		Manager.ReleaseStringBuffer(*m_pUnicode);
	if (m_pUTF8)
		Manager.ReleaseStringBuffer(*m_pUTF8);
	delete [] m_pszString; // (there MUST by such a string :)
}

/**
*  @brief
*    Sets the string
*/
void StringBufferASCII::SetString(char szString[], uint32 nLength)
{
	// The Unicode version is now dirty
	if (m_pUnicode) {
		Manager.ReleaseStringBuffer(*m_pUnicode);
		m_pUnicode = nullptr;
	}

	// The UTF8 version is now dirty
	if (m_pUTF8) {
		Manager.ReleaseStringBuffer(*m_pUTF8);
		m_pUTF8 = nullptr;
	}

	// Kill the old string (there MUST by such a string :)
	delete [] m_pszString;

	// Set new string
	m_pszString  = szString;
	m_nLength    = nLength;
	m_nMaxLength = nLength;
}

/**
*  @brief
*    Sets the new string length
*/
void StringBufferASCII::SetNewStringLength(uint32 nLength)
{
	// The Unicode version is now dirty
	if (m_pUnicode) {
		Manager.ReleaseStringBuffer(*m_pUnicode);
		m_pUnicode = nullptr;
	}

	// The UTF8 version is now dirty
	if (m_pUTF8) {
		Manager.ReleaseStringBuffer(*m_pUTF8);
		m_pUTF8 = nullptr;
	}

	// Set new string length
	m_nLength = nLength;
}

/**
*  @brief
*    Sets a character
*/
void StringBufferASCII::SetCharacter(char nCharacter)
{
	// Optimization: If we have our own unique string buffer instance in the required format we 'just'
	// cut it down to use a single character... this is faster then allocation/deallocation and we ignore
	// the potential 'waste' if memory if the old string buffer was 'larger'

	// We know there MUST be at least one character, so the following is 'safe'
	m_pszString[0] = nCharacter;
	m_pszString[1] = '\0';

	// Just ignore the 'old stuff' behind the first character (=> NO memory leak!)
	SetNewStringLength(1);
}


//[-------------------------------------------------------]
//[ Public virtual StringBuffer functions                 ]
//[-------------------------------------------------------]
uint32 StringBufferASCII::GetFormat() const
{
	return String::ASCII;
}

StringBufferASCII *StringBufferASCII::GetASCII()
{
	return this;
}

StringBufferUnicode *StringBufferASCII::GetUnicode()
{
	// Is there already a current Unicode version?
	if (!m_pUnicode) {
		// Nope, let's create one

		// Request an unicode string buffer from the string buffer manager
		m_pUnicode = Manager.GetStringBufferUnicode(m_nLength);
		if (m_pUnicode) {
			m_pUnicode->AddReference();
			m_pUnicode->Append(m_pszString, m_nLength);
		}
	}

	// Return it
	return m_pUnicode;
}

StringBufferUTF8 *StringBufferASCII::GetUTF8()
{
	// Is there already a current UTF8 version?
	if (!m_pUTF8) {
		// Nope, let's create one
		m_pUTF8 = new StringBufferUTF8(m_pszString, m_nLength);
		m_pUTF8->AddReference();
	}

	// Return it
	return m_pUTF8;
}

uint32 StringBufferASCII::GetNumOfBytes() const
{
	// For ASCII: number of bytes = length :)
	return m_nLength;
}

StringBuffer *StringBufferASCII::Clone() const
{
	// Request an ASCII string buffer from the string buffer manager
	StringBuffer *pStringBuffer = Manager.GetStringBufferASCII(m_nLength);
	if (pStringBuffer)
		pStringBuffer->Append(m_pszString, m_nLength);

	// Done
	return pStringBuffer;
}

StringBuffer *StringBufferASCII::Duplicate()
{
	// Do we need to clone this string buffer?
	if (GetRefCount() > 1) {
		return Clone();
	} else {
		// The Unicode version is now dirty
		if (m_pUnicode) {
			Manager.ReleaseStringBuffer(*m_pUnicode);
			m_pUnicode = nullptr;
		}

		// The UTF8 version is now dirty
		if (m_pUTF8) {
			Manager.ReleaseStringBuffer(*m_pUTF8);
			m_pUTF8 = nullptr;
		}

		// Just return this string buffer
		return this;
	}
}

bool StringBufferASCII::IsLessThan(const char szString[], uint32 nLength) const
{
	// "nLength" is not required in this implementation
	return (strcmp(m_pszString, szString) < 0);
}

bool StringBufferASCII::IsLessThan(const wchar_t szString[], uint32 nLength) const
{
	// "nLength" is not required in this implementation

	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);
	const bool bResult = (wcscmp(pUnicode, szString) < 0);
	delete [] pUnicode;
	return bResult;
}

bool StringBufferASCII::IsGreaterThan(const char szString[], uint32 nLength) const
{
	// "nLength" is not required in this implementation
	return (strcmp(m_pszString, szString) > 0);
}

bool StringBufferASCII::IsGreaterThan(const wchar_t szString[], uint32 nLength) const
{
	// "nLength" is not required in this implementation

	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);
	const bool bResult = (wcscmp(pUnicode, szString) > 0);
	delete [] pUnicode;
	return bResult;
}

bool StringBufferASCII::Compare(const char szString[], uint32 nLength, uint32 nPos, uint32 nCount) const
{
	if (!nPos && !nCount) {
		// Compare whole strings
		if (nLength != m_nLength)
			return false; // THAT was pretty easy, the length of the strings is not equal :)
		else
			return !strcmp(m_pszString, szString);

	} else {
		// Check start position
		if (nPos >= m_nLength)
			return false; // Strings are not equal

		// Compare strings (count can be > maximum string length)
		if (nCount)
			return !strncmp(&m_pszString[nPos], szString, nCount);
		else
			return !strcmp (&m_pszString[nPos], szString);
	}
}

bool StringBufferASCII::Compare(const wchar_t szString[], uint32 nLength, uint32 nPos, uint32 nCount) const
{
	if (!nPos && !nCount) {
		// Compare whole strings
		if (nLength != m_nLength) {
			// THAT was pretty easy, the length of the strings is not equal :)
			return false;
		} else {
			// We need to convert this ASCII string into an Unicode one :(
			wchar_t *pUnicode = new wchar_t[m_nLength + 1];
			mbstowcs(pUnicode, m_pszString, m_nLength + 1);
			const bool bResult = !wcscmp(pUnicode, szString);
			delete [] pUnicode;
			return bResult;
		}
	} else {
		// Check start position
		if (nPos >= m_nLength)
			return false; // Strings are not equal

		// We need to convert this ASCII string into an Unicode one :(
		wchar_t *pUnicode = new wchar_t[m_nLength + 1];
		mbstowcs(pUnicode, m_pszString, m_nLength + 1);

		// Compare strings (count can be > maximum string length)
		bool bResult;
		if (nCount)
			bResult = !wcsncmp(&pUnicode[nPos], szString, nCount);
		else
			bResult = !wcscmp (&pUnicode[nPos], szString);
		delete [] pUnicode;
		return bResult;
	}
}

bool StringBufferASCII::CompareNoCase(const char szString[], uint32 nLength, uint32 nPos, uint32 nCount) const
{
	if (!nPos && !nCount) {
		// Compare whole strings
		if (nLength != m_nLength)
			return false; // THAT was pretty easy, the length of the strings is not equal :)
		else
			return !_stricmp(m_pszString, szString);
	} else {
		// Check start position
		if (nPos >= m_nLength)
			return false; // Strings are not equal

		// Compare strings (count can be > maximum string length)
		if (nCount)
			return !_strnicmp(&m_pszString[nPos], szString, nCount);
		else
			return !_stricmp (&m_pszString[nPos], szString);
	}
}

bool StringBufferASCII::CompareNoCase(const wchar_t szString[], uint32 nLength, uint32 nPos, uint32 nCount) const
{
	if (!nPos && !nCount) {
		// Compare whole strings
		if (nLength != m_nLength) {
			// THAT was pretty easy, the length of the strings is not equal :)
			return false;
		} else {
			// We need to convert this ASCII string into an Unicode one :(
			wchar_t *pUnicode = new wchar_t[m_nLength + 1];
			mbstowcs(pUnicode, m_pszString, m_nLength + 1);
			const bool bResult = !_wcsicmp(pUnicode, szString);
			delete [] pUnicode;
			return bResult;
		}

	} else {
		// Check start position
		if (nPos >= m_nLength)
			return false; // Strings are not equal

		// We need to convert this ASCII string into an Unicode one :(
		wchar_t *pUnicode = new wchar_t[m_nLength + 1];
		mbstowcs(pUnicode, m_pszString, m_nLength + 1);

		// Compare strings (count can be > maximum string length)
		bool bResult;
		if (nCount)
			bResult = !_wcsnicmp(&pUnicode[nPos], szString, nCount);
		else
			bResult = !_wcsicmp (&pUnicode[nPos], szString);
		delete [] pUnicode;
		return bResult;
	}
}

bool StringBufferASCII::IsAlphabetic() const
{
	const char *pszString    = m_pszString;
	const char *pszStringEnd = pszString + m_nLength;
	for (; pszString<pszStringEnd; pszString++) {
		if (!isalpha(static_cast<unsigned char>(*pszString)))
			return false; // The string is not alphabetic
	}

	// The string is alphabetic
	return true;
}

bool StringBufferASCII::IsAlphaNumeric() const
{
	const char *pszString    = m_pszString;
	const char *pszStringEnd = pszString + m_nLength;
	for (; pszString<pszStringEnd; pszString++) {
		if (!isalpha(static_cast<unsigned char>(*pszString)) && !isdigit(static_cast<unsigned char>(*pszString)))
			return false; // The string is not alpha-numeric
	}

	// The string is alpha-numeric
	return true;
}

bool StringBufferASCII::IsNumeric() const
{
	const char *pszString    = m_pszString;
	const char *pszStringEnd = pszString + m_nLength;
	for (; pszString<pszStringEnd; pszString++) {
		if (!isdigit(static_cast<unsigned char>(*pszString)))
			return false; // The string is not numeric
	}

	// The string is numeric
	return true;
}

bool StringBufferASCII::IsSubstring(const char szString[], uint32 nLength) const
{
	return (strstr(m_pszString, szString) != nullptr);
}

bool StringBufferASCII::IsSubstring(const wchar_t szString[], uint32 nLength) const
{
	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);
	const bool bResult = wcsstr(pUnicode, szString) != nullptr;
	delete [] pUnicode;
	return bResult;
}

int StringBufferASCII::IndexOf(const char szString[], uint32 nPos, uint32 nLength) const
{
	const char *pszPos = strstr(&m_pszString[nPos], szString);
	return pszPos ? static_cast<int>(pszPos - m_pszString) : -1;
}

int StringBufferASCII::IndexOf(const wchar_t szString[], uint32 nPos, uint32 nLength) const
{
	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);
	const wchar_t *pszPos = wcsstr(&pUnicode[nPos], szString);
	const int nResult = pszPos ? static_cast<int>(pszPos - pUnicode) : -1;
	delete [] pUnicode;
	return nResult;
}

int StringBufferASCII::LastIndexOf(const char szString[], int nPos, uint32 nLength) const
{
	// Find substring
	const char *pszStringStart = m_pszString;
	const char *pszStringT     = pszStringStart + nPos;
	for (; pszStringT>=pszStringStart; pszStringT--) {
		if (!strncmp(pszStringT, szString, nLength))
			return static_cast<int>(pszStringT - pszStringStart); // We have found something
	}

	// No substring
	return -1;
}

int StringBufferASCII::LastIndexOf(const wchar_t szString[], int nPos, uint32 nLength) const
{
	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);

	// Find substring
	const wchar_t *pszStringStart = pUnicode;
	const wchar_t *pszStringT     = pszStringStart + nPos;
	for (; pszStringT>=pszStringStart; pszStringT--) {
		if (!wcsncmp(pszStringT, szString, nLength)) {
			const int nResult = static_cast<int>(pszStringT - pszStringStart);
			delete [] pUnicode;
			return nResult; // We have found something
		}
	}
	delete [] pUnicode;

	// No substring
	return -1;
}

StringBuffer *StringBufferASCII::GetSubstring(uint32 nPos, uint32 nCount) const
{
	// Request an ASCII string buffer from the string buffer manager
	StringBuffer *pStringBuffer = Manager.GetStringBufferASCII(nCount);
	if (pStringBuffer)
		pStringBuffer->Append(m_pszString + nPos, nCount);

	// Done
	return pStringBuffer;
}

StringBuffer *StringBufferASCII::ToLower()
{
	// Look for the first character that is NOT lower case
	const char *pszString    = m_pszString;
	const char *pszStringEnd = pszString + m_nLength;
	for (; pszString<pszStringEnd; pszString++) {
		// Is this already a lower character?
		if (!islower(static_cast<unsigned char>(*pszString))) {
			// Nope, now we have to clone the string buffer :(
			StringBufferASCII *pStringBufferASCIIClone = static_cast<StringBufferASCII*>(Duplicate());

			// Start with an offset - we KNOW the characters before will not be changed!
			_strlwr(pStringBufferASCIIClone->m_pszString + (pszString - m_pszString));

			// Return the new, lower case string buffer
			return pStringBufferASCIIClone;
		}
	}

	// If we are here, nothing was changed and we can just return this string buffer
	return this;
}

StringBuffer *StringBufferASCII::ToUpper()
{
	// Look for the first character that is NOT upper case
	const char *pszString    = m_pszString;
	const char *pszStringEnd = pszString + m_nLength;
	for (; pszString<pszStringEnd; pszString++) {
		// Is this already a upper character?
		if (!isupper(static_cast<unsigned char>(*pszString))) {
			// Nope, now we have to clone the string buffer :(
			StringBufferASCII *pStringBufferASCIIClone = static_cast<StringBufferASCII*>(Duplicate());

			// Start with an offset - we KNOW the characters before will not be changed!
			_strupr(pStringBufferASCIIClone->m_pszString + (pszString - m_pszString));

			// Return the new, upper case string buffer
			return pStringBufferASCIIClone;
		}
	}

	// If we are here, nothing was changed and we can just return this string buffer
	return this;
}

StringBuffer *StringBufferASCII::Delete(uint32 nPos, uint32 nCount)
{
	// We have to clone the string buffer
	StringBufferASCII *pStringBufferASCIIClone = static_cast<StringBufferASCII*>(Duplicate());

	// Characters are deleted by moving up the data following the region to delete (FAST!)
	const uint32 nNewLength = m_nLength - nCount;
	memcpy(&pStringBufferASCIIClone->m_pszString[nPos], pStringBufferASCIIClone->m_pszString + nPos + nCount, nNewLength - nPos);
	pStringBufferASCIIClone->m_pszString[nNewLength] = '\0';

	// Set the new length
	pStringBufferASCIIClone->SetNewStringLength(nNewLength);

	// Done
	return pStringBufferASCIIClone;
}

StringBuffer *StringBufferASCII::Append(const char szString[], uint32 nCount)
{
	// Calculate the new total length of the string (excluding the terminating zero)
	const uint32 nNewLength = m_nLength + nCount;

	// Is it possible to just modify the current internal string in place? (FAST!)
	if (nNewLength <= m_nMaxLength && GetRefCount() < 2) {
		// Just modify the current internal string in place
		memcpy(&m_pszString[m_nLength], szString, nCount);	// Append the new string
		m_pszString[nNewLength] = '\0';						// Set the terminating zero
		SetNewStringLength(nNewLength);						// Set the new string length

		// Return this string buffer
		return this;
	} else {
		// Request an ASCII string buffer from the string buffer manager
		StringBuffer *pStringBuffer = Manager.GetStringBufferASCII(nNewLength);
		if (pStringBuffer) {
			// Add old string
			if (m_nLength)
				pStringBuffer->Append(m_pszString, m_nLength);

			// Add string to append
			pStringBuffer->Append(szString, nCount);
		}

		// Done
		return pStringBuffer;
	}
}

StringBuffer *StringBufferASCII::Append(const wchar_t szString[], uint32 nCount)
{
	// Calculate the new total length of the string (excluding the terminating zero)
	const uint32 nNewLength = m_nLength + nCount;

	// Request an unicode string buffer from the string buffer manager
	StringBuffer *pStringBuffer = Manager.GetStringBufferUnicode(nNewLength);
	if (pStringBuffer) {
		// Add old string
		if (m_nLength)
			pStringBuffer->Append(m_pszString, m_nLength);

		// Add string to append
		pStringBuffer->Append(szString, nCount);
	}

	// Done
	return pStringBuffer;
}

StringBuffer *StringBufferASCII::Insert(const char szString[], uint32 nPos, uint32 nCount)
{
	// Calculate the new total length of the string (excluding the terminating zero)
	const uint32 nNewLength = m_nLength + nCount;

	// Is it possible to just modify the current internal string in place? (FAST!)
	if (nNewLength <= m_nMaxLength && GetRefCount() < 2) {
		// Just modify the current internal string in place
		const int nLeftCharacters = m_nLength - nPos;
		if (nLeftCharacters > 0) {
			// Make space for the new string by moving everything to the right
			// (in here, we KNOW that there's enough memory in the right to hold the string!)
			memcpy(&m_pszString[nPos + nCount], &m_pszString[nPos], nLeftCharacters);
		}
		memcpy(&m_pszString[nPos], szString, nCount);	// Append the new string at the now free space
		m_pszString[nNewLength] = '\0';					// Set the terminating zero
		SetNewStringLength(nNewLength);					// Set the new string length

		// Return this string buffer
		return this;
	} else {
		// Request an ASCII string buffer from the string buffer manager
		StringBuffer *pStringBuffer = Manager.GetStringBufferASCII(nNewLength);
		if (pStringBuffer) {
			// Add old string in front of the insert place
			if (m_nLength && nPos)
				pStringBuffer->Append(m_pszString, nPos);

			// Add new string
			pStringBuffer->Append(szString, nCount);

			// Add old string in back of the insert place
			const int nLeftCharacters = m_nLength - nPos;
			if (m_nLength && nLeftCharacters > 0)
				pStringBuffer->Append(&m_pszString[nPos], nLeftCharacters);
		}

		// Done
		return pStringBuffer;
	}
}

StringBuffer *StringBufferASCII::Insert(const wchar_t szString[], uint32 nPos, uint32 nCount)
{
	// Calculate the new total length of the string (excluding the terminating zero)
	const uint32 nNewLength = m_nLength + nCount;

	// Request an unicode string buffer from the string buffer manager
	StringBuffer *pStringBuffer = Manager.GetStringBufferUnicode(nNewLength);
	if (pStringBuffer) {
		// Add old string in front of the insert place
		if (m_nLength && nPos)
			pStringBuffer->Append(m_pszString, nPos);

		// Add new string
		pStringBuffer->Append(szString, nCount);

		// Add old string in back of the insert place
		const int nLeftCharacters = m_nLength - nPos;
		if (m_nLength && nLeftCharacters > 0)
			pStringBuffer->Append(&m_pszString[nPos], nLeftCharacters);
	}

	// Done
	return pStringBuffer;
}

StringBuffer *StringBufferASCII::Replace(char nOld, char nNew, uint32 &nReplaced)
{
	char *pszString    = m_pszString;
	char *pszStringEnd = m_pszString + m_nLength;

	// Search for the first character which should be replaced
	nReplaced = 0;
	for (; pszString<pszStringEnd; pszString++) {
		if (*pszString == nOld) {
			// Fork string buffer when the first character has been found
			StringBufferASCII *pStringBufferASCIIClone = static_cast<StringBufferASCII*>(Duplicate());

			// Set pointers to new location
			pszString    = pStringBufferASCIIClone->m_pszString + (pszString - m_pszString);
			pszStringEnd = pStringBufferASCIIClone->m_pszString + m_nLength;

			// Replace characters
			for (; pszString<pszStringEnd; pszString++) {
				if (*pszString == nOld) {
					*pszString = nNew;
					nReplaced++;
				}
			}

			// Return the new string buffer
			return pStringBufferASCIIClone;
		}
	}

	// If we are here, nothing was changed and we can just return this string buffer
	return this;
}

StringBuffer *StringBufferASCII::Replace(wchar_t nOld, wchar_t nNew, uint32 &nReplaced)
{
	char *pszString    = m_pszString;
	char *pszStringEnd = m_pszString + m_nLength;
	wchar_t nUnicode;

	// Search for the first character which should be replaced
	nReplaced = 0;
	for (; pszString<pszStringEnd; pszString++) {
		if (mbtowc(&nUnicode, pszString, 1) >= 0 && nUnicode == nOld) {
			// Request an Unicode string buffer from the string buffer manager when the first character has been found
			StringBufferUnicode *pStringBufferUnicodeClone = Manager.GetStringBufferUnicode(m_nLength);
			if (pStringBufferUnicodeClone) {
				// Copy the ASCII string into the new unicode string
				pStringBufferUnicodeClone->Append(m_pszString, m_nLength);

				// Set pointers to new location
				wchar_t *pszStringUnicode    = pStringBufferUnicodeClone->m_pszString + (pszString - m_pszString);
				wchar_t *pszStringUnicodeEnd = pStringBufferUnicodeClone->m_pszString + m_nLength;

				// Replace characters
				for (; pszStringUnicode<pszStringUnicodeEnd; pszStringUnicode++) {
					if (*pszStringUnicode == nOld) {
						*pszStringUnicode = nNew;
						nReplaced++;
					}
				}

				// Return the new string buffer
				return pStringBufferUnicodeClone;
			}
		}
	}

	// If we are here, nothing was changed and we can just return this string buffer
	return this;
}

StringBuffer *StringBufferASCII::Replace(const char szOld[], uint32 nOldLength, const char szNew[], uint32 nNewLength, uint32 &nReplaced)
{
	// [TODO] Use the string buffer manager

	// Check for only character replace
	if (nOldLength == 1 && nNewLength == 1)
		return Replace(szOld[0], szNew[0], nReplaced);

	// First, get the number of substrings to replace
	nReplaced = 0;
	const char *pszString = m_pszString;
	const char *pszEnd    = m_pszString + m_nLength;
	while (pszString<pszEnd) {
		if (!strncmp(pszString, szOld, nOldLength)) {
			nReplaced++;
			pszString += nOldLength;
		} else {
			pszString++;
		}
	}

	// Jipi, no substrings, no further work to do :)
	if (!nReplaced)
		return this; // Nothing was changed and we can just return this string buffer

	// Get and check the new length
	const uint32 nFinalLength = m_nLength + (nNewLength - nOldLength)*nReplaced;
	if (!nFinalLength)
		return nullptr; // The string is now empty!

	// Create the new string
	char *pszNewString  = new char[nFinalLength + 1];
	char *pszNewStringT = pszNewString;
	uint32 nSkipped = 0;
	pszString = m_pszString;
	for (uint32 i=0; i<nReplaced; i++) {
		// Find substring
		while (strncmp(pszString, szOld, nOldLength)) {
			pszString++;
			nSkipped++;
		}

		// Copy previous none substring characters
		if (nSkipped) {
			memcpy(pszNewStringT, pszString - nSkipped, nSkipped);
			pszNewStringT += nSkipped;
		}

		// Insert new substring
		memcpy(pszNewStringT, szNew, nNewLength);
		pszNewStringT += nNewLength;
		pszString += nOldLength;
		nSkipped = 0;
	}

	// Copy the rest of the old string to the new string
	if (*pszString != '\0') {
		uint32 i = static_cast<uint32>(pszNewString + nFinalLength - pszNewStringT);
		memcpy(pszNewStringT, pszString, i);
		pszNewStringT += i;
	}

	// Add terminating zero
	*pszNewStringT = '\0';

	// Return the new string buffer
	if (GetRefCount() > 1) {
		return new StringBufferASCII(pszNewString, nFinalLength, nFinalLength);
	} else {
		// Just modify and return this string buffer
		SetString(pszNewString, nFinalLength);
		return this;
	}
}

StringBuffer *StringBufferASCII::Replace(const wchar_t szOld[], uint32 nOldLength, const wchar_t szNew[], uint32 nNewLength, uint32 &nReplaced)
{
	// [TODO] Use the string buffer manager

	// Check for only character replace
	if (nOldLength == 1 && nNewLength == 1)
		return Replace(szOld[0], szNew[0], nReplaced);

	// We need to convert this ASCII string into an Unicode one :(
	wchar_t *pUnicode = new wchar_t[m_nLength + 1];
	mbstowcs(pUnicode, m_pszString, m_nLength + 1);

	// First, get the number of substrings to replace
	nReplaced = 0;
	const wchar_t *pszString = pUnicode;
	const wchar_t *pszEnd    = pUnicode + m_nLength;
	while (pszString<pszEnd) {
		if (!wcsncmp(pszString, szOld, nOldLength)) {
			nReplaced++;
			pszString += nOldLength;
		} else {
			pszString++;
		}
	}

	// Jipi, no substrings, no further work to do :)
	if (!nReplaced) {
		delete [] pUnicode;
		return this; // Nothing was changed and we can just return this string buffer
	}

	// Get and check the new length
	const uint32 nFinalLength = m_nLength + (nNewLength - nOldLength)*nReplaced;
	if (!nFinalLength) {
		delete [] pUnicode;
		return nullptr; // The string is now empty!
	}

	// Create the new string
	wchar_t *pszNewString  = new wchar_t[nFinalLength + 1];
	wchar_t *pszNewStringT = pszNewString;
	uint32 nSkipped = 0;
	pszString = pUnicode;
	for (uint32 i=0; i<nReplaced; i++) {
		// Find substring
		while (wcsncmp(pszString, szOld, nOldLength)) {
			pszString++;
			nSkipped++;
		}

		// Copy previous none substring characters
		if (nSkipped) {
			memcpy(pszNewStringT, pszString - nSkipped, nSkipped*sizeof(wchar_t));
			pszNewStringT += nSkipped;
		}

		// Insert new substring
		memcpy(pszNewStringT, szNew, nNewLength*sizeof(wchar_t));
		pszNewStringT += nNewLength;
		pszString += nOldLength;
		nSkipped = 0;
	}

	// Copy the rest of the old string to the new string
	if (*pszString != L'\0') {
		const uint32 i = static_cast<uint32>(pszNewString + nFinalLength - pszNewStringT);
		memcpy(pszNewStringT, pszString, i*sizeof(wchar_t));
		pszNewStringT += i;
	}

	// Add terminating zero
	*pszNewStringT = L'\0';

	// Cleanup
	delete [] pUnicode;

	// Return the new Unicode string buffer
	return new StringBufferUnicode(pszNewString, nFinalLength, nFinalLength);
}

StringBuffer *StringBufferASCII::SetCharacter(uint32 nIndex, char nCharacter)
{
	// Do we need to clone this string buffer?
	if (m_pszString[nIndex] == nCharacter) {
		// Nothing was changed
		return this;
	} else {
		// We have to clone the string buffer :(
		StringBufferASCII *pStringBufferASCIIClone = static_cast<StringBufferASCII*>(Duplicate());

		// Set the new character
		pStringBufferASCIIClone->m_pszString[nIndex] = nCharacter;

		// Return the new string buffer
		return pStringBufferASCIIClone;
	}
}

StringBuffer *StringBufferASCII::SetCharacter(uint32 nIndex, wchar_t nCharacter)
{
	// Get this character as Unicode and check whether or not we need to clone this string buffer
	wchar_t nThisCharacterUnicode;
	if (mbtowc(&nThisCharacterUnicode, &m_pszString[nIndex], 1) >= 0 && nThisCharacterUnicode != nCharacter) {
		// Request an Unicode string buffer from the string buffer manager when the first character has been found
		StringBufferUnicode *pStringBufferUnicodeClone = Manager.GetStringBufferUnicode(m_nLength);
		if (pStringBufferUnicodeClone) {
			// Copy the ASCII string into the new unicode string
			pStringBufferUnicodeClone->Append(m_pszString, m_nLength);

			// Set the new character
			pStringBufferUnicodeClone->m_pszString[nIndex] = nCharacter;

			// Return the new string buffer
			return pStringBufferUnicodeClone;
		}
	}

	// Nothing was changed
	return this;
}

StringBuffer *StringBufferASCII::TrimLeading()
{
	const char *pszString = m_pszString;
	while (*pszString == ' ' || *pszString == '\t')
		pszString++;
	if (pszString == m_pszString) {
		// Nothing to change
		return this;
	} else {
		// Get the number of characters to delete
		const uint32 nCount = static_cast<uint32>(pszString - m_pszString);

		// Is the string now empty?
		if (nCount == m_nLength)
			return nullptr;				// The string is now empty
		else
			return Delete(0, nCount);	// Return the new string buffer
	}
}

StringBuffer *StringBufferASCII::TrimTrailing()
{
	const char *pszEnd    = m_pszString + m_nLength - 1;
	const char *pszString = pszEnd;
	while (*pszString == ' ' || *pszString == '\t')
		pszString--;
	if (pszString == pszEnd) {
		// Nothing to change
		return this;
	} else {
		// Get the number of characters to delete
		const uint32 nCount = static_cast<uint32>(pszEnd - pszString);

		// Is the string now empty?
		if (nCount == m_nLength)
			return nullptr;																// The string is now empty
		else
			return Delete(static_cast<uint32>(pszString - m_pszString) + 1, nCount);	// Return the new string buffer
	}
}

StringBuffer *StringBufferASCII::RemoveLineEndings()
{
	const char *pszEnd    = m_pszString + m_nLength - 1;
	const char *pszString = pszEnd;
	while (*pszString == '\r' || *pszString == '\n')
		pszString--;
	if (pszString == pszEnd) {
		// Nothing to change
		return this;
	} else {
		// Get the number of characters to delete
		const uint32 nCount = static_cast<uint32>(pszEnd - pszString);

		// Is the string now empty?
		if (nCount == m_nLength)
			return nullptr;																// The string is now empty
		else
			return Delete(static_cast<uint32>(pszString - m_pszString) + 1, nCount);	// Return the new string buffer
	}
}


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore
