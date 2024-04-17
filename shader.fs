#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Texture coordinates passed from vertex shader
uniform sampler2D texture1; // Texture uniform

void main()
{
    FragColor = texture(texture1, TexCoords);
}
