#version 460 core
out vec4 FragColor;

in vec4 color;
in vec2 texCoord;

uniform sampler2D texture2d;
uniform bool useTexture;
uniform vec4 objColor;

void main()
{
    if(useTexture) FragColor = texture(texture2d, texCoord)*color;
    else FragColor = objColor;
}