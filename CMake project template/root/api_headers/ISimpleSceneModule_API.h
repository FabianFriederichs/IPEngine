#ifndef _ISIMPLESCENEMODULE_API_H_
#define _ISIMPLESCENEMODULE_API_H_
#include <IModule_API.h>
#include <ISimpleContentModule_API.h>

class ISimpleSceneModule_API : public IModule_API
{
public:

	//Load scene file, return id
	virtual ipengine::ipid LoadSceneFromFile(std::string filepath) = 0;
	virtual std::vector<ipengine::ipid> LoadSceneFromFile(std::vector<std::string>::const_iterator filepathstart, std::vector<std::string>::const_iterator filepathend) = 0;
	virtual void WriteSceneToFile(std::string filepath, ipengine::ipid sceneid) =0;
	virtual bool RemoveScene(ipengine::ipid id) = 0;
	virtual int RemoveScene(std::vector<ipengine::ipid>::const_iterator idstart, std::vector<ipengine::ipid>::const_iterator idend) = 0;

	//Switch scene, take scene id
	virtual bool SwitchActiveScene(ipengine::ipid id) = 0;

	//Edit scene, add/remove entity?
	virtual bool AddEntity(ipengine::ipid entityid, ipengine::ipid sceneid = std::numeric_limits<ipengine::ipid>::max()) = 0;
	virtual int AddEntity( std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator  entityidend, ipengine::ipid sceneid = std::numeric_limits<ipengine::ipid>::max()) = 0;

	virtual bool RemoveEntity(ipengine::ipid entityid, ipengine::ipid sceneid = std::numeric_limits<ipengine::ipid>::max()) = 0;
	virtual int RemoveEntity(std::vector<ipengine::ipid>::const_iterator entityidstart, std::vector<ipengine::ipid>::const_iterator  entityidend, ipengine::ipid sceneid = std::numeric_limits<ipengine::ipid>::max()) = 0;
};

#endif// !_ISIMPLESCENEMODULE_API_H_
