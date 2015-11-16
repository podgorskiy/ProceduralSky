#pragma once
#include "IFile.h"

#include <GL/gl3w.h>
#include <map>
#include <vector>
#include <string>
#include <glm/matrix.hpp>


namespace SB
{
	class Shader
	{
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
			ShaderAttribute() :d(-1){};
			bool Valid() const
			{
				return d != -1;
			}
			const ShaderAttribute& operator =(const ShaderAttribute& x){ d = x.d; type = x.type; num = x.num; return *this; };
			const ShaderAttribute& operator =(const int& x){ d = x; return *this; };
			operator int() const { return d; };
			SB_TYPE GetType() const { return type; };
			int GetNum() const { return num; };
			void SetType(SB_TYPE t) { type = t; };
			void SetNum(int n)  { num = n; };

			template<typename T>
			void SetValue(const T& value);

		protected:
			int d;
			SB_TYPE type;
			int num;
		};

		class AttributeGuard
		{
		public:
			AttributeGuard(const ShaderAttribute& attribute);
			~AttributeGuard();

		private:
			int m_attribute;
		};

		Shader() :name(0), program(-1), wireframe_program(-1){};
		~Shader(){};

		/// Gets attrubute structure of uniform by it's name. 
		/** ShaderAttribute describes unifroms type, size, OpenGL handle */
		ShaderAttribute GetUniform(const char* name);
		int GetUniformID(const char* name);

		/// Gets attrubute structure of uniform by it's ID.
		/** ID is assigned to uniform at shader loading stage
		ShaderAttribute describes unifroms type, size, OpenGL handle */
		ShaderAttribute GetUniformByID(int id);

		/// Gets count of uniforms attrubute structures that was created.
		int GetUniformsCount();

		/// Sets internal iterator in shader object to first uniform.
		void SetIteratorTofirst();

		/// Returns next uniform and it's name. 
		/** Methods SetIteratorTofirst and GetNextUniform is needed just becase it is the only method to get uniforms name,
		as far it is not stored in attrubute structures. */
		ShaderAttribute* GetNextUniform(const char*& name);

		/// Creates new programm
		int CreateProgramFrom(const char* name,
			const char* pVertexShader,
			const char* pFragmentShader);

		int CreateProgramFrom(const char* name,
			IFile* pVertexShader,
			IFile* pFragmentShader);

		/// wrapper of glGetAttribLocation
		int GetAttribLocation(const char *name);

		/// wrapper of glGetUniformLocation
		int GetUniformLocation(const char *name);
		
		/// Sets current shader for render
		void UseIt();
		void UseItDebug();

	private:
		/// Loads and compile shader. For internal use only
		int loadShader(GLenum shaderType, const char* pSource, bool quiet = false);
		/// Print source. For internal use only
		void PrintSource(const char*);

	public:

		//Attributes
		ShaderAttribute positionAttribute;
		ShaderAttribute normalAttribute;
		ShaderAttribute colorAttribute;
		ShaderAttribute tangentAttribute;
		ShaderAttribute binormalAttribute;
		ShaderAttribute alphaAttribute;
		ShaderAttribute uvAttribute;
		ShaderAttribute uvAttribute2;

		//Common uniforms
		ShaderAttribute u_WVP;
		ShaderAttribute u_W;

	private:
		int program;
		int wireframe_program;

		std::vector<ShaderAttribute>			m_uniforms;
		std::map<std::string, int>				m_uniformsNames;
		std::map<std::string, int>::iterator	m_uniformsNamesIt;

		static int current_shader;	///< to reduce switching from shader to shader
		const char* name;
	};

	inline void Shader::UseIt()
	{
		if (current_shader != program){
			glUseProgram(program);
			current_shader = program;
		}
	}

	inline void Shader::UseItDebug()
	{
		if (current_shader != wireframe_program)
		{
			glUseProgram(wireframe_program);
			current_shader = wireframe_program;
		}
	}

	inline int Shader::GetAttribLocation(const char *name)
	{
		return glGetAttribLocation(program, name);
	}

	inline int Shader::GetUniformLocation(const char *name)
	{
		return glGetUniformLocation(program, name);
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<int>(const int& value)
	{
		assert(GetType() == SB_FLOAT_MAT4);
		glUniform1iv(d, 1, reinterpret_cast<const GLint*>(&value));
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<glm::mat4>(const glm::mat4& value)
	{
		assert(GetType() == SB_FLOAT_MAT4);
		glUniformMatrix4fv(d, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&value));
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<glm::vec4>(const glm::vec4& value)
	{
		assert(GetType() == SB_FLOAT_VEC4);
		glUniform4fv(d, 1, reinterpret_cast<const GLfloat*>(&value));
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<glm::vec3>(const glm::vec3& value)
	{
		assert(GetType() == SB_FLOAT_VEC3);
		glUniform3fv(d, 1, reinterpret_cast<const GLfloat*>(&value));
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<glm::vec2>(const glm::vec2& value)
	{
		assert(GetType() == SB_FLOAT_VEC2);
		glUniform2fv(d, 1, reinterpret_cast<const GLfloat*>(&value));
	}

	template<>
	inline void Shader::ShaderAttribute::SetValue<float>(const float& value)
	{
		assert(GetType() == SB_FLOAT);
		glUniform1fv(d, 1, reinterpret_cast<const GLfloat*>(&value));
	}

}