#pragma once
#include "OpenGlApp.h"

OpenGL_App::OpenGL_App() :
	window_(nullptr) {};

OpenGL_App::OpenGL_App(const unsigned int width, const unsigned int height, const unsigned int numBatches){
	Initialize(width, height,numBatches);
};

OpenGL_App::~OpenGL_App() {
	Cleanup();
}

void OpenGL_App::Initialize(const unsigned int width, const unsigned int height, const unsigned int numBatches) {
	InitializeWindow(width, height);
	InitializeGraphics(numBatches);
	SetupResources();
	SetupImGui(window_);

}

void OpenGL_App::InitializeWindow(const unsigned int scr_width, const unsigned int scr_height) {
	if (!glfwInit()) {
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// glfw window creation
	window_ = glfwCreateWindow(scr_width, scr_height, "OpenGL", NULL, NULL);
	if (window_ == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window_);
	glfwSwapInterval(1);
	screen_width = scr_width;
	screen_height = scr_height;

	// Set the OpenGL_App instance as the user pointer
	glfwSetWindowUserPointer(window_, this);

	// Set the framebuffer size callback
	glfwSetFramebufferSizeCallback(window_, fbSizeCallback);
}
void OpenGL_App::InitializeGraphics(unsigned int numBatches) {
	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		throw std::runtime_error("Failed to initialize GLAD");
	}
	std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
	GLint maxTextures;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	iRenderer::Get()->Init(numBatches, maxTextures);
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA))
}

void OpenGL_App::UpdateDpiScale(GLFWwindow* window) {
	float xscale, yscale;
	glfwGetWindowContentScale(window, &xscale, &yscale);

	// Apply the DPI scale factor to any relevant elements (like mouse position, objects, etc.)
	std::cout << "DPI Scale: " << xscale << ", " << yscale << std::endl;
}
void OpenGL_App::WindowSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	OpenGL_App* app = static_cast<OpenGL_App*>(glfwGetWindowUserPointer(window));
	if (app) {
		app->UpdateProjectionMatrix(width, height);
		app->UpdateDpiScale(window);  // Optionally adjust for DPI scale
	}
}

float OpenGL_App::getDpiScale(GLFWwindow* window) {
	float xscale, yscale;
	glfwGetFramebufferSize(window, nullptr, nullptr);  // Get window size first
	glfwGetWindowContentScale(window, &xscale, &yscale);  // Get DPI scale factor

	// The scale factors should be the same for both axes (unless you have some unusual settings)
	return xscale;  // Return one of the scale factors (xscale or yscale)
}

void OpenGL_App::SetupResources() {
	GLint maxTextures;
	view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	projection = glm::ortho(0.0f, (float)800, (float)600, 0.0f, -1.0f, 1.0f);
	transform = glm::mat4(1.0f);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextures);
	shader_ = std::make_shared<GLShader>("Resources/Shaders/Batch.shader");
	texture_ = std::make_shared<GLTexture2D>("Resources/Textures/awesomeface.png");
	textureIDs.insert({ "awesomeface.png",texture_->getID() });
	std::vector<int> samplers;
	samplers.resize(maxTextures);
	for (int i = 0; i < maxTextures; i++)
		samplers[i] = i;
	shader_->Bind();
	shader_->SetUniform1iv("u_Textures", 32, samplers.data());
	shader_->SetUniformMat4f("u_ViewProj", view * projection);
	shader_->SetUniformMat4f("u_Transform", transform);
	vRects.push_back({ { x, y }, { 30.0f, 20.0f } });
	vRects.push_back({ { 100.0f, 100.0f }, { 80.0f, 50.0f } });
	InitCallbacks();

}

void OpenGL_App::Execute() {
	double mouse_x, mouse_y;

	while (!glfwWindowShouldClose(window_)) {
		glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  // Clear both buffers

		ProcessInput();  // Handle user input
		if (enableImGui) {
			RenderImGui();
		}

		iRenderer::Get()->SetShader(0, shader_.get());
		GLShader shader("Resources/Shaders/WireFrame.shader");
		iRenderer::Get()->SetShader(1, &shader);
		shader.Bind();
		shader.SetUniformMat4f("u_ViewProj", view * projection);
		shader.SetUniformMat4f("u_Transform", transform);

		iRenderer::Get()->BeginBatch(0);
		iRenderer::Get()->BeginBatch(1);

		glfwGetCursorPos(window_, &mouse_x, &mouse_y);
		Vec2 vMouse = { float(mouse_x), float(mouse_y) };
		Rect r = { {x,y} , { 100, 100 } };
		if (Rect::PointVsRect(vMouse, r))
		{
			iRenderer::Get()->DrawQuad(0, { x, y }, { 100, 100 }, { 1.0f, 0.0f, 0.0f, 1.0f }, texture_->getID());

			iRenderer::Get()->DrawQuad(1, { x, y }, { 100, 100 }, { 1.0f, 0.0f, 0.0f, 1.0f }, texture_->getID());
		}
		else {
			iRenderer::Get()->DrawQuad(0, { x, y }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, texture_->getID());

			iRenderer::Get()->DrawQuad(1, { x, y }, { 100, 100 }, { 1.0f, 1.0f, 1.0f, 1.0f }, texture_->getID());
		}
	

		// Add the objects into the quadtree


		// If a collision is detected, change color to red




		iRenderer::Get()->EndBatch(0);
		iRenderer::Get()->EndBatch(1);
		iRenderer::Get()->Flush(1);
		iRenderer::Get()->Flush(0);

		glfwSwapBuffers(window_);
		glfwPollEvents();

		// Optional: display stats
		const auto& stats = iRenderer::Get()->GetStats(0);
		std::cout << "Draws: " << stats.DrawCount << " | Quads: " << stats.QuadCount << "\r";
		iRenderer::Get()->ResetStats(0);
	}
}
void OpenGL_App::ProcessInput() {
	if (window_ == nullptr) {
		std::cerr << "Window is null!" << std::endl;
		return;
	}
	bool isEscapePressedRecently = false;
	bool isTabPressedRecently = false;
	GLFWwindow* glfwWindow = static_cast<GLFWwindow*>(window_);

	if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(glfwWindow, true);  // Only trigger once
	}


	if (glfwGetKey(glfwWindow, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (std::chrono::steady_clock::now() - lastTabPressTime >= debounceTime) {
			// Toggle ImGui visibility on tab press
			enableImGui = !enableImGui;
			lastTabPressTime = std::chrono::steady_clock::now();  // Update the last press time
		}
	}	
}
void OpenGL_App::UpdateProjectionMatrix(int width, int height) {
	glViewport(0, 0, width, height);  // Set the new viewport size

	// Update the projection matrix based on the new window size
	projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
}

