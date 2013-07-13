#version 140

in vec4 Position;

out vec4 outColor;

uniform int steps;

// convert float to simulated double
vec2 dubl(float a)
{
	return vec2(a, 0.0);
}

// add simulated doubles
vec2 dadd(vec2 a, vec2 b)
{
	vec2 c;
	float t1, t2, e;

	t1 = a.x + b.x;
	e = t1 - a.x;
	t2 = ((b.x - e) + (a.x - (t1 - e))) + a.y + b.y;

	c.x = t1 + t2;
	c.y = t2 - (c.x - t1);
	return c;
}

// multiply simulated doubles
vec2 dmul(vec2 a, vec2 b)
{
	vec2 c;
	float c11, c21, c2, e, t1, t2;
	float a1, a2, b1, b2, cona, conb, split = 8193.;

	cona = a.x * split;
	conb = b.x * split;
	a1 = cona - (cona - a.x);
	b1 = conb - (conb - b.x);
	a2 = a.x - a1;
	b2 = b.x - b1;

	c11 = a.x * b.x;
	c21 = a2 * b2 + (a2 * b1 + (a1 * b2 + (a1 * b1 - c11)));

	c2 = a.x * b.y + a.y * b.x;

	t1 = c11 + c2;
	e = t1 - c11;
	t2 = a.y * b.y + ((c2 - e) + (c11 - (t1 - e))) + c21;

	c.x = t1 + t2;
	c.y = t2 - (c.x - t1);

	return c;
}

vec4 complex_square(vec4 ab)
{
	return vec4(dadd(dmul(ab.xy, ab.xy), dmul(dmul(ab.zw, ab.zw), dubl(-1.0))), dmul(dmul(ab.xy, ab.zw), dubl(2.0)));
}

vec2 abs_squared(vec4 v)
{
	return dadd(dmul(v.xy, v.xy), dmul(v.zw, v.zw));
}

bool gt(vec2 a, vec2 b)
{
	if (a.x > b.x)
		return true;
	else if (a.x == b.x)
		return a.y > b.y;
	else
		return false;
}

void main()
{
	float grade = 1.0 / steps;

	outColor = vec4(1.0, 1.0, 1.0, 1.0);

	vec4 z = Position;
	vec4 temp;
	for (int i = 0; i < steps; i++)
	{
		if (gt(abs_squared(z), dubl(4.0)))
		{
			outColor = vec4(grade * i, grade * i, grade * i, 1.0);
			break;
		}

		temp = complex_square(z);
		z = vec4(dadd(temp.xy, Position.xy), dadd(temp.zw, Position.zw));
	}
}
