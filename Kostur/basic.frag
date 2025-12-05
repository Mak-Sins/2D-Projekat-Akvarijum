#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform float alpha;
uniform vec3 overrideColor;

void main()
{
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = vec4(texColor.rgb, texColor.a * alpha);
}
