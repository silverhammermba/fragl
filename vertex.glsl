#version 140

in vec2 position;

uniform mat4 camera_transform;

out vec4 Position;

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

void main()
{
	gl_Position = vec4(position, 0.0, 1.0); // already in NDC
	Position = vec4(dadd(dadd(dmul(dubl(camera_transform[0].x), dubl(position.x)), dmul(dubl(camera_transform[1].x), dubl(position.y))), dubl(camera_transform[3].x)),
	                dadd(dadd(dmul(dubl(camera_transform[0].y), dubl(position.x)), dmul(dubl(camera_transform[1].y), dubl(position.y))), dubl(camera_transform[3].y)));
}
