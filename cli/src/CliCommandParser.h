/*
 * CliCommandParser.h
 *
 *  Created on: June 13, 2017
 *      Author: j.zhou
 */

#ifndef CLI_COMMAND_PARSER_H
#define CLI_COMMAND_PARSER_H

#include <string>

class Qmss;

namespace cli {

    enum {
        MAX_COMMAND_CONTENT_LENGTH = 512
    };

    class CliCommandParser {
    public:
        CliCommandParser();
        ~CliCommandParser();

        bool parse(int argc, char* argv[]);

        bool execute(Qmss* qmss, int argc, char* argv[]);

    private:
        bool m_isValid;
        int m_cmdType;
        int m_tgtType;
        int m_subTgtType;
        int m_msgId;
        char m_cmdContent[MAX_COMMAND_CONTENT_LENGTH];

        char m_sendBuffer[MAX_COMMAND_CONTENT_LENGTH];

        // for TEST command
        int m_numUe;
        int m_numTestTime;

        // for KPI
        int m_argcKpi;
        char** m_argvKpi;
        
        void showUsage();
        bool parseParam(std::string option, int index);
        int s2i(std::string theString);
    };

}

#endif
