#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>




class ShaderManager
{
public:


	ShaderManager(std::string const &VertPath, std::string const& FragPath);
	
	~ShaderManager();



	unsigned int compileShader(unsigned int type, const char* source);
	
	bool loadShaders(std::vector<char*> vertShaders, std::vector<char*> fragShaders);
	

private:
	std::string m_FragPath;
	std::string m_VertPath;
	std::vector<char*> m_VertShaders;
	std::vector<char*> m_FragShaders;
	std::vector<const char*> source;


};

