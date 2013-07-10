#version 140

in vec2 Position;

out vec4 outColor;

void main()
{
	outColor = vec4(Position, 1.0, 1.0);
}
