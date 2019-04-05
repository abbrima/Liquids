#pragma once

#include <functional>
#include <vector>
namespace app {

	class Application
	{
	public:
		Application() {}
		virtual ~Application() {}
		virtual void OnUpdate() {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
	};

	class ApplicationMenu : public Application
	{
	public:
		ApplicationMenu(Application*& currentApplicationPointer);
		void OnImGuiRender() override;
		template<typename T> 
		void RegisterApplication(const std::string& name)
		{
			std::cout << "Registering Application: " << name << std::endl;
			m_Applications.push_back(std::make_pair(name, []() {return new T(); }));
		}
	private:
		Application*& m_CurrentApplication;
		std::vector < std::pair <std::string, std::function <Application*()> > > m_Applications;
	};
}