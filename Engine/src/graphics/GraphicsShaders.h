#pragma once

namespace graphics 
{
	class ShaderManager
	{
		// VARIABLES
	private:
		bool m_initialized = false;

		// CONSTRUCTORS/DESTRUCTORS
	public:
		ShaderManager() { Initialize(); }
		~ShaderManager() { Shutdown(); }

		// METHODES
	private:
		void Initialize();
		void Shutdown();

	public:

	};
}