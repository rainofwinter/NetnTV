#pragma once

class ScriptProcessor;

class Color
{
public:


	
	static Color fromScriptObject(ScriptProcessor * s, JSObject * obj);
	JSObject * createScriptObject(ScriptProcessor * s) const;

	Color()
	{
		r = 0;
		g = 0;
		b = 0;
		a = 1;
	}
	Color(float r, float g, float b, float a) 
	{
		this->r = r; this->g = g; this->b = b; this->a = a;
	}
	

	bool operator == (const Color & rhs) const
	{
		return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
	}

	bool operator != (const Color & rhs) const
	{
		return !(*this == rhs);
	}

	bool operator < (const Color & rhs) const
	{
		if (r != rhs.r)
			return r < rhs.r;

		if (g != rhs.g)
			return g < rhs.g;

		if (b != rhs.b)
			return b < rhs.b;

		return a < rhs.a;

	}


public:
	float r, g, b, a;
};