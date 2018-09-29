#pragma once

#include <cstdlib>
#include <cstdint>
#include <string>
#include <cstring>
#include <map>
#include <unordered_map>
#include <vector>
#include <set>
#include <stdexcept>

namespace agora { namespace tools {

class packer
{
    enum
    {
        PACKET_BUFFER_SIZE = 1024,
        PACKET_BUFFER_SIZE_MAX = 64*1024
    }
    ;
public:
    packer()
        : buffer_(PACKET_BUFFER_SIZE)
        , length_(0)
        , position_(2)
    {
    }
    ~packer()
    {
    }

public:
    packer & pack()
    {
        length_ = position_;
        position_ = 0;
        *this << length_;
        position_ = length_;
        return *this;
    }

    void reset()
    {
        length_ = 0;
        position_ = 2;
    }

    void write(uint16_t val, uint16_t position)
    {
        check_size(sizeof(val), position);
        ::memcpy(&buffer_[0] + position, &val, sizeof(val));
    }

    void write(uint32_t val, uint16_t position)
    {
        check_size(sizeof(val), position);
        ::memcpy(&buffer_[0] + position, &val, sizeof(val));
    }

    void push(uint64_t val)
    {
        check_size(sizeof(val), position_);
        ::memcpy(&buffer_[0] + position_, &val, sizeof(val));
        position_ += sizeof(val);
    }

    void push(uint32_t val)
    {
        check_size(sizeof(val), position_);
        ::memcpy(&buffer_[0] + position_, &val, sizeof(val));
        position_ += sizeof(val);
    }

    void push(uint16_t val)
    {
        check_size(sizeof(val), position_);
        ::memcpy(&buffer_[0] + position_, &val, sizeof(val));
        position_ += sizeof(val);
    }

    void push(uint8_t val)
    {
        check_size(sizeof(val), position_);
        ::memcpy(&buffer_[0] + position_, &val, sizeof(val));
        position_ += sizeof(val);
    }

    void push(const std::string & val)
    {
        push(static_cast<uint16_t>(val.length()));

        size_t length = val.length();
        check_size(length, position_);
        if (length > 0)
        {
            ::memcpy(&buffer_[0] + position_, val.data(), length);
            position_ += length;
        }
    }

    packer& push(const void* data, size_t length)
    {
        check_size(length, position_);
        if (length > 0) {
            ::memcpy(&buffer_[0] + position_, data, length);
            position_ += length;
        }
        return *this;
    }

    const char*	buffer() const { return &buffer_[0]; }
    size_t	length() const { return length_; }
    std::string body() const { return std::string(&buffer_[0] + 2, length_ - 2); }

    void check_size(size_t more, uint16_t position)
    {
        if (buffer_.size() - position < more) {
            size_t new_size = buffer_.size() * 4;
            if (new_size > PACKET_BUFFER_SIZE_MAX) {
                throw std::overflow_error("packer buffer overflow!");
            }
            buffer_.resize(new_size);
        }
    }

    packer& operator<< (uint64_t v)
    {
        push(v);
        return *this;
    }

    packer& operator<< (uint32_t v)
    {
        push(v);
        return *this;
    }

    packer& operator<< (uint16_t v)
    {
        push(v);
        return *this;
    }
    packer& operator<< (uint8_t v)
    {
        push(v);
        return *this;
    }

    packer& operator<< (int64_t v)
    {
        push(static_cast<uint64_t>(v));
        return *this;
    }

    packer& operator<< (int32_t v)
    {
        push(static_cast<uint32_t>(v));
        return *this;
    }

    packer& operator<< (int16_t v)
    {
        push(static_cast<uint16_t>(v));
        return *this;
    }
    packer& operator<< (int8_t v)
    {
        push(static_cast<uint8_t>(v));
        return *this;
    }

    packer& operator<< (const std::string & v)
    {
        push(v);
        return *this;
    }

    template<typename T>
    packer& operator<< (const std::vector<T> &v)
    {
        uint16_t count = v.size();
        *this << count;
        for (uint16_t i = 0; i < count; i++) {
            *this << v[i];
        }
        return *this;
    }

    template<typename T>
    packer& operator<< (const std::set<T> &v)
    {
        uint16_t count = v.size();
        *this << count;
        for (const T& x : v) {
            *this << x;
        }
        return *this;
    }

