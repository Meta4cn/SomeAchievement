#pragma once
#include "IShape.h"
class CShapeFactory
{
public:

	CShapeFactory(void)
	{
	}

	virtual ~CShapeFactory(void)
	{
	}

	virtual IShape* CreateShape() = 0;
};

