#version 140

in vec2 position;

uniform mat4 camera_transform;
uniform mat4 perspective_transform;

out vec2 Position;

void main()
{
	gl_Position = vec4(position, 0.0, 1.0);
	Position = (perspective_transform * camera_transform * vec4(position, 0.0, 1.0)).xy;
}
