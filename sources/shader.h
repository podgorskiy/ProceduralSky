#pragma once
#ifndef _SHADER_H
#define _SHADER_H

/**
\todo 
1) Think about moving blending stuff to separate manager.
2) Implement managing of attributes.
*/

class SBShader{
public:

	enum SB_TYPE
	{
		SB_INT = GL_INT,
		SB_SAMPLER_2D = GL_SAMPLER_2D,
		SB_GL_SAMPLER_CUBE = GL_SAMPLER_CUBE,
		SB_FLOAT = GL_FLOAT,
		SB_INT_VEC2 = GL_INT_VEC2,
		SB_FLOAT_VEC2 = GL_FLOAT_VEC2,
		SB_INT_VEC3 = GL_INT_VEC3,
		SB_FLOAT_VEC3 = GL_FLOAT_VEC3,
		SB_INT_VEC4 = GL_INT_VEC4,
		SB_FLOAT_VEC4 = GL_FLOAT_VEC4,
		SB_FLOAT_MAT2 = GL_FLOAT_MAT2,
		SB_FLOAT_MAT3 = GL_FLOAT_MAT3,
		SB_FLOAT_MAT4 = GL_FLOAT_MAT4,
	};

	class ShaderAttribute
	{
	public:
		ShaderAttribute():d(-1){};
		bool Valid() const
		{
			return d!=-1;
		}
		const ShaderAttribute& operator =(const ShaderAttribute& x){ d = x.d; type = x.type; num = x.num; return *this; };
		const ShaderAttribute& operator =(const int& x){d=x;return *this;};
		operator int() const { return d; };
		SB_TYPE GetType() const { return type; };
		int GetNum() const { return num; };
		void SetType(SB_TYPE t) { type = t; };
		void SetNum(int n)  { num = n; };
	protected:
		int d;
		SB_TYPE type;
		int num;
	};

	SBShader() :name(0), program(-1), wireframe_program(-1), m_mode(No_blend){};
	~SBShader(){};

	/*Methods*/
	
	///\todo  TODO. Perhaps it is not needed 
	//ShaderAttribute&	GetAttribute(const char* name){return *new ShaderAttribute();};
	
	/// Gets attrubute structure of uniform by it's name. 
	/** ShaderAttribute describes unifroms type, size, OpenGL handle */
	ShaderAttribute&	GetUniform(const char* name);

	/// Gets attrubute structure of uniform by it's ID.
	/** ID is assigned to uniform at shader loading stage
	ShaderAttribute describes unifroms type, size, OpenGL handle */
	ShaderAttribute&	GetUniformByID(int id);


	/// Gets count of uniforms attrubute structures that was created.
	int					GetUniformsCount();

	/// Sets internal iterator in shader object to first uniform.
	void				SetIteratorTofirst();

	/// Returns next uniform and it's name. 
	/** Methods SetIteratorTofirst and GetNextUniform is needed just becase it is the only method to get uniforms name,
	as far it is not stored in attrubute structures. */
	SBShader::ShaderAttribute*	GetNextUniform(const char*& name);

	/// Creates new programm
	int					CreateProgramFromConstChar(
							const char* name,
							const char* pVertexShader, 
							const char* pFragmentShader, 
							BlendMode mode = Alpha_1MinusAlpha);

	/// wrapper of glGetAttribLocation
	int					GetAttribLocation(const char *name);

	/// wrapper of glGetUniformLocation
	int					GetUniformLocation(const char *name);

	/// Sets current shader for render
	void				UseIt();
	void				UseItDebug();

private:
	/// Loads and compile shader. For internal use only
	int loadShader(GLenum shaderType, const char* pSource, bool quiet = false);
	/// Print source. For internal use only
	void PrintSource(const char*);

public:

	//Attributes
	ShaderAttribute positionAttribute;
	ShaderAttribute normalAttribute;
	ShaderAttribute tangentAttribute;
	ShaderAttribute binormalAttribute;
	ShaderAttribute additiveColorAttribute;
	ShaderAttribute mulColorAttribute;
	ShaderAttribute alphaAttribute;
	ShaderAttribute layersAttribute;
	ShaderAttribute specAttribute;
	ShaderAttribute edgeNormAttribute;
	ShaderAttribute uvAttribute;
	ShaderAttribute uvAttribute2;
	ShaderAttribute bonesIdsAttribute;
	ShaderAttribute bonesWeightsAttribute;
	
private:
	int program;
	int wireframe_program;
	BlendMode m_mode;

	std::vector<ShaderAttribute>		m_uniforms;
	std::map<SBString, int>				m_uniformsNames;
	std::map<SBString, int>::iterator	m_uniformsNamesIt;

	static int current_shader;	///< to reduce switching from shader to shader
	static BlendMode ms_blendMode;
	const char* name;
};



inline void SBShader::UseIt(){
	if (current_shader != program){
		glUseProgram(program);
		current_shader = program;
	}
	if (ms_blendMode != m_mode)
	{
		switch (m_mode)
		{
		case Alpha_1MinusAlpha:
		{
								  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
								  if (ms_blendMode == No_blend)
								  {
									  glEnable(GL_BLEND);
								  }
								  break;
		}
		case Alpha_1:
		{
						glBlendFunc(GL_SRC_ALPHA, GL_ONE);
						if (ms_blendMode == No_blend)
						{
							glEnable(GL_BLEND);
						}
						break;
		}
		case No_blend:
		{
						 glBlendFunc(GL_ONE, GL_ZERO);
						 glDisable(GL_BLEND);
						 break;
		}
		}
		ms_blendMode = m_mode;
	}
}

inline void SBShader::UseItDebug(){
	if (current_shader != wireframe_program){
		glUseProgram(wireframe_program);
		current_shader = wireframe_program;
	}
}

inline int SBShader::GetAttribLocation(const char *name){
	return glGetAttribLocation(program, name);
};
inline int SBShader::GetUniformLocation(const char *name){
	return glGetUniformLocation(program, name);
};

#endif
 
//........... <-- this was writen by my daughter