#include "interfaces.h"

ConflictCount ConflictCount::operator+(const ConflictCount& that) const
{
    auto result(*this);
    result += that;
    return result;
}

ConflictCount& ConflictCount::operator+=(const ConflictCount& that)
{
    baseFiles += that.baseFiles;
    baseElements += that.baseElements;
    elementChanges += that.elementChanges;
    elementConflicts += that.elementConflicts;
    fileConflicts += that.fileConflicts;
    manualResolutions += that.manualResolutions;
    maxFileSize = std::max(maxFileSize, that.maxFileSize);
    
    return *this;
}
