// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "ExtensionTest.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
ExtensionTest::ExtensionTest()
{
	//m_info.
}


void ExtensionTest::execute(std::vector<std::string> argnames, std::vector<ipengine::any>& args)
{
	int i = 0;
	for (auto name : argnames)
	{
		if (name == "this")
		{
			//Argument is calling module instance
			if (args.size() > i)
			{
				//(args[i].cast<GraphicsModule*>())
			}
		}
		if (name == "test")
		{
			if (args.size() > i)
			{
				auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
				auto mmeme = args[i].cast<std::string*>();
				args[i].cast<std::string*>()->append("Success!!");
			}
		}
		++i;
	}

}

ExtensionInformation * ExtensionTest::getInfo()
{
	return &m_info;
}
