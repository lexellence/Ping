/**************************************************************************************\
** File: Resource.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Resource class
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	class Resource
	{
	public:
		void IncrementReferenceCount();
		void DecrementReferenceCount();

		unsigned long GetReferenceCount() const;
		bool SuccessfullyLoaded() const;

	private:
		unsigned long m_referenceCount{ 1 };
		bool m_loadSuccess{ false };

	protected:
		void SetLoadSuccess(bool loadSuccess);
	};
}
