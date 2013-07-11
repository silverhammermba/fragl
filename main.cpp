#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../moogl/moogl.hpp"

using std::cerr;
using std::endl;

std::string read_file(const std::string& filename)
{
	std::string contents;

	std::ifstream in(filename);
	if (!in)
	{
		// TODO throw something
	}

	in.seekg(0, std::ios::end);
	contents.resize(in.tellg());
	in.seekg(0, std::ios::beg);
	in.read(&contents[0], contents.size());
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

	unsigned int width = 800;
	unsigned int height = 600;

	// create window
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL", nullptr, nullptr);

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
	GL::Program program {
		GL::Shader(GL::Shader::Vertex, read_file("vertex.glsl")),
		GL::Shader(GL::Shader::Fragment, read_file("fragment.glsl"))
	};

	program.use();

	GL::Uniform camera_u(program, "camera_transform");
	GL::Uniform perspective_u(program, "perspective_transform");

	glm::vec3 translate(0.f, 0.f, 0.f);
	glm::vec3 xt(1.f, 0.f, 0.f);
	glm::vec3 yt(0.f, 1.f, 0.f);

	float theta = 0.f;
	float scale = 1.f;

	glm::mat4 perspective;// = glm::perspective(90.0f, width / (float)height, 1.0f, 45.0f);
	perspective_u.set(perspective);

	GL::Attribute position(program, "position");

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	position.enable();
	position.layout<float>(2, false, 0, 0);

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

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			translate += yt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			translate -= yt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
			translate += xt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
			translate -= xt * diff * scale;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			theta += 45 * diff;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			theta -= 45 * diff;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			scale /= (1 + diff);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			scale *= (1 + diff);

		glm::mat4 transform;
		transform = glm::rotate(transform, theta, glm::vec3(0.f, 0.f, 1.f));
		transform = glm::translate(transform, translate);
		transform = glm::scale(transform, glm::vec3(scale, scale, 1.f));
		camera_u.set(transform);

		// clear screen
		GL::clear_color();

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
