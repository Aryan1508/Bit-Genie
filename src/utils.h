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
#include <chrono>
#include <vector>
#include <string>
#include <cctype>
#include <sstream>
#include <algorithm>

template <typename T, std::size_t S>
class FixedList {
public:
    using iterator       = typename std::array<T, S>::iterator;
    using const_iterator = typename std::array<T, S>::const_iterator;

    FixedList() = default;

    iterator begin() {
        return data.begin();
    }

    iterator end() {
        return data.begin() + height;
    }

    const_iterator begin() const {
        return data.begin();
    }

    const_iterator end() const {
        return data.begin() + height;
    }

    void push_back(T const &elem) {
        data[height++] = elem;
    }

    void pop() {
        height--;
    }

    void clear() {
        height = 0;
    }

    std::size_t size() const {
        return height;
    }

    T &back() {
        return data[height - 1];
    }

    T const &operator[](std::size_t pos) const {
        return data[pos];
    }
    T &operator[](std::size_t pos) {
        return data[pos];
    }

private:
    std::array<T, S> data;
    std::size_t height = 0;
};

template <typename duration = std::chrono::milliseconds>
class StopWatch {
    using clock      = std::chrono::steady_clock;
    using time_point = std::chrono::time_point<clock, duration>;

    static constexpr time_point current_time() noexcept {
        return std::chrono::time_point_cast<duration>(clock::now());
    }

public:
    StopWatch() = default;

    void reset() noexcept {
        clock_state = State::idle;
    }

    time_point go() noexcept {
        if (clock_state != State::stopped)
            start_point = current_time();

        clock_state = State::running;
        return start_point;
    }

    void stop() noexcept {
        if (clock_state == State::running) {
            stop_point  = current_time();
            clock_state = State::stopped;
        }
    }

    duration elapsed_time() const noexcept {
        switch (clock_state) {
        case State::idle:
            return std::chrono::duration_cast<duration>(
                clock::duration::zero());

        case State::running:
            return std::chrono::duration_cast<duration>(
                current_time() - start_point);

        case State::stopped:
            return std::chrono::duration_cast<duration>(
                stop_point - start_point);

        default:
            return std::chrono::duration_cast<duration>(
                clock::duration::zero());
            break;
        }
    }

protected:
    enum class State : unsigned char { idle,
                                       running,
                                       stopped };

    time_point start_point;
    time_point stop_point;
    State clock_state = State::idle;
};

template <typename Entry>
class HashTable {
private:
    std::vector<Entry> entries;

public:
    HashTable() {
        resize(8);
    }

    explicit HashTable(const std::uint64_t mb) {
        resize(mb);
    }

    void resize(const std::uint64_t mb) {
        entries.resize((mb * 1024 * 1024) / sizeof(Entry), Entry());
    }

    void reset(Entry const &entry) {
        std::fill(entries.begin(), entries.end(), entry);
    }

    void reset() {
        reset(Entry());
    }

    auto &probe(const std::uint64_t hash) {
        return entries[hash % entries.size()];
    }

    auto const &probe(const std::uint64_t hash) const {
        return entries[hash % entries.size()];
    }

    void add(const std::uint64_t hash, Entry const &entry) {
        probe(hash) = entry;
    }
};

inline void trim_str(std::string &str) {
    if (str.length() == 0)
        return;

    std::stringstream stream(str);
    std::string out;
    std::string temp;

    while (stream >> temp)
        out += temp + ' ';

    out.pop_back();
    str = std::move(out);
}

inline auto &convert_str_to_lower(std::string &str) {
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
    return str;
}

inline auto split_string(std::string_view str) {
    std::vector<std::string> parts;
    std::stringstream stream(str.data());
    std::string temp;
    while (stream >> temp) {
        parts.push_back(std::move(temp));
    }
    return parts;
}

inline auto split_string(std::string_view str, char delim) {
    std::vector<std::string> parts;
    std::stringstream stream(str.data());
    std::string temp;

    while (std::getline(stream, temp, delim)) {
        parts.push_back(std::move(temp));
    }
    return parts;
}

inline bool starts_with(std::string_view str, std::string_view comp) {
    return str.rfind(comp, 0) == 0;
}