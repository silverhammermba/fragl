#version 140

in vec2 Position;

out vec4 outColor;

uniform int steps;

vec2 complex_square(vec2 v)
{
	return vec2(v.x * v.x - v.y * v.y, v.x * v.y * 2);
}

float abs_squared(vec2 v)
{
	return v.x * v.x + v.y * v.y;
}

void main()
{
	float grade = 1.0 / steps;

	outColor = vec4(1.0, 1.0, 1.0, 1.0);

	vec2 z = Position;
	for (int i = 0; i < steps; i++)
	{
		if (abs_squared(z) > 4.0)
		{
			float grade = 1.0 - log(log(sqrt(abs_squared(z))/log(2.0)));
			outColor = vec4(grade, grade, grade, 1.0);
			break;
		}

		z = complex_square(z) + Position;
	}
}
