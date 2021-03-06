#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <alignment/tuples/CompressedDNATuple.hpp>
#include <alignment/tuples/DNATuple.hpp>
#include <alignment/tuples/TupleCountTable.hpp>
#include <alignment/tuples/TupleMetrics.hpp>
#include <pbdata/CommandLineParser.hpp>
#include <pbdata/FASTAReader.hpp>
#include <pbdata/FASTASequence.hpp>
#include <pbdata/utils.hpp>

#ifdef COMPRESSED
typedef TupleCountTable<FASTASequence, CompressedDNATuple<FASTASequence> > CountTable;
#else
typedef TupleCountTable<FASTASequence, DNATuple> CountTable;
#endif

int main(int argc, char* argv[])
{
    CommandLineParser clp;
    std::string tableFileName;
    std::vector<std::string> sequenceFiles;
    TupleMetrics tm;
    int tupleSize = 8;
    clp.SetProgramName("printTupleCountTable");
    clp.SetProgramSummary("Count the number of occurrences of every k-mer in a file.");
    clp.RegisterStringOption("table", &tableFileName, "Output table name.", true);
    clp.RegisterIntOption("wordsize", &tupleSize, "Size of words to count",
                          CommandLineParser::NonNegativeInteger, false);
    clp.RegisterStringListOption("reads", &sequenceFiles, "All sequences.", false);
    clp.RegisterPreviousFlagsAsHidden();
    std::vector<std::string> opts;

    if (argc == 2) {
        std::string fastaFileName = argv[1];
        sequenceFiles.push_back(fastaFileName);
        tableFileName = fastaFileName + ".ctab";
    } else {
        clp.ParseCommandLine(argc, argv, opts);
    }

    tm.tupleSize = tupleSize;
    tm.InitializeMask();
    std::ofstream tableOut;
    CrucialOpen(tableFileName, tableOut, std::ios::out | std::ios::binary);
    CountTable table;
    table.InitCountTable(tm);
    FASTASequence seq;
    for (size_t i = 0; i < sequenceFiles.size(); i++) {
        FASTAReader reader;
        reader.Init(sequenceFiles[i]);
        while (reader.GetNext(seq)) {
            seq.ToUpper();
            table.AddSequenceTupleCountsLR(seq);
        }
    }
    table.Write(tableOut);

    return 0;
}
