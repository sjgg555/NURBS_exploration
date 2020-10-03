#pragma once
#include <string>

class TestVertexShader
{
private:
	std::string m_compiledShaderObject = "SimpleVertexShader.cso";
public:
	TestVertexShader();
	~TestVertexShader();
};