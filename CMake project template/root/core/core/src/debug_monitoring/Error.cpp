#include <core/debug_monitoring/Error.h>

ipengine::ipex::ipex() :
	message(),
	raiseloc(),
	severity(ipex_severity::error),
	extag(0)
{}

ipengine::ipex::ipex(ipcrstr message, ipex_severity severity, ipuint32 extag, ipcrstr location) :
	message(message),
	severity(severity),
	extag(extag),
	raiseloc(location)
{}

ipengine::ipex::ipex(const ipex & other) :
	message(other.message),
	severity(other.severity),
	extag(other.extag),
	raiseloc(other.raiseloc)
{}

ipengine::ipex::ipex(ipex && other) :
	message(std::move(other.message)),
	severity(other.severity),
	extag(other.extag),
	raiseloc(std::move(other.raiseloc))
{}

ipengine::ipex & ipengine::ipex::operator=(const ipex & other)
{
	if (this == &other)
		return *this;

	message = other.message;
	severity = other.severity;
	extag = other.extag;
	raiseloc = other.raiseloc;

	return *this;

}

ipengine::ipex & ipengine::ipex::operator=(ipex && other)
{
	if (this == &other)
		return *this;

	message = std::move(other.message);
	severity = other.severity;
	extag = other.extag;
	raiseloc = std::move(other.raiseloc);

	return *this;
}

ipengine::ipex::~ipex()
{}

ipengine::ipcrstr ipengine::ipex::getMessage()
{
	return message.c_str();
}

ipengine::ipcrstr ipengine::ipex::getRaiseLocation()
{
	return raiseloc.c_str();
}

ipengine::ipex_severity ipengine::ipex::getSeverity()
{
	return severity;
}

ipengine::ipuint32 ipengine::ipex::getExTag()
{
	return extag;
}