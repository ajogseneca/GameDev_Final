#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 ourColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    vec4 texColor1 = texture(texture1, TexCoord);
    vec4 texColor2 = texture(texture2, TexCoord);
    vec4 texture_clr = mix(texColor1, texColor2, 0.5);
    texture_clr *= vec4(ourColor, 1.0);
    FragColor = texture_clr;
}

