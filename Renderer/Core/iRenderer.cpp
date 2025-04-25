#include "iRenderer.h"
std::shared_ptr<Renderer> iRenderer::renderer = nullptr;

std::shared_ptr<Renderer> iRenderer::Get() {
	if (!renderer)
		renderer = std::make_shared<Renderer>();
	return renderer;
}


// Define and initialize the static member

