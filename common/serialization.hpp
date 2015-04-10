#ifndef SERIALIZTION_HPP
#define SERIALIZTION_HPP

#include <ostream>
#include <istream>
#include <vector>

template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type>
void serialize(std::ostream& stream, T val) {
	stream.write(reinterpret_cast<const char*>(&val), sizeof(T));
}

template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type>
inline void deserialize(std::istream& stream, T& val) {
	stream.read(reinterpret_cast<char*>(&val), sizeof(T) );
}

template<typename T, typename = typename std::enable_if<std::is_pointer<T>::value>::type>
inline void serialize(std::ostream& stream, const T val, size_t size) {
	stream.write(reinterpret_cast<const char*>(val), size);
}

template<typename T, typename = typename std::enable_if<std::is_pointer<T>::value>::type>
inline void deserialize(std::istream& stream,T val, size_t size) {
	stream.read( reinterpret_cast<char*>(val), size);
}

template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type>
inline void serialize(std::ostream& stream, const std::vector<T>& vec) {
	stream.write( reinterpret_cast<const char*>(vec.data() ), vec.size() * sizeof(T) );
}

template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type>
inline void deserialize(std::istream& stream, std::vector<T>& vec) {
	stream.read( reinterpret_cast<char*>(vec.data() ), vec.size() * sizeof(T) );
}

template<typename T, typename = typename std::enable_if<std::is_class<T>::value>::type>
inline bool serialize( std::ostream& stream, const T& object) {
	object.serialize(stream);
	return true;
}

template<typename T, typename = typename std::enable_if<std::is_class<T>::value>::type>
inline bool deserialize(std::istream& stream, T& object) {
	object.deserialize(stream);
	return true;
}

template<typename T, typename = typename std::enable_if<std::is_fundamental<T>::value>::type>
constexpr size_t getSize() {
	return sizeof(T);
}

template<typename T, typename = typename std::enable_if<std::is_class<T>::value>::type>
constexpr size_t getSize(void* = nullptr) {
	return T::getSize();
}

#endif // SERIALIZTION

