#include "Application.h"
#include "imgui/imgui.h"
namespace app
{
	ApplicationMenu::ApplicationMenu(Application*& currentApplicationPointer)
		:m_CurrentApplication(currentApplicationPointer)
	{

	}
	void ApplicationMenu::OnImGuiRender()
	{
		for (auto& app : m_Applications)
		{
			if (ImGui::Button(app.first.c_str()))
				m_CurrentApplication = app.second();
		}
	}
}