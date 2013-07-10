#version 140

in vec2 position;

out vec2 Position;

void main()
{
	Position = position * 2;
	gl_Position = vec4(position, 0.0, 1.0);
}
