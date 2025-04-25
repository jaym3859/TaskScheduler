#pragma once
#include "Renderer.h"
class iRenderer {
public:
	iRenderer() = delete;
	iRenderer(const iRenderer& other) = delete;
	iRenderer& operator=(const iRenderer& other) = delete;
	~iRenderer() = default;

	static std::shared_ptr<Renderer> Get();
private:
	static std::shared_ptr<Renderer> renderer;
};