#include "Renderer.h"
std::vector<uint32_t> Renderer::TextureSlots; 
std::unordered_map<uint32_t, uint32_t> Renderer::TextureSlotLookup;

Renderer::Renderer() {
	//have to initialize first
}
Renderer::Renderer(uint32_t numBatches, uint32_t maximumTextures) {
	Init(numBatches, maximumTextures);
}
Renderer::~Renderer() {
}

void Renderer::Init(uint32_t num_batches, uint32_t maximumTextures) {
	maxTextures = maximumTextures;  // Use this to set the class-level maxTextures
	m_RendererData = new RendererData[num_batches];
	numBatches = num_batches;
	Renderer::TextureSlots.resize(maxTextures);
	for (unsigned int i = 0; i < num_batches; i++) {
		m_RendererData[i].QuadBuffer = new Vertex[MaxVertexCount];
		glCreateVertexArrays(1, &m_RendererData[i].QuadVA);
		glBindVertexArray(m_RendererData[i].QuadVA);

		glCreateBuffers(1, &m_RendererData[i].QuadVB);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererData[i].QuadVB);
		glBufferData(GL_ARRAY_BUFFER, MaxVertexCount * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

		glEnableVertexArrayAttrib(m_RendererData[i].QuadVA, 0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Vertex::Position));

		glEnableVertexArrayAttrib(m_RendererData[i].QuadVA, 1);
		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Vertex::Color));

		glEnableVertexArrayAttrib(m_RendererData[i].QuadVA, 2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Vertex::TexCoords));

		glEnableVertexArrayAttrib(m_RendererData[i].QuadVA, 3);
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Vertex::TexIndex));

		uint32_t indices[MaxIndexCount];
		uint32_t offset = 0;
		for (int i = 0; i < MaxIndexCount; i += 6) {
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		glCreateBuffers(1, &m_RendererData[i].QuadIB);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererData[i].QuadIB);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//1x1 white texture
		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererData[i].WhiteTexture);
		glBindTexture(GL_TEXTURE_2D, m_RendererData[i].WhiteTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		uint32_t color = 0xffffffff;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);

		Renderer::TextureSlots[0] = m_RendererData[i].WhiteTexture;
		for (size_t j = 1; j < maxTextures; j++)  // maxTextures should now be the correct value
			Renderer::TextureSlots[j] = 0;
	}
}
void Renderer::Shutdown() {
	for (int i = 0; i < numBatches; i++) {
		glDeleteVertexArrays(1, &m_RendererData[i].QuadVA);
		glDeleteBuffers(1, &m_RendererData[i].QuadVB);
		glDeleteBuffers(1, &m_RendererData[i].QuadIB);
		glDeleteTextures(1, &m_RendererData[i].WhiteTexture);
		delete[] m_RendererData[i].QuadBuffer;
	}
	delete[] m_RendererData;
}

void Renderer::BeginBatch(uint32_t batchID) {
		m_RendererData[batchID].QuadBufferPtr = m_RendererData[batchID].QuadBuffer;
}

void Renderer::EndBatch(uint32_t batchID) {
	GLsizeiptr size = (uint8_t*)m_RendererData[batchID].QuadBufferPtr - (uint8_t*)m_RendererData[batchID].QuadBuffer;
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererData[batchID].QuadVB);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, m_RendererData[batchID].QuadBuffer);

}

void Renderer::Flush(uint32_t batchID) {
	// Ensure lastBoundTextures has the correct size for the current batch
	static std::vector<uint32_t> lastBoundTextures;

	// Resize the vector to the correct number of texture slots if needed
	if (lastBoundTextures.size() < m_RendererData[batchID].TextureSlotIndex) {
		lastBoundTextures.resize(m_RendererData[batchID].TextureSlotIndex, 0);
	}

	// Bind textures to appropriate texture units only if they are not already bound
	for (uint32_t i = 0; i < m_RendererData[batchID].TextureSlotIndex; i++) {
		uint32_t texture = Renderer::TextureSlots[i];

		if (lastBoundTextures[i] != texture) {
			glBindTextureUnit(i, texture);  // Bind the new texture
			lastBoundTextures[i] = texture; // Update the last bound texture
		}
	}

	// Bind the shader and draw
	m_RendererData[batchID].shader->Bind();
	glBindVertexArray(m_RendererData[batchID].QuadVA);
	glDrawElements(GL_TRIANGLES, m_RendererData[batchID].IndexCount, GL_UNSIGNED_INT, nullptr);
	m_RendererData[batchID].RenderStats.DrawCount++;

	// Reset batch stats and texture slots for the next batch
	m_RendererData[batchID].IndexCount = 0;
	m_RendererData[batchID].TextureSlotIndex = 1;

	// Unbind shader (optional but recommended for state management)
	m_RendererData[batchID].shader->Unbind();

	// Unbind all textures by setting texture units to 0 only if needed
	for (uint32_t i = 0; i < m_RendererData[batchID].TextureSlotIndex; i++) {
		if (lastBoundTextures[i] != 0) {
			glBindTextureUnit(i, 0);  // Unbind the texture unit by setting it to 0
			lastBoundTextures[i] = 0; // Reset the last bound texture
		}
	}
}
void Renderer::SetShader(uint32_t batchID, GLShader* batchShader) {
	m_RendererData[batchID].shader = batchShader;
}

void Renderer::DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
	if ((m_RendererData[batchID].IndexCount >= MaxIndexCount) ||
		(m_RendererData[batchID].RenderStats.QuadCount >= MaxQuadCount)) {
		EndBatch(batchID);
		Flush(batchID);
		BeginBatch(batchID);
	}

	float textureIndex = 0.0f;

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x, position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f,0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x, position.y + size.y , 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].IndexCount += 6;
	m_RendererData[batchID].RenderStats.QuadCount++;
}

