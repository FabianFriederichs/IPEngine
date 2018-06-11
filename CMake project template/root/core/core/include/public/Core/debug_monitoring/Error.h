#ifndef _ERROR_H_
#define _ERROR_H_
#include <core/core_config.h>
#include <core/core_types.h>


namespace ipengine
{
	enum class ipex_severity
	{
		fatal,
		error,
		warning,
		info
	};

	class CORE_API ipex
	{
	private:
		ipstring message;
		ipstring raiseloc;
		ipex_severity severity;
		ipuint32 extag;

	public:
		ipex();
		ipex(ipcrstr message, ipex_severity severity = ipex_severity::fatal, ipuint32 extag = 0, ipcrstr location = "");
		ipex(const ipex& other);
		ipex(ipex&& other);
		ipex& operator=(const ipex& other);
		ipex& operator=(ipex&& other);

		virtual ~ipex();
		virtual ipcrstr getMessage();
		virtual ipcrstr getRaiseLocation();
		virtual ipex_severity getSeverity();
		virtual ipuint32 getExTag();
	};
}
#endif