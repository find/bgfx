/*
 * Copyright 2011-2012 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <string.h>
#include <bx/hash.h>
#include <bx/uint32_t.h>

#include "vertexdecl.h"

namespace bgfx
{
	static const uint8_t s_attribTypeSizeDx9[AttribType::Count][4] =
	{
		{  4,  4,  4,  4 },
		{  4,  4,  8,  8 },
		{  4,  4,  8,  8 },
		{  4,  8, 12, 16 },
	};

	static const uint8_t s_attribTypeSizeDx11[AttribType::Count][4] =
	{
		{  1,  2,  4,  4 },
		{  2,  4,  8,  8 },
		{  2,  4,  8,  8 },
		{  4,  8, 12, 16 },
	};

	static const uint8_t s_attribTypeSizeGl[AttribType::Count][4] =
	{
		{  1,  2,  4,  4 },
		{  2,  4,  6,  8 },
		{  2,  4,  6,  8 },
		{  4,  8, 12, 16 },
	};

	static const uint8_t (*s_attribTypeSize[RendererType::Count])[AttribType::Count][4] =
	{
#if BGFX_CONFIG_RENDERER_DIRECT3D9
		&s_attribTypeSizeDx9,
#elif BGFX_CONFIG_RENDERER_DIRECT3D11
		&s_attribTypeSizeDx11,
#elif BGFX_CONFIG_RENDERER_OPENGL|BGFX_CONFIG_RENDERER_OPENGLES2|BGFX_CONFIG_RENDERER_OPENGLES3
		&s_attribTypeSizeGl,
#else
		&s_attribTypeSizeDx9,
#endif // BGFX_CONFIG_RENDERER_
		&s_attribTypeSizeDx9,
		&s_attribTypeSizeDx11,
		&s_attribTypeSizeGl,
		&s_attribTypeSizeGl,
		&s_attribTypeSizeGl,
	};

	void VertexDecl::begin(RendererType::Enum _renderer)
	{
		m_hash = _renderer; // use hash to store renderer type while building VertexDecl.
		m_stride = 0;
		memset(m_attributes, 0xff, sizeof(m_attributes) );
		memset(m_offset, 0, sizeof(m_offset) );
	}

	void VertexDecl::end()
	{
		m_hash = bx::hashMurmur2A(m_attributes, sizeof(m_attributes) );
	}

	void VertexDecl::add(Attrib::Enum _attrib, uint8_t _num, AttribType::Enum _type, bool _normalized, bool _asInt)
	{
		const uint8_t encodedNorm = (_normalized&1)<<6;
		const uint8_t encodedType = (_type&3)<<3;
		const uint8_t encodedNum = (_num-1)&3;
		const uint8_t encodeAsInt = (_asInt&(!!"\x1\x1\x0\x0"[_type]) )<<7;

		m_attributes[_attrib] = encodedNorm|encodedType|encodedNum|encodeAsInt;
		m_offset[_attrib] = m_stride;
		m_stride += (*s_attribTypeSize[m_hash])[_type][_num-1];
	}

	void VertexDecl::decode(Attrib::Enum _attrib, uint8_t& _num, AttribType::Enum& _type, bool& _normalized, bool& _asInt) const
	{
		uint8_t val = m_attributes[_attrib];
		_num = (val&3)+1;
		_type = AttribType::Enum((val>>3)&3);
		_normalized = !!(val&(1<<6) );
		_asInt = !!(val&(1<<7) );
	}

	const char* getAttribName(Attrib::Enum _attr)
	{
		static const char* attrName[Attrib::Count] = 
		{
			"Attrib::Position",
			"Attrib::Normal",
			"Attrib::Color0",
			"Attrib::Color1",
			"Attrib::Indices",
			"Attrib::Weights",
			"Attrib::TexCoord0",
			"Attrib::TexCoord1",
			"Attrib::TexCoord2",
			"Attrib::TexCoord3",
			"Attrib::TexCoord4",
			"Attrib::TexCoord5",
			"Attrib::TexCoord6",
			"Attrib::TexCoord7",
		};

		return attrName[_attr];
	}

	void dump(const VertexDecl& _decl)
	{
#if BGFX_CONFIG_DEBUG
		BX_TRACE("vertexdecl %08x (%08x), stride %d"
			, _decl.m_hash
			, hashMurmur2A(_decl.m_attributes, sizeof(_decl.m_attributes) )
			, _decl.m_stride
			);

		for (uint32_t attr = 0; attr < Attrib::Count; ++attr)
		{
			if (0xff != _decl.m_attributes[attr])
			{
				uint8_t num;
				AttribType::Enum type;
				bool normalized;
				bool asInt;
				_decl.decode(Attrib::Enum(attr), num, type, normalized, asInt);

				BX_TRACE("\tattr %d - %s, num %d, type %d, norm %d, asint %d, offset %d"
					, attr
					, getAttribName(Attrib::Enum(attr) )
					, num
					, type
					, normalized
					, asInt
					, _decl.m_offset[attr]
				);
			}
		}
#else
		BX_UNUSED(_decl);
#endif // BGFX_CONFIG_DEBUG
	}

	void vertexPack(const float _input[4], bool _inputNormalized, Attrib::Enum _attr, const VertexDecl& _decl, void* _data, uint32_t _index)
	{
		if (!_decl.has(_attr) )
		{
			return;
		}

		uint32_t stride = _decl.getStride();
		uint8_t* data = (uint8_t*)_data + _index*stride + _decl.getOffset(_attr);

		uint8_t num;
		AttribType::Enum type;
		bool normalized;
		bool asInt;
		_decl.decode(_attr, num, type, normalized, asInt);

		switch (type)
		{
		default:
		case AttribType::Uint8:
			{
				uint8_t* packed = (uint8_t*)data;
				if (_inputNormalized)
				{
					if (asInt)
					{
						switch (num)
						{
						default: *packed++ = uint8_t(*_input++ * 127.0f + 128.0f);
						case 3:  *packed++ = uint8_t(*_input++ * 127.0f + 128.0f);
						case 2:  *packed++ = uint8_t(*_input++ * 127.0f + 128.0f);
						case 1:  *packed++ = uint8_t(*_input++ * 127.0f + 128.0f);
						}
					}
					else
					{
						switch (num)
						{
						default: *packed++ = uint8_t(*_input++ * 255.0f);
						case 3:  *packed++ = uint8_t(*_input++ * 255.0f);
						case 2:  *packed++ = uint8_t(*_input++ * 255.0f);
						case 1:  *packed++ = uint8_t(*_input++ * 255.0f);
						}
					}
				}
				else
				{
					switch (num)
					{
					default: *packed++ = uint8_t(*_input++);
					case 3:  *packed++ = uint8_t(*_input++);
					case 2:  *packed++ = uint8_t(*_input++);
					case 1:  *packed++ = uint8_t(*_input++);
					}
				}
			}
			break;

		case AttribType::Uint16:
			{
				uint16_t* packed = (uint16_t*)data;
				if (_inputNormalized)
				{
					if (asInt)
					{
						switch (num)
						{
						default: *packed++ = uint16_t(*_input++ * 32767.0f + 32768.0f);
						case 3:  *packed++ = uint16_t(*_input++ * 32767.0f + 32768.0f);
						case 2:  *packed++ = uint16_t(*_input++ * 32767.0f + 32768.0f);
						case 1:  *packed++ = uint16_t(*_input++ * 32767.0f + 32768.0f);
						}
					}
					else
					{
						switch (num)
						{
						default: *packed++ = uint16_t(*_input++ * 65535.0f);
						case 3:  *packed++ = uint16_t(*_input++ * 65535.0f);
						case 2:  *packed++ = uint16_t(*_input++ * 65535.0f);
						case 1:  *packed++ = uint16_t(*_input++ * 65535.0f);
						}
					}
				}
				else
				{
					switch (num)
					{
					default: *packed++ = uint16_t(*_input++);
					case 3:  *packed++ = uint16_t(*_input++);
					case 2:  *packed++ = uint16_t(*_input++);
					case 1:  *packed++ = uint16_t(*_input++);
					}
				}
			}
			break;

		case AttribType::Half:
			{
				uint16_t* packed = (uint16_t*)data;
				switch (num)
				{
				default: *packed++ = bx::halfFromFloat(*_input++);
				case 3:  *packed++ = bx::halfFromFloat(*_input++);
				case 2:  *packed++ = bx::halfFromFloat(*_input++);
				case 1:  *packed++ = bx::halfFromFloat(*_input++);
				}
			}
			break;

		case AttribType::Float:
			memcpy(data, _input, num*sizeof(float) );
			break;
		}
	}

	void vertexUnpack(float _output[4], Attrib::Enum _attr, const VertexDecl& _decl, const void* _data, uint32_t _index)
	{
		if (!_decl.has(_attr) )
		{
			memset(_output, 0, 4*sizeof(float) );
			return;
		}

		uint32_t stride = _decl.getStride();
		uint8_t* data = (uint8_t*)_data + _index*stride + _decl.getOffset(_attr);

		uint8_t num;
		AttribType::Enum type;
		bool normalized;
		bool asInt;
		_decl.decode(_attr, num, type, normalized, asInt);

		switch (type)
		{
		default:
		case AttribType::Uint8:
			{
				uint8_t* packed = (uint8_t*)data;
				if (asInt)
				{
					switch (num)
					{
					default: *_output++ = (float(*packed++) - 128.0f)*1.0f/127.0f;
					case 3:  *_output++ = (float(*packed++) - 128.0f)*1.0f/127.0f;
					case 2:  *_output++ = (float(*packed++) - 128.0f)*1.0f/127.0f;
					case 1:  *_output++ = (float(*packed++) - 128.0f)*1.0f/127.0f;
					}
				}
				else
				{
					switch (num)
					{
					default: *_output++ = float(*packed++)*1.0f/255.0f;
					case 3:  *_output++ = float(*packed++)*1.0f/255.0f;
					case 2:  *_output++ = float(*packed++)*1.0f/255.0f;
					case 1:  *_output++ = float(*packed++)*1.0f/255.0f;
					}
				}
			}
			break;

		case AttribType::Uint16:
			{
				uint16_t* packed = (uint16_t*)data;
				if (asInt)
				{
					switch (num)
					{
					default: *_output++ = (float(*packed++) - 32768.0f)*1.0f/32767.0f;
					case 3:  *_output++ = (float(*packed++) - 32768.0f)*1.0f/32767.0f;
					case 2:  *_output++ = (float(*packed++) - 32768.0f)*1.0f/32767.0f;
					case 1:  *_output++ = (float(*packed++) - 32768.0f)*1.0f/32767.0f;
					}
				}
				else
				{
					switch (num)
					{
					default: *_output++ = float(*packed++)*1.0f/65535.0f;
					case 3:  *_output++ = float(*packed++)*1.0f/65535.0f;
					case 2:  *_output++ = float(*packed++)*1.0f/65535.0f;
					case 1:  *_output++ = float(*packed++)*1.0f/65535.0f;
					}
				}
			}
			break;

		case AttribType::Half:
			{
				uint16_t* packed = (uint16_t*)data;
				switch (num)
				{
				default: *_output++ = bx::halfToFloat(*packed++);
				case 3:  *_output++ = bx::halfToFloat(*packed++);
				case 2:  *_output++ = bx::halfToFloat(*packed++);
				case 1:  *_output++ = bx::halfToFloat(*packed++);
				}
			}
			break;

		case AttribType::Float:
			memcpy(_output, data, num*sizeof(float) );
			_output += num;
			break;
		}

		switch (num)
		{
		case 1: *_output++ = 0.0f;
		case 2: *_output++ = 0.0f;
		case 3: *_output++ = 0.0f;
		default: break;
		}
	}

} // namespace bgfx