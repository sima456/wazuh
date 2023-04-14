#include "stringUtils.hpp"

namespace base::utils::string
{

std::vector<std::string> split(std::string_view str, const char delimiter)
{
    std::vector<std::string> ret;
    while (true)
    {
        auto pos = str.find(delimiter);
        if (pos == str.npos)
        {
            break;
        }
        ret.emplace_back(str.substr(0, pos));
        str = str.substr(pos + 1);
    }

    if (!str.empty())
    {
        ret.emplace_back(str);
    }

    return ret;
}

std::string join(const std::vector<std::string>& strVector, std::string_view separator, const bool startsWithSeparator)
{
    std::string strResult {};
    for (std::size_t i = 0; i < strVector.size(); ++i)
    {
        strResult.append((!startsWithSeparator && 0 == i) ? "" : separator);
        strResult.append(strVector.at(i));
    }

    return strResult;
}

std::vector<std::string> splitEscaped(std::string_view input, const char& splitChar, const char& escape)
{
    std::vector<std::string> splitted;
    // Add first segment
    splitted.emplace_back("");

    for (std::size_t i = 0; i < input.size(); ++i)
    {
        const auto& thisChar = input[i];
        if (thisChar == escape && i + 1 < input.size())
        {
            const auto& nextChar = input[i + 1];
            // Escape char
            if (nextChar == escape || nextChar == splitChar)
            {
                splitted.back() += nextChar;
                ++i;
            }
            else
            {
                splitted.back() += thisChar;
            }
        }
        else if (thisChar == splitChar)
        {
            // Add another segment
            splitted.push_back("");
        }
        else
        {
            splitted.back() += thisChar;
        }
    }

    return splitted;
}

} // namespace base::utils::string
