/*
	JSON2SQLite v0.1
    Copyright (C) 2011  Ali Najafizadeh

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __JSON_PARSER__
#define __JSON_PARSER__

namespace mz
{
	template <class T>
	class Stack
	{
	public:
		Stack( int len )
		{
			m_i = len - 1;
			mp_stack = new T[len];
			m_len = len;
		}

		~Stack(){ if( mp_stack ) delete [] mp_stack; }

		void Pop(){ if( m_i < m_len-1 ) m_i++; }

		bool Push( T value )
		{
			if( m_i == -1 ) return false;
			mp_stack[m_i--] = value;
			return true;
		}

		bool Get( T& value )
		{
			if( m_i + 1 != m_len ) 
			{
				value = mp_stack[m_i+1];
				return true;
			}
			return false;
		}

		int size(){ return m_i - m_len + 1; }

	private:
		T*	mp_stack;
		int m_i;
		int m_len;
	};

	class JSONParser
	{
	public:
		class Node
		{
		public:
			enum NodeType
			{
				VALUE,
				LIST,
				DICT,
				ERROR,
			};

			Node( long ns, 
				  long ne, 
				  long vs, 
				  long ve, 
				  NodeType nodeType, 
				  void* file );
			~Node();

			inline NodeType GetNodeType() const{ return m_nodeType; }
			void GetNodeName( std::string& name );
			void GetNodeValue( std::string& value );

			Node* Next();
			void Debug();

		private:
			long		m_cr;		//current position
			long		m_ns;		//name start position
			long		m_ne;		//name end position
			long		m_vs;		//value start
			long		m_ve;		//value end
			NodeType	m_nodeType;	//type of the node
			void*		mp_file;	//file pointer
		};

		JSONParser( const char* filename );
		~JSONParser();
		bool Validate( int depth );
		Node* GetRootNode();

	private:
		void* mp_file;
	};

	long GetSize( long pos, char type, void* file );
}

#endif