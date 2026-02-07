#pragma once


namespace TR
{
	class Event
	{
	public:
		enum class Type
		{
			close_application,
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
