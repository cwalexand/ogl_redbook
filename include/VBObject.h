#ifndef __VBOBJECT_H
#define __VBOBJECT_H

class VBObject
{
public:
    VBObject(void);
    ~VBObject(void);

    bool LoadFromVBM(const char * filename, int vertexIndex, int normalIndex, int texCoord0Index);
    void Render(unsigned int frame_index = 0, unsigned int instances = 0);
    void BindVertexArray();

    unsigned int GetVertexCount(unsigned int frame = 0);
    unsigned int GetAttributeCount(void) const;
    const char * GetAttributeName(unsigned int index) const;

private:

    bool Free(void);

    struct VBM_HEADER
    {
        unsigned int magic;
        unsigned int size;
        char name[64];
        unsigned int num_attribs;
        unsigned int num_frames;
        unsigned int num_vertices;
        unsigned int num_indices;
        unsigned int index_type;
    };

    struct VBM_ATTRIB_HEADER
    {
        char name[64];
        unsigned int type;
        unsigned int components;
        unsigned int flags;
    };

    struct VBM_FRAME_HEADER
    {
        unsigned int first;
        unsigned int count;
        unsigned int flags;
    };

    unsigned int m_vao;
    unsigned int m_attribute_buffer;
    unsigned int m_index_buffer;

    VBM_HEADER * m_header;
    VBM_ATTRIB_HEADER * m_attrib;
    VBM_FRAME_HEADER * m_frame;
};

#endif // __VBOBJECT_H
