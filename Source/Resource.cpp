/**************************************************************************************\
** File: Resource.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Resource class
**
\**************************************************************************************/
#include "pch.h"
#include "Resource.h"

namespace d2d
{
	void Resource::IncrementReferenceCount()
	{
		++m_referenceCount;
	}
	void Resource::DecrementReferenceCount()
	{
		--m_referenceCount;
	}
	unsigned long Resource::GetReferenceCount() const
	{
		return m_referenceCount;
	}
	bool Resource::SuccessfullyLoaded() const
	{
		return m_loadSuccess;
	}
	void Resource::SetLoadSuccess(bool loadSuccess)
	{
		m_loadSuccess = loadSuccess;
	}
}
