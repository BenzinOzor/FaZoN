#pragma once


namespace TR
{
	class Event
	{
	public:
		enum class Type
		{
			create_entries,		// Close the currently opened entries file and set a new one up.
			close_entries,		// Close the currently opened entries file, clearing entries and saved path to their file.
			create_project,		// Close the currently opened project and set a new one up.
			close_project,		// Close the currently opened project, clearing all entries and path to entries and enum files.
			COUNT
		};

		Event() = default;

		Event( const Type& _eType )
			: m_type( _eType )
		{}

		Type m_type = Type::COUNT;

		union
		{
			
		};
	};
}
