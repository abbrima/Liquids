#pragma once

#include <functional>
#include <vector>
#include "OpenGL/Renderer.h"
namespace app {

	class Application
	{
	public:
		Application() {}
		virtual ~Application() {}
		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void FreeGuiRender() {}
	};

	class ApplicationMenu : public Application
	{
	public:
		ApplicationMenu(Application*& currentApplicationPointer);
		void OnImGuiRender() override;
		void OnRender() override;
		template<typename T> 
		void RegisterApplication(const std::string& name)
		{
			std::cout << "Registering Application: " << name << std::endl;
			m_Applications.push_back(std::make_pair(name, []() {return new T(); }));
		}
	private:
		Renderer renderer;
		Application*& m_CurrentApplication;
		std::vector < std::pair <std::string, std::function <Application*()> > > m_Applications;
	};
}