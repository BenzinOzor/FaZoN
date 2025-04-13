#pragma once

#include <variant>


namespace fzn
{
	template<class... Ts>
	struct Overloaded : Ts... { using Ts::operator()...; };
	// explicit deduction guide (not needed as of C++20)
	template<class... Ts>
	Overloaded( Ts... ) -> Overloaded<Ts...>;

	template< typename ... Types >
	class Variant
	{
	public:
		Variant() = default;
		/*template <class _Ty>
		Variant( const _Ty& _obj ) : m_value( _obj ) {}
		template <class _Ty>
		Variant( _Ty& _obj ) : m_value( _obj ) {}
		template <class _Ty>
		Variant( const _Ty&& _obj ) : m_value( _obj ) {}*/
		template <class _Ty>
		Variant( _Ty&& _obj ) : m_value( _obj ) {}
		Variant( const Variant& _variant ): m_value( _variant.m_value ) {}
		Variant( Variant&& _variant ) { std::swap( m_value, _variant.m_value ); }

		template < typename Type >
		Type get()
		{
			return std::get< Type >( m_value );
		}

		template <class _Ty>
		Variant& operator=( _Ty&& _Obj )				{ return m_value.operator =( _Obj ); }
		Variant& operator=( const Variant& _variant )	{ m_value = _variant.m_value; return *this; }
		Variant& operator=( Variant& _variant )			{ m_value = _variant.m_value; return *this; }
		Variant& operator=( const Variant&& _variant )	{ std::swap( m_value, _variant.m_value ); return *this; }
		Variant& operator=( Variant&& _variant )		{ std::swap( m_value, _variant.m_value ); return *this; }

		bool operator==( const Variant& _variant ) const { return m_value == _variant.m_value; }

		template < typename Type >
		const Type get() const
		{
			return std::get< Type >( m_value );
		}

		template < typename Type >
		Type* get_if()
		{
			return std::get_if< Type >( &m_value );
		}

		template < typename Type >
		const Type* get_if() const
		{
			return std::get_if< Type >( &m_value );
		}

		template < typename Type >
		bool is() const
		{
			return std::holds_alternative< Type >( m_value );
		}

		template< typename Ret, class Visitor, Types... >
		const Ret visit( Visitor&& _visitor ) const { return std::visit( _visitor, m_value ) ; }

		template< typename Ret, class Visitor, Types... >
		Ret visit( Visitor&& _visitor ) { return std::visit( _visitor, m_value ); }

		std::variant< Types... >& get_value() { return m_value; }
		const std::variant< Types... >& get_value() const { return m_value; }

	private:
		std::variant< Types... > m_value;
	};
} // namespace fzn
