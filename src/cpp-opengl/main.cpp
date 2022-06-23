#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

// ���ڿ��
static constexpr int SCR_WIDTH = 800;
// ���ڸ߶�
static constexpr int SCR_HEIGHT = 600;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

//��������
static void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, true);
	}
}

int main()
{

	//��ʼ��GLFW
	glfwInit();
	//����GLFW
	//���汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//�ΰ汾��
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//ʹ�ú���ģʽ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif // __APPLE__

	//����glfw����
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "learnOpengl", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// ��ʼ��GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	while (!glfwWindowShouldClose(window))
	{
		//input
		processInput(window);
		//render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		// ��鲢�����¼�����������
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	//ɾ��֮ǰ�ķ����������Դ
	glfwTerminate();
	return 0;
}