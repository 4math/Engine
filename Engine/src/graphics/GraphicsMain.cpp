#include "GraphicsMain.h"

using namespace graphics;

void graphics::GraphicsManager::Initialize()
{
	m_initialized = true;
}

void graphics::GraphicsManager::Shutdown()
{
	m_initialized = false;
}
