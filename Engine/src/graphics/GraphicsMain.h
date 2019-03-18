#pragma once

#include <string>;


namespace graphics
{
	constexpr int EXIT_CODE_OK = 0;
	constexpr int EXIT_CODE_FAILURE = -1;

	class GraphicsManager
	{
		// VARIABLES
	private:
		bool m_environment = false;
		bool m_renderer = false;
		bool m_creator = false;
/*TODO:
	renderer
	creator
*/
	public:


		// CONSTRUCTORS/DESTRUCTORS
	public:
		GraphicsManager() { Initialize("Debug process"); }
		GraphicsManager(std::string process_name_) { Initialize(process_name_); }
		GraphicsManager(std::string process_name_, int window_width_, int window_height_) { Initialize(process_name_, window_width_, window_height_); }
		~GraphicsManager() { Shutdow(); }

		// METHODES
	private:
		int Initialize(std::string process_name_, int window_width_ = 1280, int window_height_ = 720);
		int Shutdow();

	public:

	};
}
