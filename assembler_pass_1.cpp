#include <cctype>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

enum { LABL, INST, OPR1, OPR2, COUNT };

vector<pair<string, int>> sym_table, literal_table;

int pool_pos = 0, LC = 0;

string label_access_handler(const string& name)
{
    size_t pos    = name.find_first_of("+-");
    string base   = (pos == string::npos) ? name : name.substr(0, pos);
    string offset = (pos == string::npos) ? ""   : name.substr(pos);

    size_t i;
    for (i = 0; i < sym_table.size(); i++) {
        if (sym_table[i].first == base)
            return "(S," + to_string(i + 1) + ")" + offset;
    }

    sym_table.push_back({ base, 0 }); 

    return "(S," + to_string(i + 1) + ")" + offset;
}

void labl_decl_handler(string words[COUNT], int PC) 
{
    int addr = PC;

    if (words[INST] == "EQU" || words[INST] == "ORIGIN") {
        size_t pos    = words[OPR1].find_first_of("+-");
        string base   = (pos == string::npos) ? words[OPR1] : words[OPR1].substr(0, pos);
        string offset = (pos == string::npos) ? ""          : words[OPR1].substr(pos);

        for (size_t i = 0; i < sym_table.size(); i++) {
            if (sym_table[i].first == base) {
                if (words[INST] == "EQU") {
                    addr = offset.empty()? sym_table[i].second:sym_table[i].second+stoi(offset);
                } else if (words[INST] == "ORIGIN") {
                    LC   = offset.empty()? sym_table[i].second:sym_table[i].second+stoi(offset);
                }
                break;
            }
        }
    }

    if (words[LABL] == "") return;
    (void)label_access_handler(words[LABL]);
    for (size_t i = 0; i < sym_table.size(); i++) {
        if (sym_table[i].first == words[LABL]) {
            sym_table[i].second = addr;
            break;
        }
    }
}

string imperative_handler(string id) {
    string out = to_string(LC) + "\t(IS," + id + ")";
    LC        += 1;
    return out;
}

string declarative_handler(string id, string opr1) {
    string out = to_string(LC) + "\t(DL," + id + ")";
    if (id == "01") {
        LC += 1;
    } else if (id == "02") {
        LC += stoi(opr1);
    }
    return out;
}

string directive_handler(string id, string opr1)
{
    string out = "\t(AD," + id + ")";
    switch (stoi(id)) {
    case 1:     // START
        LC = stoi(opr1);
        break;
    case 3:     // ORIGIN
    case 4:     // EQU
        break;
    case 2:     // END
    case 5:     // LTORG
        for (size_t i = pool_pos; i < literal_table.size(); i++)
        {
            literal_table[i].second = LC;
            out = out + "\n" + to_string(LC) + "\t(DL,01)\t(C," + literal_table[i].first + ")";
            LC  += 1;
        }
        pool_pos = literal_table.size();
        break;
    }
    return out;
}

string inst_handler(string words[COUNT]) {
    if      (words[INST] == "STOP")    { return imperative_handler("00");}
    else if (words[INST] == "ADD")     { return imperative_handler("01");}
    else if (words[INST] == "SUB")     { return imperative_handler("02");}
    else if (words[INST] == "MULT")    { return imperative_handler("03");}
    else if (words[INST] == "MOVER")   { return imperative_handler("04");}
    else if (words[INST] == "MOVEM")   { return imperative_handler("05");}
    else if (words[INST] == "COMP")    { return imperative_handler("06");}
    else if (words[INST] == "BC")      { return imperative_handler("07");}
    else if (words[INST] == "DIV")     { return imperative_handler("08");}
    else if (words[INST] == "READ")    { return imperative_handler("09");}
    else if (words[INST] == "PRINT")   { return imperative_handler("10");}

    else if (words[INST] == "DC")      { return declarative_handler("01", words[OPR1]);}
    else if (words[INST] == "DS")      { return declarative_handler("02", words[OPR1]);}

    else if (words[INST] == "START")   { return directive_handler("01", words[OPR1]);}
    else if (words[INST] == "END")     { return directive_handler("02", words[OPR1]);}
    else if (words[INST] == "ORIGIN")  { return directive_handler("03", words[OPR1]);}
    else if (words[INST] == "EQU")     { return directive_handler("04", words[OPR1]);}
    else if (words[INST] == "LTORG")   { return directive_handler("05", words[OPR1]);}

    else { return "UNKNOWN INST! \'" + words[INST] + "\'";}
}

string operand_handler(string words[COUNT], int opr_no) {
    if      (words[opr_no].empty())      { return "";}

    else if (words[opr_no] == "AREG")    { return "(1)";}
    else if (words[opr_no] == "BREG")    { return "(2)";}
    else if (words[opr_no] == "CREG")    { return "(3)";}
    else if (words[opr_no] == "DREG")    { return "(4)";}

    else if (words[opr_no] == "LT")      { return "(1)";}
    else if (words[opr_no] == "LE")      { return "(2)";}
    else if (words[opr_no] == "EQ")      { return "(3)";}
    else if (words[opr_no] == "GT")      { return "(4)";}
    else if (words[opr_no] == "GE")      { return "(5)";}
    else if (words[opr_no] == "ANY")     { return "(6)";}

    else if (words[opr_no].at(0) == '=') {
        literal_table.push_back({words[opr_no].substr(2, words[opr_no].size()-3),0}); // discard 2 chars (=') from start & 1 char (') from end
        return "(L," + to_string(literal_table.size()) + ")";
    }

    else if (words[opr_no].at(0) == '\'') {
        return "(C," + words[opr_no].substr(1, words[opr_no].size()-2) + ")"; // discard 1 char (') from both ends
    }

    else if ( words[opr_no].at(0) == '-' || 
            (   words[opr_no].at(0) >= '0'
            &&  words[opr_no].at(0) <= '9' )
        ) {
        return "(C," + words[opr_no] + ")";
    }

    else {
        return label_access_handler(words[opr_no]);
    }
}

int main() {
    string line;
    while (getline(cin, line)) {
        if (line.find_first_not_of(" \t") == string::npos) continue;

        istringstream iss(line);
        string words[COUNT], buf;

        int i     = isspace(line[0]) ? INST : LABL;
        while (i < COUNT && iss >> buf)
            words[i++] = buf;

        labl_decl_handler(words, LC);
        cout << "\n" << inst_handler(words);
        cout << "\t" << operand_handler(words, OPR1);
        cout << "\t" << operand_handler(words, OPR2);
    }

    cout << "\n\n";
    for (size_t i = 0; i < sym_table.size(); i++) {
        cout << sym_table[i].first << "\t" << sym_table[i].second << "\n";
    }
    for (size_t i = 0; i < literal_table.size(); i++) {
        cout << "='" << literal_table[i].first << "'\t" << literal_table[i].second << "\n";
    }
}

// g++ ./assembler_pass_1.cpp
// ./a.out < test_case_file
