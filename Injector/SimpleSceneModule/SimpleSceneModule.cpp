// Plugin2.cpp : Defines the exported functions for the DLL application.
//

#include "SimpleSceneModule.h"

// This is the constructor of a class that has been exported.
// see Plugin2.h for the class definition
SimpleSceneModule::SimpleSceneModule()
{
	m_info.identifier = "SimpleSceneModule";
	m_info.version = "1.0";
	m_info.iam = "ISimpleSceneModule_API";
	return;
}

SimpleSceneModule::SceneId SimpleSceneModule::LoadSceneFromFile(std::string filepath)
{
	return SceneId();
}

SimpleSceneModule::SceneId SimpleSceneModule::LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend)
{
	return SceneId();
}

bool SimpleSceneModule::RemoveScene(SceneId id)
{
	return false;
}

bool SimpleSceneModule::RemoveScene(std::vector<SCM::EntityId>::const_iterator idstart, std::vector<SCM::EntityId>::const_iterator idend)
{
	return false;
}

void SimpleSceneModule::SwitchActiveScene(SceneId id)
{
}

bool SimpleSceneModule::AddEntity(SceneId sceneid, SCM::EntityId entityid)
{
	return false;
}

bool SimpleSceneModule::AddEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend)
{
	return false;
}

bool SimpleSceneModule::RemoveEntity(SceneId sceneid, SCM::EntityId entityid)
{
	return false;
}

bool SimpleSceneModule::RemoveEntity(SceneId sceneid, std::vector<SCM::EntityId>::const_iterator entityidstart, std::vector<SCM::EntityId>::const_iterator entityidend)
{
	return false;
}