    template<typename K, typename V>
    packer& operator<< (const std::pair<K, V> & p)
    {
        *this << p.first << p.second;
        return *this;
    }

    template<typename K, typename V>
    packer& operator<< (const std::map<K, V> & v)
    {
        uint16_t count = v.size();
        *this << count;
        for (const typename std::map<K, V>::value_type& x : v) {
            *this << x;
        }
        return *this;
    }

    template<typename K, typename V>
    packer& operator<< (const std::unordered_map<K, V> & v)
    {
        uint16_t count = v.size();
        *this << count;
        for (const typename std::unordered_map<K, V>::value_type& x : v) {
            *this << x;
        }
        return *this;
    }

private:
    std::vector<char> buffer_;
    uint16_t	length_;
    uint16_t	position_;
};

class unpacker
{
public:
    unpacker(const char* buf, size_t len, bool copy = false)
        : buffer_(NULL)
        , length_(len)
        , position_(0)
        , copy_(copy)
    {
        if (copy_) {
            buffer_ = new char[len];
            ::memcpy(buffer_, buf, len);
        }
        else {
            buffer_ = const_cast<char *>(buf);
        }
    }
    ~unpacker()	{
        if (copy_) {
            delete [] buffer_;
        }
    }

public:
    unpacker& rewind() {
        position_ = 2;
        return *this;
    }

    void reset() {
        position_ = 0;
    }

    void write(uint16_t val, uint16_t position) {
        check_size(sizeof(val), position);
        ::memcpy(buffer_ + position, &val, sizeof(val));
    }

    uint64_t pop_uint64() {
        uint64_t v = 0;
        check_size(sizeof(v), position_);
        ::memcpy(&v, buffer_ + position_, sizeof(v));
        position_ += sizeof(v);
        return v;
    }

    uint32_t pop_uint32() {
        uint32_t v = 0;
        check_size(sizeof(v), position_);
        ::memcpy(&v, buffer_ + position_, sizeof(v));
        position_ += sizeof(v);
        return v;
    }

    uint16_t pop_uint16() {
        uint16_t v = 0;
        check_size(sizeof(v), position_);
        ::memcpy(&v, buffer_ + position_, sizeof(v));
        position_ += sizeof(v);
        return v;
    }

    uint8_t	pop_uint8() {
        uint8_t v = 0;
        check_size(sizeof(v), position_);
        ::memcpy(&v, buffer_ + position_, sizeof(v));
        position_ += sizeof(v);
        return v;
    }

    std::string	pop_string() {
        uint16_t length = pop_uint16();
        check_size(length, position_);
        std::string s = std::string(buffer_ + position_, length);
        position_ += length;

        return s;
    }

    const char*	buffer() const {
        return buffer_;
    }

    size_t length() const {
        return length_;
    }

    void check_size(size_t more, uint16_t position) const {
        if (static_cast<size_t>(length_ - position) < more) {
            throw std::overflow_error("unpacker buffer overflow!");
        }
    }

    unpacker& operator>> (uint64_t & v)
    {
        v = pop_uint64();
        return *this;
    }

    unpacker& operator>> (uint32_t & v)
    {
        v = pop_uint32();
        return *this;
    }
    unpacker& operator>> (uint16_t & v)
    {
        v = pop_uint16();
        return *this;
    }
    unpacker& operator>> (uint8_t & v)
    {
        v = pop_uint8();
        return *this;
    }

    unpacker& operator>> (int64_t & v)
    {
        v = static_cast<int64_t>(pop_uint64());
        return *this;
    }
    unpacker& operator>> (int32_t & v)
    {
        v = static_cast<int32_t>(pop_uint32());
        return *this;
    }
    unpacker& operator>> (int16_t & v)
    {
        v = static_cast<int16_t>(pop_uint16());
        return *this;
    }
    unpacker& operator>> (int8_t & v)
    {
        v = static_cast<int8_t>(pop_uint8());
        return *this;
    }
    unpacker& operator>> (std::string & v)
    {
        v = pop_string();
        return *this;
    }

    template<typename T>
    unpacker& operator>> (std::vector<T> &v)
    {
        uint16_t count = pop_uint16();
        for (uint16_t i = 0; i < count; i++) {
            T t;
            *this >> t;
            v.push_back(t);
        }
        return *this;
    }

    template<typename T>
    unpacker& operator>> (std::set<T> &v)
    {
        uint16_t count = pop_uint16();
        for (uint16_t i = 0; i < count; i++) {
            T t;
            *this >> t;
            v.insert(t);
        }
        return *this;
    }

