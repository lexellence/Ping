/**************************************************************************************\
** File: Resource.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Resource class
**
\**************************************************************************************/
#ifndef RESOURCE_H
#define RESOURCE_H
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
#endif //RESOURCE_H