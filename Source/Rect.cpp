/**************************************************************************************\
** File: Rect.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Rect class
**
\**************************************************************************************/
#include "pch.h"
#include "Rect.h"

namespace d2d
{
	Rect::Rect(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound)
	  :	m_lowerBound{ newLowerBound },
		m_upperBound{ newUpperBound }
	{
		SortBounds();
	}
	void Rect::SetBounds(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound)
	{
		m_lowerBound = newLowerBound;
		m_upperBound = newUpperBound;
		SortBounds();
	}
	void Rect::SetCenter(const b2Vec2& center, const b2Vec2& dimensions)
	{
		b2Vec2 halfDimensions = 0.5f * dimensions;
		m_lowerBound = center - halfDimensions;
		m_upperBound = center + halfDimensions;
		SortBounds();
	}
	const b2Vec2& Rect::GetCenter() const
	{
		return 0.5f * (m_lowerBound + m_upperBound);
	}
	const b2Vec2& Rect::GetDimensions() const
	{
		return (m_upperBound - m_lowerBound);
	}
	float Rect::GetWidth() const
	{
		return m_upperBound.x - m_lowerBound.x;
	}
	float Rect::GetHeight() const
	{
		return m_upperBound.y - m_lowerBound.y;
	}
	const b2Vec2& Rect::GetLowerBound() const
	{
		return m_lowerBound;
	}
	const b2Vec2& Rect::GetUpperBound() const
	{
		return m_upperBound;
	}
	float Rect::GetPerimeter() const
	{
		float width = m_upperBound.x - m_lowerBound.x;
		float height = m_upperBound.y - m_lowerBound.y;
		return 2.0f * (width + height);
	}
	bool Rect::CollidesWith(const Rect& otherRect) const
	{
		// If we can't find a line that separates the two, then we know they collide.
		return !( m_lowerBound.x > otherRect.m_upperBound.x ||
				  otherRect.m_lowerBound.x > m_upperBound.x ||
				  m_lowerBound.y > otherRect.m_upperBound.y ||
				  otherRect.m_lowerBound.y > m_upperBound.y );
	}
	bool Rect::Contains(const Rect& otherRect) const
	{
		return ( m_lowerBound.x <= otherRect.m_lowerBound.x &&
				 m_lowerBound.y <= otherRect.m_lowerBound.y &&
				 otherRect.m_upperBound.x <= m_upperBound.x &&
				 otherRect.m_upperBound.y <= m_upperBound.y );
	}
	bool Rect::Contains(const b2Vec2& point) const
	{
		return ( point.x >= m_lowerBound.x &&
			     point.x <= m_upperBound.x &&
			     point.y >= m_lowerBound.y &&
			     point.y <= m_upperBound.y );
	}
	void Rect::SortBounds()
	{
		if (m_lowerBound.x > m_upperBound.x)
		{
			float temp = m_lowerBound.x;
			m_lowerBound.x = m_upperBound.x;
			m_upperBound.x = temp;
		}
		if (m_lowerBound.y > m_upperBound.y)
		{
			float temp = m_lowerBound.y;
			m_lowerBound.y = m_upperBound.y;
			m_upperBound.y = temp;
		}
	}
}