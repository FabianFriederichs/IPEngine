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
	if (!init)
	{
		init = true;
		ipengine::Scheduler& sched = m_core->getScheduler();
		auto time = ipengine::Time(1.f, 1);
		handle.push_back(sched.subscribe(ipengine::TaskFunction::make_func<ExtensionTest, &ExtensionTest::changeCamera>(this), time.nano(), ipengine::Scheduler::SubType::Interval, 1, &m_core->getThreadPool(),true));
		auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
		args[0].cast<IGraphics_API*>()->setCameraEntity(scm->getEntityByName("Camera")->m_entityId);
	}
	//for (auto name : argnames)
	//{
	//	if (name == "this")
	//	{
	//		//Argument is calling module instance
	//		if (args.size() > i)
	//		{
	//			//(args[i].cast<GraphicsModule*>())
	//		}
	//	}
	//	if (name == "test")
	//	{
	//		if (args.size() > i)
	//		{
	//			auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	//			auto mmeme = args[i].cast<std::string*>();
	//			args[i].cast<std::string*>()->append("Success!!");
	//		}
	//	}
	//	++i;
	//}

}

ExtensionInformation * ExtensionTest::getInfo()
{
	return &m_info;
}

void ExtensionTest::changeCamera(ipengine::TaskContext &c)
{
	auto scm = m_info.dependencies.getDep<SCM::ISimpleContentModule_API>("SCM");
	std::cout << -scm->getEntityByName("Camera")->m_transformData.setData()->m_location.z;
	scm->getEntityByName("Camera")->m_transformData.setData()->m_location = scm->getEntityByName("Camera")->m_transformData.getData()->m_location + scm->getEntityByName("Camera")->m_transformData.getData()->m_localZ * 1.0f;
	scm->getEntityByName("Camera")->m_transformData.setData()->m_isMatrixDirty = true;
	
	/*
	Rotation:
	yawquat <- calculate yaw rotation quat from mouse delta stuff
	pitchquat <- calculate pitch rotation quat from mouse delta stuff	
	newRotation = normalize(yawquat * currentRotation * pitchquat)	
	*/
}
