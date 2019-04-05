#pragma once
#include "Renderer.h"

class Texture 
{
private:
	unsigned int m_RendererID;
	std::string m_FilePath;
	std::string m_Directory;
	std::string type;
	unsigned char* m_LocalBuffer;
	int m_Width, m_Height, m_BPP;
public:
	Texture(const std::string& path, bool repeat = false);
	~Texture();

	void Bind(unsigned int slot=0) const;
	void Unbind(unsigned int slot=0) const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline std::string GetFilePath() const { return m_FilePath; }
	inline std::string GetType() const { return type; }
	inline void SetType(std::string val) { type = val; }
	inline std::string GetDirectory() const { return m_Directory; }
	inline void SetDirectory(std::string val) { m_Directory = val; }
};