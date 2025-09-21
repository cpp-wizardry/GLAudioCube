#ifndef GLOBAL_SHADER_H
#define	GLOBAL_SHADER_H


//const char* vertexShaderSource = R"(
//#version 330 core
//layout (location = 0) in vec3 aPos;
//uniform mat4 mvp;
//void main() {
//    gl_Position = mvp * vec4(aPos, 1.0);
//}
//)";

inline const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 transform;

void main()
{
    gl_Position = transform * vec4(aPos, 1.0);
}
)glsl";


inline const char* IvertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main() {
    gl_Position = transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)";

inline const char* lightVertexShader = R"(#version 330 core

layout(location = 0) in vec3 aPos;   
layout(location = 1) in vec2 aTex;   
layout(location = 2) in vec3 aNormal; 

out vec2 TexCoord;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 transform;   
uniform mat4 model;      

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aTex;

    gl_Position = transform * vec4(aPos, 1.0);
}
)";

inline const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

inline const char* IfragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D uTexture;

void main() {
    FragColor = texture(uTexture, TexCoord);
}
)";

inline const char* lightFragmentShader = R"(#version 330 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform sampler2D texture1;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

uniform vec3 objectColor;

void main()
{
   
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

   
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

   
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor * texture(texture1, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}
)";

#endif // !GLOBAL_SHADER_H
