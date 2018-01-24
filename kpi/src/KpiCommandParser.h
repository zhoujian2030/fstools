/*
 * KpiCommandParser.h
 *
 *  Created on: May 10, 2017
 *      Author: j.zhou
 */

#ifndef KPI_COMMAND_PARSER_H
#define KPI_COMMAND_PARSER_H

#include <string>

class Qmss;

namespace kpi {

    class KpiCommandParser {
    public:
        KpiCommandParser();
        ~KpiCommandParser();

#ifdef KPI_L3
        bool parseAndExecute(int argc, char* argv[])
#else
        bool parseAndExecute(Qmss* qmss, int argc, char* argv[]);
#endif

    private:        
        void parseMinorOptions(std::string option, std::string value);
        void parseOptions(std::string option, std::string value);

        void showUsage();
        void showMinorUsage();
    };

}

#endif
