#ifndef SETS_SYS_HPP_INCLUDED
#define SETS_SYS_HPP_INCLUDED

#include <iostream>

#include "dynamic_array.hpp"

class SetsSys
{
public:
    SetsSys(DynArr<std::string>&& tops): _tops(std::move(tops)), _indexes(_tops.size())
    {
        for (unsigned int i = 0; i < _indexes.size(); i++)
            _indexes[i] = i;
    }
    bool unionSets(const std::string& set, const std::string& x)
    {
        int indexSet = -1, indexX = -1;
        for (int i = 0; i < _tops.size(); i++)
        {
            if (_tops[i] == set)
                indexSet = i;
            if (_tops[i] == x)
                indexX = i;
        }
        if (indexSet == -1 || indexX == -1)
            return false;

        for (; indexX != _indexes[indexX];)
            indexX = _indexes[indexX];
        
        for (; indexSet != _indexes[indexSet];)
            indexSet = _indexes[indexSet];
        
        _indexes[indexX] = _indexes[indexSet];
        return true;
    }
    std::string findSet(const std::string& x) const
    {
        int indexX = -1;
        for (int i = 0; i < _tops.size(); i++)
        {
            if (_tops[i] == x)
                indexX = i;
        }
        for (; indexX != _indexes[indexX];)
            indexX = _indexes[indexX];

        return _tops[indexX];
    }

friend std::ostream& operator<<(std::ostream&, const SetsSys&);

private:
    DynArr<std::string> _tops;
    DynArr<unsigned int> _indexes;
};

std::ostream& operator<<(std::ostream& os, const SetsSys& setsSys)
{
    for (int i = 0; i < setsSys._tops.size(); i++)
        os << setsSys._tops[i] << " " << setsSys._indexes[i] << "\n";

    return os;
}

#endif // SETS_SYS_HPP_INCLUDED