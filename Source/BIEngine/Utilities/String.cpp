#include "String.h"

namespace BIEngine
{
    //Данная реализация взята с ответа от пользователя iFreilicht на этот пост https://stackoverflow.com/questions/2342162/stdstring-formatting-like-sprintf
    template<typename ...Args>
    std::string Format(const std::string& format, Args ... args)
    {
        int size_s = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
        std::unique_ptr<char[]> buf(new char[size_s]);
        std::snprintf(buf.get(), size_s, format.c_str(), args ...);
        return std::string(buf.get(), buf.get() + size_s - 1); // We don't want the '\0' inside
    }

    /*Данная реализация может быть найдена по следущей ссылке: http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html
    Цитата из источника:
    "The original code is from the C/C++ Users Journal. The author is Mike Cornelison."
    "No use restriction is mentioned in the source file or the other files I found in the CD."*/
    bool WildcardMatch(const char* pat, const char* str) {
        int i, star;

    new_segment:

        star = 0;
        if (*pat == '*') {
            star = 1;
            do { pat++; } while (*pat == '*');
        }

    test_match:

        for (i = 0; pat[i] && (pat[i] != '*'); i++) {
            if (str[i] != pat[i]) {
                if (!str[i]) return 0;
                if ((pat[i] == '?') && (str[i] != '.')) continue;
                if (!star) return 0;
                str++;
                goto test_match;
            }
        }
        if (pat[i] == '*') {
            str += i;
            pat += i;
            goto new_segment;
        }
        if (!str[i]) return 1;
        if (i && pat[i - 1] == '*') return 1;
        if (!star) return 0;
        str++;
        goto test_match;
    }

}