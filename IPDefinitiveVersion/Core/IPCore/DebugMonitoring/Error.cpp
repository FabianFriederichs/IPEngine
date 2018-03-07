#include "Error.h"

ipengine::ipex::ipex()
{}

ipengine::ipex::ipex(ipcrstr message, ipex_severity severity, ipuint32 extag, ipcrstr location)
{}

ipengine::ipex::ipex(const ipex & other)
{}

ipengine::ipex::ipex(ipex && other)
{}

ipengine::ipex & ipengine::ipex::operator=(const ipex & other)
{
	// TODO: hier Rückgabeanweisung eingeben
}

ipengine::ipex & ipengine::ipex::operator=(ipex && other)
{
	// TODO: hier Rückgabeanweisung eingeben
}

ipengine::ipex::~ipex()
{}

ipengine::ipcrstr ipengine::ipex::getMessage()
{
	return ipcrstr();
}

ipengine::ipcrstr ipengine::ipex::getRaiseLocation()
{
	return ipcrstr();
}

ipengine::ipex_severity ipengine::ipex::getSeverity()
{
	return ipex_severity();
}

ipengine::ErrorHandler::ErrorHandler(ErrorManager * manager)
{}
