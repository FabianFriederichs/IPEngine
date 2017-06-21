#ifndef _FW_CONFIG_H
#define _FW_CONFIG_H

//Debug settings for glerror.h
#define THROW_ON_GL_ERROR 1		//throw exception when OpenGL error occures
#define HOLD_ON_GL_ERROR 0		//print error and wait for a key when OpenGL error occures
#define LOG_GL_ERRORS 1			//Log all errors to "glerrorlog.txt"
#define CGA2_DEBUG					//if not defined, GLERR does nothing

#endif
