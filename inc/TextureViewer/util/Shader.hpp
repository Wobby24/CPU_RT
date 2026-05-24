#ifndef SHADER_H
#define SHADER_H

#include <string>

class Shader
{
public:
    // the program ID
    unsigned int ID;

    // constructor reads and builds the shader
 //   Shader(std::string vertexPath, std::string fragmentPath);
    Shader(const char* vertCode, const char* fragCode);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
private:
    bool isGladInitialized() const;
    void CreateShader();
    bool isShaderCreated;
  //  std::string m_vsPath;
  //  std::string m_fsPath;
    const char* vertexCode;
    const char* fragmentCode;
    void checkCompileErrors(unsigned int shader, std::string type);
};

#endif
