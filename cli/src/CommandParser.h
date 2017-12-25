/*
 * CommandParser.h
 *
 *  Created on: June 13, 2017
 *      Author: j.zhou
 */

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>

class Qmss;

namespace cli {

    enum {
        MAX_COMMAND_CONTENT_LENGTH = 512
    };

    class CommandParser {
    public:
        CommandParser();
        ~CommandParser();

        bool parse(int argc, char* argv[]);

        bool send(Qmss* qmss);

    private:
        bool m_isValid;
        int m_cmdType;
        int m_tgtType;
        int m_subTgtType;
        char m_cmdContent[MAX_COMMAND_CONTENT_LENGTH];

        char m_sendBuffer[MAX_COMMAND_CONTENT_LENGTH];

        // for TEST command
        int m_numUe;
        int m_numTestTime;
        
        void showUsage();
        bool parseParam(std::string option, int index);
        int s2i(std::string theString);
    };

}

#endif
