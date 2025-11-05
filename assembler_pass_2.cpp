#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

enum { SYM_TAB, LIT_TAB, PROGRAM };

enum { SYM, VAL };
enum { LC, INST, OPR1, OPR2, COUNT };

vector<pair<string, string>> sym_table, lit_table;

const string no_mc = "-------";

#define str_chop(str, start_chop, end_chop) str.substr(start_chop, str.size() - start_chop - end_chop)
#define sym_deref(tab, idx_str) tab[stoi(idx_str) - 1].second

string inst_handler(string inst)
{
    if      (inst.substr(1, 2) == "AD") { return no_mc; } 
    else if (inst.substr(1, 2) == "IS") { return str_chop(inst, 4, 1); }
    else if (inst.substr(1, 2) == "DL") { return str_chop(inst, 4, 1) == "01" ? "00" : no_mc; }
    else                                { return "UNKNOWN INST '" + inst + "'"; }
}

string operand_handler(string opr)
{
    if      (opr.size() == 3)           { return str_chop(opr, 1, 1); }
    else if (opr[1] == 'C')             { return str_chop(opr, 3, 1); }
    else if (opr[1] == 'S')             { return sym_deref(sym_table, str_chop(opr,3,1)); }
    else if (opr[1] == 'L')             { return sym_deref(lit_table, str_chop(opr,3,1)); }
    else                                { return "UNKNOWN OPR '" + opr + "'"; }
}

int main()
{
    string line, output = "";
    int idx = SYM_TAB;

    while (getline(cin, line) && idx <= PROGRAM) {
        if (line.find_first_not_of(" \t") == string::npos) {
            idx += 1;
            // cout << "\n" << idx << "\n";
            continue;
        }

        istringstream iss(line);
        string words[COUNT], buf;

        int i = isspace(line[0]) ? INST : LC;
        while (i < COUNT && iss >> buf)
            words[i++] = buf;

        if (idx == SYM_TAB)      { sym_table.push_back({ words[SYM], words[VAL] }); }
        else if (idx == LIT_TAB) { lit_table.push_back({ words[SYM], words[VAL] }); }
        else if (idx == PROGRAM) {

            output += "\n" + words[LC] + "\t" + inst_handler(words[INST]);
            if (inst_handler(words[INST]) == no_mc) { output += " NO MC " + no_mc; } 
            else if (i == OPR1)                     { output += "\t0\t0 "; }
            else if (i == OPR2)                     { output += "\t0\t" + operand_handler(words[OPR1]); }
            else if (i == COUNT)                    { output += "\t" + operand_handler(words[OPR1]) + "\t" + operand_handler(words[OPR2]); }
        }
    }

    cout << output << "\n\n";
}

//expected input file format: SYMTAB, LITTAB, PROGRAM, 
// in this specific order, seperated by a single blank line
// if a table is missing, it will be represented as 2 consecutive blank lines
// g++ ./assembler_pass_2.cpp
// ./a.out < test_case_file
