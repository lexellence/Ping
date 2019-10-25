/**************************************************************************************\
** File: Rect.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Rectangle class
**
\**************************************************************************************/
#ifndef RECT_H
#define RECT_H

namespace d2d
{
	class Rect
	{
	public:
		Rect() = default;
		Rect(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound);
		void SetBounds(const b2Vec2& newLowerBound, const b2Vec2& newUpperBound);
		void SetCenter(const b2Vec2& center, const b2Vec2& dimensions);
		const b2Vec2& GetCenter() const;
		const b2Vec2& GetDimensions() const;
		float GetWidth() const;
		float GetHeight() const;
		const b2Vec2& GetLowerBound() const;
		const b2Vec2& GetUpperBound() const;
		float GetPerimeter() const;
		bool CollidesWith(const Rect& otherRect) const;
		bool Contains(const Rect& otherRect) const;
		bool Contains(const b2Vec2& point) const;

	private:
		b2Vec2 m_lowerBound{ 0.0f, 0.0f };
		b2Vec2 m_upperBound{ 0.0f, 0.0f };

		void SortBounds();
	};
}

#endif //RECT_H