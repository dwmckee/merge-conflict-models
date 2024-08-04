#include "projectstate.h"
#include "projectid.h"

#include <algorithm>

ProjectState::ProjectState() {};

auto ProjectState::operator[](size_t i) -> value_type&
{
  return _files[i];
}

auto ProjectState::operator[](size_t i) const -> const value_type&
{
  return _files[i];
}

auto ProjectState::at(size_t i) -> value_type&
{
    return _files.at(i);
}
auto ProjectState::at(size_t i) const -> const value_type&
{
    return _files.at(i);
}

auto ProjectState::find(id_t id) -> iterator
{
  return std::find_if(begin(), end(),
		      [&id](const auto &pf){ return pf.id() == id; });
}

auto ProjectState::find(id_t id) const -> const_iterator
{
  return std::find_if(cbegin(),cend(),
		      [&id](const auto &pf){ return pf.id() == id; });
}

ProjectFile &ProjectState::addFile(id_t id)
{
    _files.emplace_back(id);
    return _files.back();
}

std::pair<id_t, id_t> ProjectState::fileSizeLimits() const
{
  const auto [minIt, maxIt] = std::minmax_element(begin(), end(),
        [](const auto& lhs, const auto& rhs) {
            return lhs.size() < rhs.size();
        });
    return std::make_pair(minIt->size(),maxIt->size());
}
