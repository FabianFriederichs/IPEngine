/** \addtogroup config
Implements the configuration management functionality of the core.
*  @{
*/

/*!
\file ConfigManager.h
\brief configuration manager interface
*/

#ifndef _CONFIG_MANAGER_H_
#define _CONFIG_MANAGER_H_
#include <IPCore/core_config.h>
#include <IPCore/Core/ICoreTypes.h>

namespace ipengine
{
	/*!
	\brief Configuration manager.
	
	The ConfigManager class reads and writes configuration files and provides convenient access to the
	stored configuration values.

	The file format used is a text-based hierarchical format that forms a tree of key/value pairs.
	Supported types are boolean values, integral values, floating point values and strings.

	The structure of such a config file is simple and intuitive:

	\code{.unparsed}
	# this is a comment

	# root section
	root_a
		# subsection
		asub1
			# a value
			someBooleanValue = true
			fancyint = 50
		asub2
			someString = "../Assets/Dependencygraph/TestDepGraph.xml"
			anotherString = "../Output/Win32/Release"
		asub3
			negativeInteger = -2
		thisBelongsToroot_a = true
	root_b
		someSubsection_1
			someIntegerValue = 100
			someFloatingPointValue = -0.4
			anotherFloatingPointValue = 1.e-9
		someSubSection_2
			ASubectionOfASubsecion
				some_bool = false
				someOtherBool = true
	# sections of the same name are merged when the file is loaded
	root_a
		someImportantValue = 42
	\endcode

	Indentations are part of the syntax and indicate a level increment in the tree.
	Individual values can be read and written like the following:

	\code{.cpp}
	bool someValue = configMangerInstance.getBool("root_a.asub1.someBooleanValue");
	configMangerInstance.setInt("root_a.asub1.fancyint", 42);
	\endcode

	New values can be added, too.
	*/
	class CORE_API ConfigManager
	{
	public:
		ConfigManager();
		~ConfigManager();
		/*!
		\brief Loads a configuration file.
		\param[in] path		Path to the configuration file.
		\returns			Returns true if the file was loaded successfully.
		*/
		bool loadConfigFile(const iprstr path);
		/*!
		\brief Writes current values to a configuration file
		\param[in] path		Optional. When no path is passed, the last load file is overwritten. If a path is passed
							a new configuration file is created if it does not exist already.
		\returns			Returns true if the file was written successfully.
		*/
		bool saveConfigFile(const iprstr path = nullptr);

		/*!
		\brief Returns the path to the currently loaded file.
		\returns The path to the currently loaded file.
		*/
		ipstring getCurrentFile();
		
		/*!
		\brief Returns the boolean value of the point-qualified entry name.
		\param[in] configstring		Point-qualified name of the desired value.
		\returns					Returns the value from the config file if the entry was found and the type
									is boolean, otherwise returns false.
		*/
		ipbool			getBool(const iprstr configstring);
		/*!
		\brief Returns the integer value of the point-qualified entry name.
		\param[in] configstring		Point-qualified name of the desired value.
		\returns					Returns the value from the config file if the entry was found and the type
									is integer, otherwise returns 0.
		*/
		ipint64			getInt(const iprstr configstring);
		/*!
		\brief Returns the floating-point value of the point-qualified entry name.
		\param[in] configstring		Point-qualified name of the desired value.
		\returns					Returns the value from the config file if the entry was found and the type
									is floating-point, otherwise returns 0.0f.
		*/
		ipdouble		getFloat(const iprstr configstring);
		/*!
		\brief Returns the string value of the point-qualified entry name.
		\param[in] configstring		Point-qualified name of the desired value.
		\returns					Returns the value from the config file if the entry was found and the type
									is string, otherwise returns an empty string.
		*/
		ipstring		getString(const iprstr configstring);

		/*!
		\brief Adds or sets the value of the given, point-qualified name.
		\param[in] configstring		Point-qualified name of the desired value.
		\param[in] value			The desired value.
		\returns					Returns true if the value was successfully set. Returns false if configstring
									was not a valid value name.
		*/
		ipbool			setBool(const iprstr configstring, ipbool value);
		/*!
		\brief Adds or sets the value of the given, point-qualified name.
		\param[in] configstring		Point-qualified name of the desired value.
		\param[in] value			The desired value.
		\returns					Returns true if the value was successfully set. Returns false if configstring
		was not a valid value name.
		*/
		ipbool			setInt(const iprstr configstring, ipint64 value);
		/*!
		\brief Adds or sets the value of the given, point-qualified name.
		\param[in] configstring		Point-qualified name of the desired value.
		\param[in] value			The desired value.
		\returns					Returns true if the value was successfully set. Returns false if configstring
		was not a valid value name.
		*/
		ipbool			setFloat(const iprstr configstring, ipdouble value);
		/*!
		\brief Adds or sets the value of the given, point-qualified name.
		\param[in] configstring		Point-qualified name of the desired value.
		\param[in] value			The desired value.
		\returns					Returns true if the value was successfully set. Returns false if configstring
		was not a valid value name.
		*/
		ipbool			setString(const iprstr configstring, const iprstr value);

	private:
		class ConfigImpl;
		/*!
		\brief Pointer to implementation
		*/
		ConfigImpl* m_impl;
	};
}



#endif

/** @}*/