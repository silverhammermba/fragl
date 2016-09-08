#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using std::cerr;
using std::endl;

char* read_file(const std::string& filename)
{
	std::ifstream in(filename);
	if (!in)
	{
		// TODO throw something
	}

	in.seekg(0, std::ios::end);

	int size = in.tellg();

	char* contents = new char[size + 1];
	contents[size] = '\0';

	in.seekg(0, std::ios::beg);
	in.read(contents, size);
	in.close();

	return contents;
}

int main()
{
	if (!glfwInit())
	{
		cerr << "Failed to initialize GLFW\n";
		return 1;
	}

	// specify non-deprecated OpenGL >= 3.1
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	unsigned int width = 1600;
	unsigned int height = 900;

	// create window
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", glfwGetPrimaryMonitor(), nullptr);

	if (window == nullptr)
	{
		cerr << "Failed to create window\n";
		return 1;
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cerr << "Failed to initialize GLEW\n";
		return 1;
	}

	float surface[] = {
		-1.f,  1.f,
		 1.f,  1.f,
		 1.f, -1.f,
		-1.f, -1.f
	};

	GLshort elements[] = {
		0, 1, 2,
		0, 2, 3
	};

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(surface), surface, GL_STATIC_DRAW);

	GLuint ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);

	// create program
	GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	char* vertex_source = read_file("vertex.glsl");
	glShaderSource(vertex_shader, 1, &vertex_source, nullptr);
	glCompileShader(vertex_shader);
	delete[] vertex_source;

	GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	char* fragment_source = read_file("fragment.glsl");
	glShaderSource(fragment_shader, 1, &fragment_source, nullptr);
	glCompileShader(fragment_shader);
	delete[] fragment_source;

	GLuint program = glCreateProgram();
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);

	glLinkProgram(program);
	glUseProgram(program);

	GLint camera_u = glGetUniformLocation(program, "camera_transform");
	GLint steps_u = glGetUniformLocation(program, "steps");

	glm::vec3 translate(0.f, 0.f, 0.f);
	glm::vec3 xt(1.f, 0.f, 0.f);
	glm::vec3 yt(0.f, 1.f, 0.f);

	float theta = 0.f;
	float scale = 2.f;

	int steps = 40;

	GLint position = glGetAttribLocation(program, "position");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(position);
	glVertexAttribPointer(position, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	double elapsed = 0.0;
	float diff = 0.f;
	double last = 0.0;

	// input/display loop
	while (!glfwWindowShouldClose(window))
	{
		elapsed = glfwGetTime();
		diff = elapsed - last;
		last = elapsed;

		glm::vec3 delta(0.f, 0.f, 0.f);

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			delta += yt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			delta -= yt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			delta += xt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			delta -= xt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			theta += 45 * diff;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			theta -= 45 * diff;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			scale /= (1 + diff);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			scale *= (1 + diff);
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			steps += 1;
		else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			if (steps > 1)
				steps -= 1;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cerr << (1/scale) << endl;

		// TODO refactor the shit out of this
		translate += glm::vec3((glm::rotate(glm::mat4(), theta, glm::vec3(0.f, 0.f, 1.f)) * glm::vec4(delta, 1.f)));

		glm::mat4 transform;
		transform = glm::translate(transform, translate);
		transform = glm::rotate(transform, theta, glm::vec3(0.f, 0.f, 1.f));
		transform = glm::scale(transform, glm::vec3(scale, scale, 1.f));

		// correct for aspect ratio
		transform = glm::scale(transform, glm::vec3(width / (float)height, 1.f, 1.f));

		glUniformMatrix4fv(camera_u, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform1i(steps_u, steps);

		// clear screen
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	// cleanup GLFW
	glfwTerminate();

	return 0;
}
