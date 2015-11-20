#include "SBShader.h"

#include <cassert>
#include <iostream>

using namespace SB;

int Shader::current_shader=-1;

void Shader::PrintSource(const char* source)
{
	int i;
	int size = strlen(source);
	char* t = new char[size + 1];
	int j=0;
	int line = 1;
	for(i=0; i<size; i++)
	{
		t[j] = source[i];
		bool lineBreak = t[j] == '\n';
		bool endOfSource = i == (size - 1);

		if (lineBreak || endOfSource)
		{
			if (endOfSource)
			{
				j++;
			}
			t[j]=0;
			j=0;
			printf("%3d:\t%s\n",line++,t);
		}
		else
		{
			++j;
		}
	}
	delete[] t;
}

int Shader::loadShader(GLenum shaderType, const char* pSource, bool quiet)
{
	m_uniforms.reserve(20);

	GLuint shader = glCreateShader(shaderType);
	
	if (shader)
	{
		glShaderSource(shader, 1, &pSource, NULL);
		if (!quiet)
		{
			if (GL_VERTEX_SHADER==shaderType)
			{
				std::cout << "Compiling vertex shader   :  " << name << std::endl;
			}
			if (GL_FRAGMENT_SHADER==shaderType)
			{
				std::cout << "Compiling fragment shader :  " << name << std::endl;
			}
		}
		
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		
		if (!compiled) 
		{
			std::cout << "\n  <----------------- ERROR ! ----------------->\n";
		}
		
		if ((infoLen>1) && (!quiet || !compiled)) 
		{
			if (compiled)
			{
				std::cout << "\n  <--- Warning ! --->\n";
			}
			
			PrintSource(pSource);
			
			char* buf = new char[infoLen];
			glGetShaderInfoLog(shader, infoLen, NULL, buf);
			std::cout << "Compilation log:\n" << buf << std::endl;
			delete[] buf;
			
			if (!compiled) 
			{
				glDeleteShader(shader);
				shader = 0;
			}
		}
		
		if (!compiled) 
		{
			std::cout << "\n  <------------------------------------------->\n";
		}
	}
	return shader;
}

int Shader::CreateProgramFrom(const char *name, const char* pVertexShader, const char* pFragmentShader)
{
	this->name = name; 
	
	GLuint vertexshader =   loadShader(GL_VERTEX_SHADER,   pVertexShader);

	GLuint fragmentshader = loadShader(GL_FRAGMENT_SHADER, pFragmentShader);

	int	prog = glCreateProgram();		 	// create empty OpenGL Program
	glAttachShader(prog, vertexshader);		// add the vertex shader to program
	glAttachShader(prog, fragmentshader);	// add the fragment shader to program
	glLinkProgram(prog);	 

	// Check the link status
	int linked;
	glGetProgramiv(prog, GL_LINK_STATUS, &linked);
	if(!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1)
		{
			char* infoLog = new char[infoLen];
			glGetProgramInfoLog(prog, infoLen, NULL, infoLog);
			std::cout << "Error linking program:\n" << infoLog << std::endl;
			delete[] infoLog;
			return EXIT_FAILURE;
		}
		glDeleteProgram(prog);
	}
	program = prog;
	int total = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);
	
	for (int i = 0; i<total; ++i)  
	{
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(program, GLuint(i), sizeof(name)-1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetUniformLocation(program, name);
		ShaderAttribute uniform;
		uniform = location;
		uniform.SetType((Shader::SB_TYPE)type);
		uniform.SetNum(num);
		m_uniforms.push_back(uniform);
		m_uniformsNames[name] = m_uniforms.size() - 1;
	}
	
	//Attributes
	positionAttribute = GetAttribLocation("a_pos");
	normalAttribute = GetAttribLocation("a_normal");
	colorAttribute = GetAttribLocation("a_color");
	tangentAttribute = GetAttribLocation("a_tangent");
	binormalAttribute = GetAttribLocation("a_binormal");
	alphaAttribute = GetAttribLocation("a_alpha");
	uvAttribute = GetAttribLocation("a_uv");
	uvAttribute2 = GetAttribLocation("a_uv2");

	//Common uniforms
	u_WVP = GetUniform("u_WVP");
	u_W = GetUniform("u_W");
	
	#ifndef NDEBUG
	fragmentshader = loadShader(GL_FRAGMENT_SHADER, 
"void main() {\n"
"	gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
"}\n",true);
	prog = glCreateProgram();		 // create empty OpenGL Program
	glAttachShader(prog, vertexshader);   // add the vertex shader to program
	glAttachShader(prog, fragmentshader); // add the fragment shader to program
	glLinkProgram(prog); 
	wireframe_program = prog;
	#endif
	
	return EXIT_SUCCESS;
}


int Shader::CreateProgramFrom(const char* name,
	IFile* pVertexShader,
	IFile* pFragmentShader)
{
	std::string vshader;
	vshader.resize(pVertexShader->GetSize());
	pVertexShader->Read(&vshader[0], pVertexShader->GetSize());
	std::string fshader;
	fshader.resize(pFragmentShader->GetSize());
	pFragmentShader->Read(&fshader[0], pFragmentShader->GetSize());
	return CreateProgramFrom(name, vshader.c_str(), fshader.c_str());
}

int Shader::GetUniformID(const char* name)
{
	std::map<std::string, int>::iterator it = m_uniformsNames.find(name);
	if (it != m_uniformsNames.end())
	{
		return it->second;
	}
	return -1;
}

Shader::ShaderAttribute Shader::GetUniform(const char* name)
{
	std::map<std::string, int>::iterator it = m_uniformsNames.find(name);
	if (it != m_uniformsNames.end())
	{
		return m_uniforms[it->second];
	}
	return ShaderAttribute();
}

Shader::ShaderAttribute Shader::GetUniformByID(int id)
{
	assert(id<static_cast<int>(m_uniforms.size())); // Wrong id
	return m_uniforms[id];
}

int Shader::GetUniformsCount()
{
	return m_uniformsNames.size();
}

void Shader::SetIteratorTofirst()
{
	m_uniformsNamesIt = m_uniformsNames.begin();
}

Shader::ShaderAttribute* Shader::GetNextUniform(const char*& name)
{
	name = m_uniformsNamesIt->first.c_str();
	return &m_uniforms[(m_uniformsNamesIt++)->second];
}

Shader::AttributeGuard::AttributeGuard(const ShaderAttribute& attribute)
{
	m_attribute = attribute;
	glEnableVertexAttribArray(m_attribute);
};

Shader::AttributeGuard::~AttributeGuard()
{
	glDisableVertexAttribArray(m_attribute);
};
