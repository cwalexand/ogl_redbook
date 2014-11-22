#include <GL/glew.h>
#include <fstream>
#include "VBObject.h"


VBObject::VBObject(void)
    : m_vao(0),
      m_attribute_buffer(0),
      m_index_buffer(0),
      m_header(0),
      m_attrib(0),
      m_frame(0)
{
}


VBObject::~VBObject(void)
{
    Free();
}


unsigned int VBObject::GetVertexCount(unsigned int frame)
{
    return frame < m_header->num_frames ? m_frame[frame].count : 0;
}

unsigned int VBObject::GetAttributeCount(void) const
{
    return m_header->num_attribs;
}

const char * VBObject::GetAttributeName(unsigned int index) const
{
    return index < m_header->num_attribs ? m_attrib[index].name : 0;
}

void VBObject::BindVertexArray()
{
    glBindVertexArray(m_vao);
}

bool VBObject::LoadFromVBM(const char * filename, int vertexIndex, int normalIndex, int texCoord0Index)
{
    std::ifstream f(filename, std::ios::binary);

    if (!f) return false;

    m_header = new VBM_HEADER;
    f.read((char *)m_header, sizeof(VBM_HEADER));

    m_attrib = new VBM_ATTRIB_HEADER[m_header->num_attribs];
    f.read((char *)m_attrib, m_header->num_attribs * sizeof(VBM_ATTRIB_HEADER));

    m_frame = new VBM_FRAME_HEADER[m_header->num_frames];
    f.read((char *)m_frame, m_header->num_frames * sizeof(VBM_FRAME_HEADER));

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);
    glGenBuffers(1, &m_attribute_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_attribute_buffer);

    unsigned int total_data_size = 0;
    for (unsigned int i = 0; i < m_header->num_attribs; ++i) 
    {
        int attribIndex = i;

        if(attribIndex == 0)
            attribIndex = vertexIndex;
        else if(attribIndex == 1)
            attribIndex = normalIndex;
         else if(attribIndex == 2)
            attribIndex = texCoord0Index;

        glVertexAttribPointer(attribIndex, m_attrib[i].components, m_attrib[i].type, GL_FALSE, 0, (GLvoid *)total_data_size);
        glEnableVertexAttribArray(attribIndex);
        total_data_size += m_attrib[i].components * sizeof(GLfloat) * m_header->num_vertices;
    }

    glBufferData(GL_ARRAY_BUFFER, total_data_size, NULL, GL_STATIC_DRAW);
    char *buf = (char *)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    f.read(buf, total_data_size);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    if (m_header->num_indices) 
    {
        glGenBuffers(1, &m_index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer);
        unsigned int element_size;
        switch (m_header->index_type) 
        {
            case GL_UNSIGNED_SHORT:
                element_size = sizeof(GLushort);
                break;
            default:
                element_size = sizeof(GLuint);
                break;
        }

        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_header->num_indices * element_size, NULL, GL_STATIC_DRAW);
        buf = (char *)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
        f.read(buf, m_header->num_indices * element_size);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
    }

    f.close();
    glBindVertexArray(0);

    return true;
}

bool VBObject::Free(void)
{
    glDeleteBuffers(1, &m_index_buffer);
    m_index_buffer = 0;
    glDeleteBuffers(1, &m_attribute_buffer);
    m_attribute_buffer = 0;
    glDeleteVertexArrays(1, &m_vao);
    m_vao = 0;

    delete m_header;
    m_header = NULL;

    delete [] m_attrib;
    m_attrib = NULL;

    delete [] m_frame;
    m_frame = NULL;

    return true;
}

void VBObject::Render(unsigned int frame_index, unsigned int instances)
{
    if (frame_index >= m_header->num_frames)
        return;

    glBindVertexArray(m_vao);
    if (instances) {
        if (m_header->num_indices)
            glDrawElementsInstanced(GL_TRIANGLES, 
                                    m_frame[frame_index].count, 
                                    GL_UNSIGNED_INT, 
                                    (GLvoid *)(m_frame[frame_index].first * sizeof(GLuint)), 
                                    instances);
        else
            glDrawArraysInstanced(GL_TRIANGLES, 
                                  m_frame[frame_index].first, 
                                  m_frame[frame_index].count, 
                                  instances);
    } else {
        if (m_header->num_indices)
            glDrawElements(GL_TRIANGLES, 
                           m_frame[frame_index].count, 
                           GL_UNSIGNED_INT, 
                           (GLvoid *)(m_frame[frame_index].first * sizeof(GLuint)));
        else
            glDrawArrays(GL_TRIANGLES, 
                         m_frame[frame_index].first, 
                         m_frame[frame_index].count);
    }
    glBindVertexArray(0);
}
