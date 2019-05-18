#include "Tools/Skybox.h"
#include "GL/glew.h"


Skybox::Skybox() : enable(true)
{
	std::vector<float> databox = {
		//rear face  
		-100.0f, -100.0f, -100.0f, 0.0f    ,  0.0f,
		-100.0f,  100.0f, -100.0f, 0.0f    ,  1.0f,
		100.0f, -100.0f, -100.0f,  1.0f    ,  0.0f,
		100.0f,  100.0f, -100.0f,  1.0f    ,  1.0f,
		//front face
		-100.0f, -100.0f,  100.0f, 1.0f    ,  0.0f,
		-100.0f,  100.0f,  100.0f, 1.0f    ,  1.0f,
		100.0f, -100.0f,  100.0f, 0.0f    ,  0.0f,
		100.0f,  100.0f,  100.0f, 0.0f    ,  1.0f,
		//bottom 
		-100.0f, -100.0f,  100.0f, 1.0f    ,  0.0f,
		100.0f, -100.0f,  100.0f, 1.0f    ,  1.0f,
		-100.0f, -100.0f , -100.0f, 0.0f    ,  0.0f,
		100.0f, -100.0f , -100.0f, 0.0f    ,  1.0f,
		//top
		-100.0f,  100.0f,  100.0f, 1.0f    ,  1.0f,
		100.0f ,  100.0f,  100.0f, 1.0f    ,  0.0f,
		-100.0f,  100.0f, -100.0f, 0.0f    ,  1.0f,
		100.0f,  100.0f, -100.0f, 0.0f    ,  0.0f,
		//left
		-100.0f, -100.0f,  100.0f,  0.0f    ,  0.0f,
		-100.0f,  100.0f,  100.0f,  0.0f    ,  1.0f,
		-100.0f, -100.0f, -100.0f, 1.0f    ,  0.0f,
		-100.0f,  100.0f, -100.0f, 1.0f    ,  1.0f,
		//right
		100.0f, -100.0f,  100.0f, 1.0f    ,  0.0f,
		100.0f,  100.0f,  100.0f, 1.0f    ,  1.0f,
		100.0f, -100.0f, -100.0f, 0.0f    ,  0.0f,
		100.0f,  100.0f, -100.0f, 0.0f    ,  1.0f
	};
	m_VBO = new VertexBuffer(databox.data(), (uint)databox.size() * sizeof(float));
	VertexBufferLayout boxlayout;
	boxlayout.Push<float>(3);
	boxlayout.Push<float>(2);
	m_VAO = new VertexArray();
	m_VAO->AddBuffer(*m_VBO, boxlayout);
	unsigned int a1[] = { 0,1,2,2,3,1 };
	unsigned int a2[] = { 4,5,6,6,7,5 };
	unsigned int a3[] = { 8,9,10,10,11,9 };
	unsigned int a4[] = { 12,13,14,14,15,13 };
	unsigned int a5[] = { 16,17,18,18,19,17 };
	unsigned int a6[] = { 20,21,22,22,23,21 };
	m1 = new IndexBuffer(a1, 6);
	m2 = new IndexBuffer(a2, 6);
	m3 = new IndexBuffer(a3, 6);
	m4 = new IndexBuffer(a4, 6);
	m5 = new IndexBuffer(a5, 6);
	m6 = new IndexBuffer(a6, 6);
	
	t1 = new Texture("Resources/Images/Skybox/back.bmp",false);
	t1->Bind(25);
	t2 = new Texture("Resources/Images/Skybox/front.bmp", false);
	t2->Bind(26);
	t3 = new Texture("Resources/Images/Skybox/down.bmp", false);
	t3->Bind(27);
	t4 = new Texture("Resources/Images/Skybox/up.bmp", false);
	t4->Bind(28);
	t5 = new Texture("Resources/Images/Skybox/left.bmp", false);
	t5->Bind(29);
	t6 = new Texture("Resources/Images/Skybox/right.bmp", false);
	t6->Bind(30);

	ChooseBox(3);
	m_Shader = new Shader("Resources/Shaders/Texture2D.shader");
	m_Shader->Unbind();
}
void Skybox::ChooseBox(int i)
{
	if (enable)
	   delete t1, t2, t3, t4, t5, t6;
	enable = true;

	switch (i)
	{
	case 1:
		t1 = new Texture("Resources/Images/Skybox/bluemoon_bk.tga", false);
		t1->Bind(25);
		t2 = new Texture("Resources/Images/Skybox/bluemoon_ft.tga", false);
		t2->Bind(26);
		t3 = new Texture("Resources/Images/Skybox/bluemoon_dn.tga", false);
		t3->Bind(27);
		t4 = new Texture("Resources/Images/Skybox/bluemoon_up.tga", false);
		t4->Bind(28);
		t5 = new Texture("Resources/Images/Skybox/bluemoon_lf.tga", false);
		t5->Bind(29);
		t6 = new Texture("Resources/Images/Skybox/bluemoon_rt.tga", false);
		t6->Bind(30); break;
	case 0:
		t1 = new Texture("Resources/Images/Skybox/back.bmp", false);
		t1->Bind(25);
		t2 = new Texture("Resources/Images/Skybox/front.bmp", false);
		t2->Bind(26);
		t3 = new Texture("Resources/Images/Skybox/down.bmp", false);
		t3->Bind(27);
		t4 = new Texture("Resources/Images/Skybox/up.bmp", false);
		t4->Bind(28);
		t5 = new Texture("Resources/Images/Skybox/left.bmp", false);
		t5->Bind(29);
		t6 = new Texture("Resources/Images/Skybox/right.bmp", false);
		t6->Bind(30); break;
	case 2:
		t1 = new Texture("Resources/Images/Skybox/bloody-marvelous_bk.tga", false);
		t1->Bind(25);
		t2 = new Texture("Resources/Images/Skybox/bloody-marvelous_ft.tga", false);
		t2->Bind(26);
		t3 = new Texture("Resources/Images/Skybox/bloody-marvelous_dn.tga", false);
		t3->Bind(27);
		t4 = new Texture("Resources/Images/Skybox/bloody-marvelous_up.tga", false);
		t4->Bind(28);
		t5 = new Texture("Resources/Images/Skybox/bloody-marvelous_lf.tga", false);
		t5->Bind(29);
		t6 = new Texture("Resources/Images/Skybox/bloody-marvelous_rt.tga",false);
		t6->Bind(30); break;
	case 3:
		enable = false; break;
	case 4:
		t1 = new Texture("Resources/Images/Skybox/hills_bk.tga", false);
		t1->Bind(25);
		t2 = new Texture("Resources/Images/Skybox/hills_ft.tga", false);
		t2->Bind(26);
		t3 = new Texture("Resources/Images/Skybox/hills_dn.tga", false);
		t3->Bind(27);
		t4 = new Texture("Resources/Images/Skybox/hills_up.tga", false);
		t4->Bind(28);
		t5 = new Texture("Resources/Images/Skybox/hills_lf.tga", false);
		t5->Bind(29);
		t6 = new Texture("Resources/Images/Skybox/hills_rt.tga", false);
		t6->Bind(30); break;
	case 5:
		t1 = new Texture("Resources/Images/Skybox/desert_night_bk.tga", false);
		t1->Bind(25);
		t2 = new Texture("Resources/Images/Skybox/desert_night_ft.tga", false);
		t2->Bind(26);
		t3 = new Texture("Resources/Images/Skybox/desert_night_dn.tga", false);
		t3->Bind(27);
		t4 = new Texture("Resources/Images/Skybox/desert_night_up.tga", false);
		t4->Bind(28);
		t5 = new Texture("Resources/Images/Skybox/desert_night_lf.tga", false);
		t5->Bind(29);
		t6 = new Texture("Resources/Images/Skybox/desert_night_rt.tga", false);
		t6->Bind(30); break;
	}
}
void Skybox::Render(glm::mat4& mvp) const
{
	if (enable)
	{
		m_Shader->Bind();

		t1->Bind(25);
		t2->Bind(26);
		t3->Bind(27);
		t4->Bind(28);
		t5->Bind(29);
		t6->Bind(30);
		m_Shader->SetUniformMat4f("u_MVP", mvp);
		m_Shader->SetUniform1i("u_Texture", 25);
		renderer.DrawTriangles(*m_VAO, *m1, *m_Shader);
		m_Shader->SetUniform1i("u_Texture", 26);
		renderer.DrawTriangles(*m_VAO, *m2, *m_Shader);
		m_Shader->SetUniform1i("u_Texture", 27);
		renderer.DrawTriangles(*m_VAO, *m3, *m_Shader);
		m_Shader->SetUniform1i("u_Texture", 28);
		renderer.DrawTriangles(*m_VAO, *m4, *m_Shader);
		m_Shader->SetUniform1i("u_Texture", 29);
		renderer.DrawTriangles(*m_VAO, *m5, *m_Shader);
		m_Shader->SetUniform1i("u_Texture", 30);
		renderer.DrawTriangles(*m_VAO, *m6, *m_Shader);
	}

}
Skybox::~Skybox()
{
	delete m_VAO, m1, m2, m3, m4, m5, m6, m_Shader, t1, t2, t3, t4, t5, t6,m_VBO;
}