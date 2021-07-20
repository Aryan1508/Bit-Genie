/*
  Bit-Genie is an open-source, UCI-compliant chess engine written by
  Aryan Parekh - https://github.com/Aryan1508/Bit-Genie

  Bit-Genie is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Bit-Genie is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once
#include <array>

template<typename T, std::size_t S>
class FixedList
{
public:
    using iterator       = typename std::array<T, S>::iterator;
    using const_iterator = typename std::array<T, S>::const_iterator;

    FixedList() = default;

    iterator begin() 
    {
        return data.begin();
    }    

    iterator end() 
    {
        return data.begin() + height;
    }

    const_iterator begin() const
    {
        return data.begin();
    }    

    const_iterator end() const
    {
        return data.begin() + height;
    }

    void push_back(T const& elem) 
    {
        data[height++] = elem;
    }

    void pop() 
    {
        height--;
    }

    void clear()
    {
        height = 0;
    }

    std::size_t size() const  
    {
        return height;
    }

    T const& operator[](std::size_t pos) const { return data[pos]; }
    T      & operator[](std::size_t pos)       { return data[pos]; }
private:
    std::array<T, S> data;
    std::size_t height = 0;
};