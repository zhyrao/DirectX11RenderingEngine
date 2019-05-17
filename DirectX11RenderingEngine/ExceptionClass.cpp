#include "ExceptionClass.h"
#include <sstream>

ExceptionClass::ExceptionClass(int line, const char* file) noexcept
	:
	line(line),
	file(file)
{
}

const char* ExceptionClass::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* ExceptionClass::GetType() const noexcept
{
	return "DX Exception";
}

int ExceptionClass::GetLine() const noexcept
{
	return line;
}

const std::string& ExceptionClass::GetFile() const noexcept
{
	return file;
}

std::string ExceptionClass::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << file << std::endl
		<< "[Line] " << line;
	return oss.str();
}

ExceptionClass::~ExceptionClass()
{

}

