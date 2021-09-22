#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform float x_step;
uniform float y_step;
uniform float tick;
uniform sampler2D ourTexture;

void main()
{
    FragColor = texture(ourTexture, TexCoord + vec2(tick/10.0, 0.0f));
//    FragColor = vec4(0.0, 0.0, 0.0, 1.0f);
}