void Renderer::DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, uint32_t textureID) {
	if (m_RendererData[batchID].IndexCount >= MaxIndexCount ||
		(m_RendererData[batchID].RenderStats.QuadCount >= MaxQuadCount) ||
		m_RendererData[batchID].TextureSlotIndex > 31) {
		EndBatch(batchID);
		Flush(batchID);
		BeginBatch(batchID);
	}
	constexpr glm::vec4 color = { 1.0f,1.0f,1.0f,1.0f };

	float textureIndex = 0.0f;
	auto it = Renderer::TextureSlotLookup.find(textureID);
	if (it != Renderer::TextureSlotLookup.end()) {
		textureIndex = (float)it->second;  // Access the value of the map (the texture index)
	}
	else {
		// If not found, allocate a new slot for the texture
		textureIndex = (float)m_RendererData[batchID].TextureSlotIndex;
		Renderer::TextureSlots[m_RendererData[batchID].TextureSlotIndex] = textureID;
		Renderer::TextureSlotLookup[textureID] = m_RendererData[batchID].TextureSlotIndex;
		m_RendererData[batchID].TextureSlotIndex++;
	}

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x,position.y,0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x, position.y + size.y , 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = color;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].IndexCount += 6;
	m_RendererData[batchID].RenderStats.QuadCount++;
}
void Renderer::DrawQuad(uint32_t batchID, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, uint32_t textureID) {
	if (m_RendererData[batchID].IndexCount >= MaxIndexCount ||
		(m_RendererData[batchID].RenderStats.QuadCount >= MaxQuadCount) ||
		m_RendererData[batchID].TextureSlotIndex > 31) {
		EndBatch(batchID);
		Flush(batchID);
		BeginBatch(batchID);
	}

	float textureIndex = 0.0f;
	auto it = Renderer::TextureSlotLookup.find(textureID);
	if (it != Renderer::TextureSlotLookup.end()) {
		textureIndex = (float)it->second;  // Access the value of the map (the texture index)
	}
	else {
		// If not found, allocate a new slot for the texture
		textureIndex = (float)m_RendererData[batchID].TextureSlotIndex;
		Renderer::TextureSlots[m_RendererData[batchID].TextureSlotIndex] = textureID;
		Renderer::TextureSlotLookup[textureID] = m_RendererData[batchID].TextureSlotIndex;
		m_RendererData[batchID].TextureSlotIndex++;
	}

	m_RendererData[batchID].QuadBufferPtr->Position = { position.x,position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { color.x,color.y,color.z,color.r };
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { color.x,color.y,color.z,color.r };;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x + size.x, position.y + size.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { color.x,color.y,color.z,color.r };
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { position.x, position.y + size.y , 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { color.x,color.y,color.z,color.r };;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].IndexCount += 6;
	m_RendererData[batchID].RenderStats.QuadCount++;
}

void Renderer::Draw(uint32_t batchID, std::shared_ptr<RenderableObject> obj) {
	if (m_RendererData[batchID].IndexCount >= MaxIndexCount ||
		(m_RendererData[batchID].RenderStats.QuadCount >= MaxQuadCount) ||
		m_RendererData[batchID].TextureSlotIndex > 31) {
		EndBatch(batchID);
		Flush(batchID);
		BeginBatch(batchID);
	}

	float textureIndex = 0.0f;
	auto it = Renderer::TextureSlotLookup.find(obj->textureID);
	if (it != Renderer::TextureSlotLookup.end()) {
		textureIndex = (float)it->second;  // Access the value of the map (the texture index)
	}
	else {
		// If not found, allocate a new slot for the texture
		textureIndex = (float)m_RendererData[batchID].TextureSlotIndex;
		Renderer::TextureSlots[m_RendererData[batchID].TextureSlotIndex] = obj->textureID;
		Renderer::TextureSlotLookup[obj->textureID] = m_RendererData[batchID].TextureSlotIndex;
		m_RendererData[batchID].TextureSlotIndex++;
	}

	m_RendererData[batchID].QuadBufferPtr->Position = { obj->position.x,obj->position.y,0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { obj->color.x,obj->color.y,obj->color.z,obj->color.r };
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { obj->position.x + obj->size.x, obj->position.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { obj->color.x,obj->color.y,obj->color.z,obj->color.r };;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,0.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { obj->position.x + obj->size.x, obj->position.y + obj->size.y, 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { obj->color.x,obj->color.y,obj->color.z,obj->color.r };;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 1.0f,1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;
	m_RendererData[batchID].QuadBufferPtr->Position = { obj->position.x, obj->position.y + obj->size.y , 0.0f };
	m_RendererData[batchID].QuadBufferPtr->Color = { obj->color.x,obj->color.y,obj->color.z,obj->color.r };;
	m_RendererData[batchID].QuadBufferPtr->TexCoords = { 0.0f, 1.0f };
	m_RendererData[batchID].QuadBufferPtr->TexIndex = textureIndex;
	m_RendererData[batchID].QuadBufferPtr++;

	m_RendererData[batchID].IndexCount += 6;
	m_RendererData[batchID].RenderStats.QuadCount++;
}


const Renderer::Stats& Renderer::GetStats(uint32_t batchID) {
	return m_RendererData[batchID].RenderStats;
}

void Renderer::ResetStats(uint32_t batchID) {
	memset(&m_RendererData[batchID].RenderStats, 0, sizeof(Stats));
}
