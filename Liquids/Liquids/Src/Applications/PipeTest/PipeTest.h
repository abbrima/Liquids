#pragma once
#include "Applications/Application.h"
#include "imgui/imgui.h"

#include "OpenGL/IndexBuffer.h"
#include "OpenGL/VertexArray.h"
#include "OpenGL/VertexBuffer.h"
#include "OpenGL/VertexBufferLayout.h"
#include "OpenGL/Shader.h"
#include "OpenGL/Renderer.h"
#include "OpenGL/SSBO.h"
#include "Applications/Liquids/Particle.h"
#define DISPATCHSIZE 128
#define MAX_PARTICLES 100000