    template<typename K, typename V>
    unpacker& operator>> (std::map<K, V> & x)
    {
        uint16_t count = pop_uint16();
        for (uint16_t i = 0; i < count; i++) {
            K k;
            V v;
            *this >> k >> v;
            x.insert(std::make_pair(k, v));
        }
        return *this;
    }

    template<typename K, typename V>
    unpacker& operator>> (std::unordered_map<K, V> & x)
    {
        uint16_t count = pop_uint16();
        for (uint16_t i = 0; i < count; i++) {
            K k;
            V v;
            *this >> k >> v;
            x.insert(std::make_pair(k, v));
        }
        return *this;
    }

private:
    char *			buffer_;
    uint16_t		length_;
    uint16_t		position_;
    bool			copy_;
};

#ifndef __DECLARE_STRUCT__
#define __DECLARE_STRUCT__

#define DECLARE_STRUCT_END };

#define DECLARE_SIMPLE_STRUCT_1_START(name,type1,name1) \
struct name { \
type1 name1;\
name():name1(){}\
name(type1 n1):name1(n1){}\
name(const name& r):name1(r.name1){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  return *this; \
}

#define DECLARE_SIMPLE_STRUCT_1(name,type1,name1) DECLARE_SIMPLE_STRUCT_1_START(name,type1,name1) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_2_START(name,type1,name1,type2,name2) \
struct name { \
type1 name1;\
type2 name2;\
name():name1(),name2(){}\
name(type1 n1,type2 n2):name1(n1),name2(n2){}\
name(const name& r):name1(r.name1),name2(r.name2){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_2(name,type1,name1,type2,name2) DECLARE_SIMPLE_STRUCT_2_START(name,type1,name1,type2,name2) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
name():name1(),name2(),name3(){}\
name(type1 n1,type2 n2,type3 n3):name1(n1),name2(n2),name3(n3){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_3(name,type1,name1,type2,name2,type3,name3) DECLARE_SIMPLE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
name():name1(),name2(),name3(),name4(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4):name1(n1),name2(n2),name3(n3),name4(n4){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_4(name,type1,name1,type2,name2,type3,name3,type4,name4) DECLARE_SIMPLE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
type5 name5;\
name():name1(),name2(),name3(),name4(),name5(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4,type5 n5):name1(n1),name2(n2),name3(n3),name4(n4),name5(n5){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4),name5(r.name5){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  name5 = r.name5; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_5(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) DECLARE_SIMPLE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
type5 name5;\
type6 name6;\
name():name1(),name2(),name3(),name4(),name5(),name6(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4,type5 n5,type6 n6):name1(n1),name2(n2),name3(n3),name4(n4),name5(n5),name6(n6){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4),name5(r.name5),name6(r.name6){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  name5 = r.name5; \
  name6 = r.name6; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_6(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) DECLARE_SIMPLE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
type5 name5;\
type6 name6;\
type7 name7;\
name():name1(),name2(),name3(),name4(),name5(),name6(),name7(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4,type5 n5,type6 n6,type7 n7):name1(n1),name2(n2),name3(n3),name4(n4),name5(n5),name6(n6),name7(n7){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4),name5(r.name5),name6(r.name6),name7(r.name7){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  name5 = r.name5; \
  name6 = r.name6; \
  name7 = r.name7; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_7(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) DECLARE_SIMPLE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
type5 name5;\
type6 name6;\
type7 name7;\
type8 name8;\
name():name1(),name2(),name3(),name4(),name5(),name6(),name7(),name8(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4,type5 n5,type6 n6,type7 n7,type8 n8):name1(n1),name2(n2),name3(n3),name4(n4),name5(n5),name6(n6),name7(n7),name8(n8){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4),name5(r.name5),name6(r.name6),name7(r.name7),name8(r.name8){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  name5 = r.name5; \
  name6 = r.name6; \
  name7 = r.name7; \
  name8 = r.name8; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_8(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) DECLARE_SIMPLE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
DECLARE_STRUCT_END

#define DECLARE_SIMPLE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
struct name { \
type1 name1;\
type2 name2;\
type3 name3;\
type4 name4;\
type5 name5;\
type6 name6;\
type7 name7;\
type8 name8;\
type9 name9;\
name():name1(),name2(),name3(),name4(),name5(),name6(),name7(),name8(),name9(){}\
name(type1 n1,type2 n2,type3 n3,type4 n4,type5 n5,type6 n6,type7 n7,type8 n8,type9 n9):name1(n1),name2(n2),name3(n3),name4(n4),name5(n5),name6(n6),name7(n7),name8(n8),name9(n9){}\
name(const name& r):name1(r.name1),name2(r.name2),name3(r.name3),name4(r.name4),name5(r.name5),name6(r.name6),name7(r.name7),name8(r.name8),name9(r.name9){}\
name & operator=(const name & r) { \
  if (this == &r) return *this; \
  name1 = r.name1; \
  name2 = r.name2; \
  name3 = r.name3; \
  name4 = r.name4; \
  name5 = r.name5; \
  name6 = r.name6; \
  name7 = r.name7; \
  name8 = r.name8; \
  name9 = r.name9; \
  return *this; \
}
#define DECLARE_SIMPLE_STRUCT_9(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) DECLARE_SIMPLE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
DECLARE_STRUCT_END


#define DECLARE_STRUCT_1_START(name,type1,name1) DECLARE_SIMPLE_STRUCT_1_START(name,type1,name1) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_1(name,type1,name1) DECLARE_STRUCT_1_START(name,type1,name1) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_2_START(name,type1,name1,type2,name2) DECLARE_SIMPLE_STRUCT_2_START(name,type1,name1,type2,name2) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_2(name,type1,name1,type2,name2) DECLARE_STRUCT_2_START(name,type1,name1,type2,name2) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) DECLARE_SIMPLE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_3(name,type1,name1,type2,name2,type3,name3) DECLARE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) DECLARE_SIMPLE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_4(name,type1,name1,type2,name2,type3,name3,type4,name4) DECLARE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) DECLARE_SIMPLE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
  if (l.name5 != r.name5) \
  { \
    return l.name5 < r.name5; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  if (l.name5 != r.name5) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_5(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) DECLARE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) DECLARE_SIMPLE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
  if (l.name5 != r.name5) \
  { \
    return l.name5 < r.name5; \
  } \
  if (l.name6 != r.name6) \
  { \
    return l.name6 < r.name6; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  if (l.name5 != r.name5) \
  { \
    return false; \
  } \
  if (l.name6 != r.name6) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_6(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) DECLARE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) DECLARE_SIMPLE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
  if (l.name5 != r.name5) \
  { \
    return l.name5 < r.name5; \
  } \
  if (l.name6 != r.name6) \
  { \
    return l.name6 < r.name6; \
  } \
  if (l.name7 != r.name7) \
  { \
    return l.name7 < r.name7; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  if (l.name5 != r.name5) \
  { \
    return false; \
  } \
  if (l.name6 != r.name6) \
  { \
    return false; \
  } \
  if (l.name7 != r.name7) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_7(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) DECLARE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) DECLARE_SIMPLE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
  if (l.name5 != r.name5) \
  { \
    return l.name5 < r.name5; \
  } \
  if (l.name6 != r.name6) \
  { \
    return l.name6 < r.name6; \
  } \
  if (l.name7 != r.name7) \
  { \
    return l.name7 < r.name7; \
  } \
  if (l.name8 != r.name8) \
  { \
    return l.name8 < r.name8; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  if (l.name5 != r.name5) \
  { \
    return false; \
  } \
  if (l.name6 != r.name6) \
  { \
    return false; \
  } \
  if (l.name7 != r.name7) \
  { \
    return false; \
  } \
  if (l.name8 != r.name8) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_8(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) DECLARE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
DECLARE_STRUCT_END
#define DECLARE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) DECLARE_SIMPLE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
friend bool operator<(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return l.name1 < r.name1; \
  } \
  if (l.name2 != r.name2) \
  { \
    return l.name2 < r.name2; \
  } \
  if (l.name3 != r.name3) \
  { \
    return l.name3 < r.name3; \
  } \
  if (l.name4 != r.name4) \
  { \
    return l.name4 < r.name4; \
  } \
  if (l.name5 != r.name5) \
  { \
    return l.name5 < r.name5; \
  } \
  if (l.name6 != r.name6) \
  { \
    return l.name6 < r.name6; \
  } \
  if (l.name7 != r.name7) \
  { \
    return l.name7 < r.name7; \
  } \
  if (l.name8 != r.name8) \
  { \
    return l.name8 < r.name8; \
  } \
  if (l.name9 != r.name9) \
  { \
    return l.name9 < r.name9; \
  } \
 return false; \
} \
friend bool operator==(const name & l, const name & r) \
{ \
  if (l.name1 != r.name1) \
  { \
    return false; \
  } \
  if (l.name2 != r.name2) \
  { \
    return false; \
  } \
  if (l.name3 != r.name3) \
  { \
    return false; \
  } \
  if (l.name4 != r.name4) \
  { \
    return false; \
  } \
  if (l.name5 != r.name5) \
  { \
    return false; \
  } \
  if (l.name6 != r.name6) \
  { \
    return false; \
  } \
  if (l.name7 != r.name7) \
  { \
    return false; \
  } \
  if (l.name8 != r.name8) \
  { \
    return false; \
  } \
  if (l.name9 != r.name9) \
  { \
    return false; \
  } \
  return true; \
} \
friend bool operator!=(const name & l, const name & r) \
{ \
  return !(l == r); \
}

#define DECLARE_STRUCT_9(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) DECLARE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
DECLARE_STRUCT_END

#endif

#ifndef __DECLARE_PACKABLE__
#define __DECLARE_PACKABLE__

#define DECLARE_PACKABLE_1_START(name,type1,name1) DECLARE_STRUCT_1_START(name,type1,name1) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1; \
    return p;\
}
#define DECLARE_PACKABLE_1(name,type1,name1) DECLARE_PACKABLE_1_START(name,type1,name1) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_2_START(name,type1,name1,type2,name2) DECLARE_STRUCT_2_START(name,type1,name1,type2,name2) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2; \
    return p;\
}
#define DECLARE_PACKABLE_2(name,type1,name1,type2,name2) DECLARE_PACKABLE_2_START(name,type1,name1,type2,name2) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_3_START(name,type1,name1,type2,name2,type3,name3) DECLARE_STRUCT_3_START(name,type1,name1,type2,name2,type3,name3) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3; \
    return p;\
}
#define DECLARE_PACKABLE_3(name,type1,name1,type2,name2,type3,name3) DECLARE_PACKABLE_3_START(name,type1,name1,type2,name2,type3,name3) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) DECLARE_STRUCT_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4; \
    return p;\
}
#define DECLARE_PACKABLE_4(name,type1,name1,type2,name2,type3,name3,type4,name4) DECLARE_PACKABLE_4_START(name,type1,name1,type2,name2,type3,name3,type4,name4) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) DECLARE_STRUCT_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4 << x.name5; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4 >> x.name5; \
    return p;\
}
#define DECLARE_PACKABLE_5(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) DECLARE_PACKABLE_5_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) DECLARE_STRUCT_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4 << x.name5 << x.name6; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4 >> x.name5 >> x.name6; \
    return p;\
}
#define DECLARE_PACKABLE_6(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) DECLARE_PACKABLE_6_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) DECLARE_STRUCT_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4 << x.name5 << x.name6 << x.name7; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4 >> x.name5 >> x.name6 >> x.name7; \
    return p;\
}
#define DECLARE_PACKABLE_7(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) DECLARE_PACKABLE_7_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) DECLARE_STRUCT_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4 << x.name5 << x.name6 << x.name7 << x.name8; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4 >> x.name5 >> x.name6 >> x.name7 >> x.name8; \
    return p;\
}
#define DECLARE_PACKABLE_8(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) DECLARE_PACKABLE_8_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8) \
DECLARE_STRUCT_END
#define DECLARE_PACKABLE_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) DECLARE_STRUCT_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
friend packer & operator<< (packer& p, const name & x) \
{	\
    p << x.name1 << x.name2 << x.name3 << x.name4 << x.name5 << x.name6 << x.name7 << x.name8 << x.name9; \
    return p;\
}\
friend unpacker & operator>> (unpacker & p, name & x) \
{			\
  p >> x.name1 >> x.name2 >> x.name3 >> x.name4 >> x.name5 >> x.name6 >> x.name7 >> x.name8 >> x.name9; \
    return p;\
}
#define DECLARE_PACKABLE_9(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) DECLARE_PACKABLE_9_START(name,type1,name1,type2,name2,type3,name3,type4,name4,type5,name5,type6,name6,type7,name7,type8,name8,type9,name9) \
DECLARE_STRUCT_END

#endif
}}
