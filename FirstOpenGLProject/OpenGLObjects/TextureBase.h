#pragma once

class TextureBase 
{
public:
	TextureBase() {}
	virtual void bind() const = 0;
	virtual unsigned int getID() const = 0;
};