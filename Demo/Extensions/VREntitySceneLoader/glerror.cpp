#include "glerror.h"

void printglerror(const char* file, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::stringstream messagestream;
		messagestream << "An OpenGL error occured at file: \"" << file << "\", line: " << line << ":\n";
		
		switch (err)
		{
		case GL_INVALID_ENUM:
			messagestream << "invalid enum";
			break;
		case GL_INVALID_VALUE:
			messagestream << "invalid value";
			break;
		case GL_INVALID_OPERATION:
			messagestream << "invalid operation";
			break;
		case GL_STACK_OVERFLOW:
			messagestream << "stack overflow";
			break;
		case GL_STACK_UNDERFLOW:
			messagestream << "stack underflow";
			break;
		case GL_OUT_OF_MEMORY:
			messagestream << "out of memory";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			messagestream << "invalid framebuffer operation";
			break;
		case GL_CONTEXT_LOST:
			messagestream << "context lost";
			break;
		default:
			messagestream << "unknown gl error";
		}
		std::cerr  << messagestream.str() << std::endl;
		if (HOLD_ON_GL_ERROR)
			std::getchar();

		if (LOG_GL_ERRORS)
		{
			try
			{
				std::fstream fs;
				fs.open("glerrorlog.txt", std::ios_base::out | std::ios_base::app);
				if (fs.is_open())
				{
					fs << messagestream.str() << std::endl;
					fs.close();
				}
			}
			catch (const std::exception& ex)
			{
				std::string ioerr = "Error: Unable to write glerror.txt : ";
				ioerr.append(ex.what());
				throw std::runtime_error(ioerr.c_str());
			}
		}

		if (THROW_ON_GL_ERROR)
			throw std::logic_error(messagestream.str());
	}
}

bool checkglerror_(const char* file, int line)
{
	GLenum err = glGetError();
	if (err != GL_NO_ERROR)
	{
		std::stringstream messagestream;
		messagestream << "An OpenGL error occured at file: \"" << file << "\", line: " << line << ":\n";
		switch (err)
		{
		case GL_INVALID_ENUM:
			messagestream << "invalid enum";
			break;
		case GL_INVALID_VALUE:
			messagestream << "invalid value";
			break;
		case GL_INVALID_OPERATION:
			messagestream << "invalid operation";
			break;
		case GL_STACK_OVERFLOW:
			messagestream << "stack overflow";
			break;
		case GL_STACK_UNDERFLOW:
			messagestream << "stack underflow";
			break;
		case GL_OUT_OF_MEMORY:
			messagestream << "out of memory";
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			messagestream << "invalid framebuffer operation";
			break;
		case GL_CONTEXT_LOST:
			messagestream << "context lost";
			break;
		default:
			messagestream << "unknown gl error";
		}
		std::cerr << messagestream.str() << std::endl;

		if (LOG_GL_ERRORS)
		{
			try
			{
				std::fstream fs;
				fs.open("glerrorlog.txt", std::ios_base::out | std::ios_base::app);
				if (fs.is_open())
				{
					fs << messagestream.str() << std::endl;
					fs.close();
				}
			}
			catch (const std::exception& ex)
			{
				std::string ioerr = "Error: Unable to write glerror.txt : ";
				ioerr.append(ex.what());
				throw std::runtime_error(ioerr.c_str());
			}
		}

		return true;
	}
	else
	{
		return false;
	}
}