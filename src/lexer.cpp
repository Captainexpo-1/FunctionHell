#include "lexer.hpp"

Lexer::Lexer() {
    m_CurrentIndex = 0;
    m_Line = 1;
    m_Column = 1;
}

std::vector<Token> Lexer::tokenize(std::string source) {
    m_Source = source;
    tokens.clear();
    std::smatch match;

    while (m_CurrentIndex < m_Source.size()) {
        bool matched = false;

        for (const auto& tokenType : token_priorities) {
            std::regex regex(token_regexes[tokenType]);
            std::string substring = m_Source.substr(m_CurrentIndex);

            if (std::regex_search(substring, match, regex, std::regex_constants::match_continuous)) {
                if (tokenType != WHITESPACE && tokenType != COMMENT) { // Skip whitespaces and comments
                    
                    //std::cout << "GOT: " << token_strings[tokenType] << " at line " << m_Line << ", column " << m_Column << ": " << (tokenType != NEWLINE ? match.str() : "\\n") << std::endl;
                    tokens.emplace_back(tokenType, match.str(), m_Line, m_Column);
                    if(tokenType == NEWLINE) {
                        m_Line += 1;
                        m_Column = 1;
                    }
                }

                m_CurrentIndex += match.length();
                m_Column += match.length();
                matched = true;
                break;
            }
        }

        if (!matched) {
            std::cerr << "Unexpected character at line " << m_Line << ", column " << m_Column << ": " << m_Source[m_CurrentIndex] << std::endl;
            ++m_CurrentIndex;
            ++m_Column;
        }
    }

    tokens.emplace_back(END_OF_FILE, "", m_Line, m_Column);
    return tokens;
}