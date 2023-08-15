/*************
 * Kenneth Cluck
 * 08/07/2023
 * CS-330 Final Project
**************/
#pragma once

// Define vertex shader
const char *vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"	ourColor = aColor;\n"
"}\0";

// Define vertex shader for texture shaders
const char *textureVertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout (location = 2) in vec2 aTexCoord;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 ourColor;\n"
"out vec2 TexCoord;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"	ourColor = aColor;\n"
"	TexCoord = aTexCoord;\n"
"}\0";

// Define fragment shader which handles colors
const char *fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(ourColor, 1.0);\n"
"}\0";

// Define texture shader which handles textures
const char *textureFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord;\n"
"uniform sampler2D texture1;\n"
"void main()\n"
"{\n"
"	FragColor = texture(texture1, TexCoord);\n"
"}\0";

const char *lightVertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"	FragPos = vec3(model * vec4(aPos, 1.0));\n"
"	Normal = aNormal;\n"
"}\0";

const char *lightFragmentShader = "#version 330 core\n"
"struct Material {\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"	float shininess;\n"
"};\n"
"uniform Material material;\n"
"struct Light {\n"
"	vec3 position;\n"
"	vec3 ambient;\n"
"	vec3 diffuse;\n"
"	vec3 specular;\n"
"};\n"
"uniform Light light;\n"
"out vec4 FragColor;\n"
"in vec3 Normal;\n"
"in vec3 FragPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec3 lightPos;\n"
"void main()\n"
"{\n"
"	vec3 ambient = light.ambient * material.ambient;\n"
"	vec3 norm = normalize(Normal);\n"
"	vec3 lightDir = normalize(lightPos - FragPos);\n"
"	float diff = max(dot(norm, lightDir), 0.0);\n"
"	vec3 diffuse = light.diffuse * (diff * material.diffuse);\n"
"	vec3 viewDir = normalize(viewPos - FragPos);\n"
"	vec3 reflectDir = reflect(-lightDir, norm);\n"
"	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);\n"
"	vec3 specular = light.specular * (spec * material.specular);\n"
"	vec3 result = ambient + diffuse + specular;\n"
"	FragColor = vec4(result, 1.0);\n"
"}\0";

const char *ambientVertexShader = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

// Define fragment shader which handles colors
const char *ambientFragmentShader = "#version 330 core\n"
"out vec4 FragColor;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(lightColor * objectColor, 1.0);\n"
"}\0";