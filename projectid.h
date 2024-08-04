#ifndef PROJECTID_H
#define PROJECTID_H

#include <cstdint>

using id_t = uint32_t;

id_t nextId();

id_t operator""_id (unsigned long long value);

#endif//PROJECTID_H