void OpenGL_App::Cleanup() {
	CleanupImGui();
}
//frambuffer size callback
void OpenGL_App::fbSizeCallback(GLFWwindow* window, int width, int height) {
	// Retrieve the OpenGL_App instance from the window user pointer
	OpenGL_App* app = static_cast<OpenGL_App*>(glfwGetWindowUserPointer(window));

	if (app) {
		app->UpdateProjectionMatrix(width, height);
	}
}

void OpenGL_App::EnableImGui() {
	enableImGui = true;
}
void OpenGL_App::DisableImGui() {
	enableImGui = false;
}
void OpenGL_App::SetupImGui(GLFWwindow* window)
{
	// Initialize ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiStyle& style = ImGui::GetStyle();
	ImGuiIO& io = ImGui::GetIO();
	ImFontConfig config;

	config.SizePixels = 20.0f;
	io.Fonts->AddFontDefault(&config);
	style.ScaleAllSizes(1);
	// Setup ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 130");

	// Additional setup (optional)
	// ...
}

void OpenGL_App::CleanupImGui()
{
	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
void OpenGL_App::RenderImGui()
{
	// Start new ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Your ImGui UI code here (e.g., windows, buttons, etc.)
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		ImGui::Begin("OpenGL Demo");
		static float f = 0.0f;

		ImGui::SliderFloat("A-X", &x, 0.0f, 700.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::SliderFloat("A-Y", &y, 0.0f, 500.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::SliderFloat("B-X", &bx, 0.0f, 700.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::SliderFloat("B-Y", &by, 0.0f, 500.0f);            // Edit 1 float using a slider from 0.0f to 1.0f

		//ImGui::SliderFloat3("Translation B", &translationB.x, 0.0f, 960.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}
	// Render ImGui frame
	ImGui::Render();
	glClearColor(imguiClearColor.x, imguiClearColor.y, imguiClearColor.z, imguiClearColor.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void OpenGL_App::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		if (button == GLFW_MOUSE_BUTTON_LEFT) {
			std::cout << "Left mouse button clicked!" << std::endl;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
			std::cout << "Right mouse button clicked!" << std::endl;
		}
	}
}

void OpenGL_App::HandleCursorPos(GLFWwindow* window, double xpos, double ypos) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// Flip Y since GLFW origin is top-left
	float x = (float)xpos;
	float y = (float)(height - ypos);  // flipped

	// Normalize to NDC [-1, 1] (adjust for window size)www
	float ndc_x = (2.0f * x) / width - 1.0f;
	float ndc_y = (2.0f * y) / height - 1.0f;

	glm::vec4 mouseNDC(ndc_x, ndc_y, 0.0f, 1.0f);

	// Use updated projection and view matrices
	glm::mat4 vp = projection * view;

	// Convert from NDC to world coordinates
	glm::vec4 worldPos = glm::inverse(vp) * mouseNDC;

	Vec2 vMouseWorld = { worldPos.x, worldPos.y };

	std::cout << "Mouse World Position: (" << vMouseWorld.x << ", " << vMouseWorld.y << ")\n";

	Rect r = { {x, y}, {100.0f, 100.0f} };
	if (Rect::PointVsRect(vMouseWorld, r)) {
		std::cout << "Mouse is inside the rectangle!" << std::endl;
	}
}


void OpenGL_App::InitCallbacks() {
	glfwSetMouseButtonCallback(window_, OpenGL_App::mouse_button_callback);
	glfwSetWindowUserPointer(window_, this);
	glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double xpos, double ypos) {
		OpenGL_App* app = static_cast<OpenGL_App*>(glfwGetWindowUserPointer(window));
		if (app) app->HandleCursorPos(window, xpos, ypos);
		});
}
