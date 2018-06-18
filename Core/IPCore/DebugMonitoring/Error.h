/** \addtogroup debug
*  @{
*/

/*!
\file Error.h
\brief Custom exception types
*/
#ifndef _ERROR_H_
#define _ERROR_H_
#include <IPCore/core_config.h>
#include <IPCore/Core/ICoreTypes.h>


namespace ipengine
{
	/*!
	\brief Severity of an exception.
	*/
	enum class ipex_severity
	{
		fatal, //!< Fatal error, the system has entered an errornous state that cannot be recovered from
		error, //!< An error that can be managed by the subsystem which produced it
		warning, //!< Warning
		info //!< Information
	};

	/*!
	\brief A custom exception class

	This class represents the common exception type that should be used throughout the system.
	It stores a message, a hint that tells where the exception was raised, a severity and a customizable tag.

	The tag can be used to differentiate between exception types without having to write subclasses of ipex.
	*/
	class CORE_API ipex
	{
	private:
		//! The error message
		ipstring message;
		//! A message that tells where the exception was raised
		ipstring raiseloc;
		//! Severity of the exception
		ipex_severity severity;
		//! A tag that can be used to differentiate between exception types
		ipuint32 extag;

	public:
		/*!
		\brief Creates a fatal exception with no message.
		*/
		ipex();
		/*!
		\brief Creates an exceptions with the given parameters.
		\param[in] message		Error message
		\param[in] severity		Severity of the exception. Defaults to ipex_severity::fatal.
		\param[in] extag		A freely choosable tag. Defaults to 0.
		\param[in] location		A string that tells the raise location. Defaults to an empty string.
		*/
		ipex(ipcrstr message, ipex_severity severity = ipex_severity::fatal, ipuint32 extag = 0, ipcrstr location = "");
		//! Copy constructor
		ipex(const ipex& other);
		//! Move constructor
		ipex(ipex&& other);
		//! Copy assignment operator
		ipex& operator=(const ipex& other);
		//! Move assignment operator
		ipex& operator=(ipex&& other);

		//! Destructor
		virtual ~ipex();
		/*!
		\brief Returns the error message.
		\returns	Returns the error message.
		*/
		virtual ipcrstr getMessage();
		/*!
		\brief Returns the raise location.
		\returns	Returns the raise location.
		*/
		virtual ipcrstr getRaiseLocation();
		/*!
		\brief Returns the severity.
		\returns	Returns the severity.
		*/
		virtual ipex_severity getSeverity();
		/*!
		\brief Returns the exception tag.
		\returns	Returns the exception tag.
		*/
		virtual ipuint32 getExTag();
	};
}
#endif

/** @}*/