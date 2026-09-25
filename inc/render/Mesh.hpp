#pragma once

#include "math/math.hpp"
#include "render/Shader.hpp"

#include <GL/gl.h>
#include <vector>

namespace mbl { namespace render {

/// Raw vertex data (VAO/VBO) with a user-defined vertex layout. Fill with add_vertex_data()/
/// add_vertex_layout(), call upload() once, then draw() each frame.
class   Mesh
{
    public:
        /// One glVertexAttribPointer binding, describing one field of the vertex struct.
        struct	VertexLayout
		{
			// Location (layout (location = X))
			u32	location;

			// Number of .type in the layout (3 for a vec3f)
			u32	count;
			// Type of the layout (GL_FLOAT for a vec3f)
			GLenum		type;

			// Offset of layout in struct
			u64	offset;
		};
    public:
        Mesh() {}
        ~Mesh()
        {
        	clear();
        }
        void	clear()
        {
	        if (_VAO != 0)
	        {
					glDeleteVertexArrays(1, &_VAO);
	        	_VAO = 0;
	        }
			if (_VBO != 0)
			{
				glDeleteBuffers(1, &_VBO);
				_VBO = 0;
			}
			_mesh_bytes.clear();
			_vertex_layouts.clear();
        }

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

		/// Appends raw vertex bytes; set_sizeof_layout() must be called first (used to count vertices).
		void	add_vertex_data(u8 *bytes, u64 size)
		{
            _vertices += size / _sizeof_layout;
			_mesh_bytes.insert(_mesh_bytes.end(), bytes, bytes + size);
		}
		/// Declares one vertex attribute (mirrors a `layout(location = ...)` in the vertex shader).
		void	add_vertex_layout(u32 location, u32 count, GLenum type, u32 offset)
		{
			_vertex_layouts.push_back(VertexLayout{.location = location, .count = count, .type = type, .offset = offset});
		}
		/// Sets the byte size of one vertex (stride).
		void	set_sizeof_layout(u64 size)
		{
			_sizeof_layout = size;
		}

		/// Uploads the accumulated vertex data and layout to the GPU.
		void	upload()
		{
			if (_VAO != 0)
				glDeleteVertexArrays(1, &_VAO);
			if (_VBO != 0)
				glDeleteBuffers(1, &_VBO);

			glGenVertexArrays(1, &_VAO);
			glGenBuffers(1, &_VBO);

			glBindVertexArray(_VAO);

			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferData(GL_ARRAY_BUFFER, _mesh_bytes.size(), _mesh_bytes.data(), GL_STATIC_DRAW);

			for (auto &v : _vertex_layouts)
			{
				glVertexAttribPointer(v.location, v.count, v.type, GL_FALSE, _sizeof_layout, (void*)v.offset);
				glEnableVertexAttribArray(v.location);
			}

			glBindVertexArray(0);
		}
		/// Draws the mesh with the given primitive mode; returns the vertex count.
		u64	draw(GLenum mode)
		{
			glBindVertexArray(_VAO);
			glDrawArrays(mode, 0, _vertices);
			glBindVertexArray(0);
			return (_vertices);
		}
		u64	vertices() {return (_vertices);}
    private:
    	u32				_VAO = 0;
		u32				_VBO = 0;

		std::vector<u8>		_mesh_bytes;
		uint64_t			_vertices = 0;

		u32					        _sizeof_layout = 0;
		std::vector<VertexLayout>	_vertex_layouts;
};

}}
