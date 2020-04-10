namespace glpp {
template <>
inline void VertexArray::AttribFormat<float>(GLuint attribindex,
                                             GLuint relativeoffset) {
  AttribFormat(attribindex, 1, GL_FLOAT, GL_FALSE, relativeoffset);
}

template <>
inline void VertexArray::AttribFormat<glm::vec2>(GLuint attribindex,
                                                 GLuint relativeoffset) {
  AttribFormat(attribindex, 2, GL_FLOAT, GL_FALSE, relativeoffset);
}

template <>
inline void VertexArray::AttribFormat<glm::vec3>(GLuint attribindex,
                                                 GLuint relativeoffset) {
  AttribFormat(attribindex, 3, GL_FLOAT, GL_FALSE, relativeoffset);
}
}  // namespace glpp
