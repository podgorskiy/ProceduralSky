#include <vector>
#include <map>
#include "common.h"

#include "memleak.h"

#include "SBScene/SBString.h"
#include "SBShader.h"


#undef MessageBox
#define MessageBox(...) ;

int SBShader::current_shader=-1;
BlendMode SBShader::ms_blendMode=Alpha_1MinusAlpha;

void SBShader::PrintSource(const char* source)
{
	int i;
	for(i=0;i<100000;i++)
		if(source[i]==0) break;
	int size = i;
	char* t = new char[size];
	int j=0;
	int line = 1;
	for(i=0;i<size;i++)
	{
		t[j]=source[i];
		if( (t[j++]=='\n') || (i==size-1) ){
			t[--j]=0;
			j=0;
			LOG_("%3d:   %s",line++,t);
		}
	}
	delete[] t;
}

int SBShader::loadShader(GLenum shaderType, const char* pSource, bool quiet){

	m_uniforms.reserve(20);

	GLuint shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
		if(!quiet){
		if(GL_VERTEX_SHADER==shaderType){
			LOG_GLSL("Compiling vertex shader   :  %s",name);}
		if(GL_FRAGMENT_SHADER==shaderType){
			LOG_GLSL("Compiling fragment shader :  %s",name);}
		}
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (!compiled) {
			LOG_("\n  <----------------- ERROR ! ----------------->\n");
		}
        if ((infoLen>1) && (!quiet || !compiled)) {
			if(compiled){
				LOG_("\n  <--- Warning ! --->\n");}
			PrintSource(pSource);
            char* buf = (char*) malloc(infoLen);
            if (buf) {
                glGetShaderInfoLog(shader, infoLen, NULL, buf);
				LOG_GLSL("Compilation log:\n%s",buf);
                free(buf);
            }
			if (!compiled) {
            glDeleteShader(shader);
            shader = 0;
			}
        }        
		if (!compiled) {
			LOG_("\n  <------------------------------------------->\n");
		}
    }
    return shader;
}

int SBShader::CreateProgramFromConstChar( const char *name, const char* pVertexShader, const char* pFragmentShader, BlendMode mode){
	this->name = name; 
	m_mode = mode;
	
	GLuint vertexshader =   loadShader(GL_VERTEX_SHADER,   pVertexShader);

	GLuint fragmentshader = loadShader(GL_FRAGMENT_SHADER, pFragmentShader);

	int	prog = glCreateProgram();         // create empty OpenGL Program
	glAttachShader(prog, vertexshader);   // add the vertex shader to program
	glAttachShader(prog, fragmentshader); // add the fragment shader to program
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
			char* infoLog = (char*)malloc(sizeof(char) * infoLen);
			glGetProgramInfoLog(prog, infoLen, NULL, infoLog);
			LOG_GLSL("Error linking program:\n%s\n", infoLog);
			free(infoLog);
			return 0;
		}
		glDeleteProgram(prog);
	}
	program = prog;
	int total = -1;
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &total);
    
	for (int i = 0; i<total; ++i)  {
		int name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(program, GLuint(i), sizeof(name)-1,
			&name_len, &num, &type, name);
		name[name_len] = 0;
		GLuint location = glGetUniformLocation(program, name);
		ShaderAttribute uniform;
		uniform = location;
		uniform.SetType((SBShader::SB_TYPE)type);
		uniform.SetNum(num);
		m_uniforms.push_back(uniform);
		m_uniformsNames[SBString(name)] = m_uniforms.size() - 1;
	}
	
	positionAttribute = GetAttribLocation("a_pos");
	normalAttribute = GetAttribLocation("a_normal");
	tangentAttribute = GetAttribLocation("a_tangent");
	binormalAttribute = GetAttribLocation("a_binormal");
	additiveColorAttribute = GetAttribLocation("a_additiveColor");
	mulColorAttribute = GetAttribLocation("a_mulColor");
	alphaAttribute = GetAttribLocation("a_alpha");
	layersAttribute = GetAttribLocation("a_layers");
	specAttribute = GetAttribLocation("a_spec");
	edgeNormAttribute = GetAttribLocation("a_edgeNorm");
	uvAttribute = GetAttribLocation("a_uv");
	uvAttribute2 = GetAttribLocation("a_uv2");
	bonesIdsAttribute = GetAttribLocation("a_bonesIds");
	bonesWeightsAttribute = GetAttribLocation("a_bonesWeights");
	
    #ifndef NDEBUG
	fragmentshader = loadShader(GL_FRAGMENT_SHADER, 
"void main() {\n"
"	gl_FragColor = vec4(1.0,1.0,1.0,1.0);\n"
"}\n",true);
	prog = glCreateProgram();         // create empty OpenGL Program
	glAttachShader(prog, vertexshader);   // add the vertex shader to program
	glAttachShader(prog, fragmentshader); // add the fragment shader to program
	glLinkProgram(prog); 
	wireframe_program = prog;
	#endif
	
	return 0;
}

SBShader::ShaderAttribute& SBShader::GetUniform(const char* name)
{
	std::map<SBString, int>::iterator it = m_uniformsNames.find(name);
	if (it != m_uniformsNames.end())
	{
		return m_uniforms[it->second];
	}
	LOGE("Uniform not found: %s", name);
	return m_uniforms[0];
}

SBShader::ShaderAttribute& SBShader::GetUniformByID(int id)
{
	PASSERT2(id<static_cast<int>(m_uniforms.size()), "Wrong ID: %d", id);
	return m_uniforms[id];
}

int SBShader::GetUniformsCount()
{
	return m_uniformsNames.size();
}

void SBShader::SetIteratorTofirst()
{
	m_uniformsNamesIt = m_uniformsNames.begin();
}

SBShader::ShaderAttribute* SBShader::GetNextUniform(const char*& name)
{
	name = m_uniformsNamesIt->first.c_str();
	return &m_uniforms[(m_uniformsNamesIt++)->second];
}

