#include "CVS_Mesh.h"

//TODO: remove this include
#include "CVS_Bone.h"

void CVS_Mesh::Init()
{
	/*
	GLuint vertexBuffer, boneBuffer, indexBuffer;
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	printf("mesh initializing\n");

	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &boneBuffer);
	glGenBuffers(1, &indexBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(AttribIndex::POSITION);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

	glEnableVertexAttribArray(AttribIndex::UV);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(glm::vec3));

	glEnableVertexAttribArray(AttribIndex::NORMAL);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(glm::vec3) + sizeof(glm::vec2)));

	glBindBuffer(GL_ARRAY_BUFFER, boneBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBones.size() * sizeof(VertexBoneData), &m_vertexBones[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(AttribIndex::BONE_ID);
	glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), 0);
	glEnableVertexAttribArray(AttribIndex::BONE_ID_2);
	glVertexAttribIPointer(4, MAX_BONES_PER_VERTEX - 4, GL_INT, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 4));

	glEnableVertexAttribArray(AttribIndex::BONE_WEIGHT);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 6));
	glEnableVertexAttribArray(AttribIndex::BONE_WEIGHT_2);
	glVertexAttribPointer(6, MAX_BONES_PER_VERTEX - 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (void*)(sizeof(uint32)* 10));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	glBindVertexArray(0);
	glDeleteBuffers(1, &vertexBuffer);
	glDeleteBuffers(1, &boneBuffer);
	glDeleteBuffers(1, &indexBuffer);
	*/
}
