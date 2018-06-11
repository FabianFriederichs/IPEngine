#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_
#include <core/core_config.h>
#include <core/core_types.h>

namespace ipengine
{
	class CORE_API ConfigManager
	{
	public:
		ConfigManager();
		~ConfigManager();
		//loads a config file
		bool loadConfigFile(const iprstr path);
		//writes cached config data to last loaded file (per default) or to given new config file
		bool saveConfigFile(const iprstr path = nullptr);
		ipstring getCurrentFile();
		
		ipbool			getBool(const iprstr configstring);
		ipint64			getInt(const iprstr configstring);
		ipdouble		getFloat(const iprstr configstring);
		ipstring		getString(const iprstr configstring);

		ipbool			setBool(const iprstr configstring, ipbool value);
		ipbool			setInt(const iprstr configstring, ipint64 value);
		ipbool			setFloat(const iprstr configstring, ipdouble value);
		ipbool			setString(const iprstr configstring, const iprstr value);

	private:
		class ConfigImpl; ConfigImpl* m_impl;
	};
}



#endif