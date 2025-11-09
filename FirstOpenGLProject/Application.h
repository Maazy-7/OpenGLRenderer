#include <iostream>
#include <memory>

#include "Window.h"


class Application 
{

private:

	std::unique_ptr<Window> m_window;

public:

	void run();
	

};