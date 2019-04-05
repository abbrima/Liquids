#include "Texture.h"
#include "stb_image/stb_image.h"
#include <iostream>
#include <algorithm>
Texture::Texture(const std::string& path,bool repeat)
	:m_RendererID(0),m_FilePath(path), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0)
{
	for (unsigned int i = 0; i < m_FilePath.length(); i++)
	{
		if (m_FilePath[i] == (char)92)
			m_FilePath[i] = '/';
	}
	std::cout << m_FilePath << std::endl;
	GLCall(glActiveTexture(GL_TEXTURE31));
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	if (m_Width == 0 && m_Height == 0 && m_BPP == 0)
		std::cout << "ERROR: Failed to load texture: " << m_FilePath << std::endl;

	GLCall(glGenTextures(1, &m_RendererID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	if (!repeat)
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	}
	else
	{
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
	}
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8 , m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glActiveTexture(GL_TEXTURE0));
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
}


Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}
void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}
void Texture::Unbind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}