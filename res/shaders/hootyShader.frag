
#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D texture;

void main()
{
    FragColor = texture(texture, TexCoord);
}
