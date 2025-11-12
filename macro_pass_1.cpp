#include <iostream>
#include <sstream>

using namespace std;

enum { MNT, KPDTAB, MDT, PNTAB, PROGRAM, COUNT };

string op[COUNT];
int tab_sizes[COUNT];

#define tab_push(idx, str)                                    \
    op[idx] += "\n" + /*to_string(tab_sizes[idx]) +  "\t" + */str; \
    tab_sizes[idx] += 1
// ------------------------------------------------------------------------------

static inline bool trim(string& s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");
    s            = (start == string::npos) ? "" : s.substr(start, end - start + 1);
    return (start != string::npos);
}

void find_and_replace(const string& param, const string& idx)
{
    size_t pos = 0;
    while ((pos = op[MDT].find(param, pos)) != string::npos) {
        op[MDT].replace(pos, param.length(), idx);
        pos += idx.length();
    }
}

// ------------------------------------------------------------------------------

void process_macro()
{
    string header, buf, name;
    int KPDTP = tab_sizes[KPDTAB];     // keyword parameter definition table ptr
    int MDTP  = tab_sizes[MDT];        // macro definition table ptr
    int KP    = 0;                     // keyword param cnt
    int PP    = 0;                     // positional param cnt

    getline(cin, header);
    trim(header);
    istringstream iss(header);
    getline(iss, name, ' ');

    while (getline(cin, buf) && trim(buf)) {
        tab_push(MDT, buf);
        if (buf.substr(0, 4) == "MEND") { 
            // op[PNTAB] += "\n --- " + name;
            tab_sizes[PNTAB] = 1; 
            break; 
        }
    }

    while (getline(iss, buf, ',') && trim(buf)) {
        size_t pos = buf.find_first_of("=");
        string arg;
        if (pos == string::npos) { PP += 1; arg = buf; }
        else                     { KP += 1; arg = buf.substr(0, pos);
            tab_push(KPDTAB, arg.substr(1) + "\t" + buf.substr(pos + 1));
        }

        find_and_replace(arg, "(P," + to_string(tab_sizes[PNTAB]) + ")");
        tab_push(PNTAB, arg.substr(1));
    }

    tab_push(MNT, name + "\t" + to_string(PP) + "\t" + to_string(KP) + "\t" + to_string(MDTP) + "\t" + to_string(KPDTP));
}

// ------------------------------------------------------------------------------

int main()
{
    string line;

    for (size_t i = 0; i < COUNT; i++) { tab_sizes[i] = 1; }

    // op[MNT]     += "\n MNT";
    // op[MNT]     += "\n\tNAME\t#PP\t#KP\t#MDTP\t#KPDTAB";
    // op[KPDTAB]  += "\n KPDTAB";
    // op[KPDTAB]  += "\n\tNAME\tVALUE";
    // op[MDT]     += "\n MDT";
    // op[PNTAB]   += "\n PNTAB";
    // op[PROGRAM] += "\n PROGRAM";

    while (getline(cin, line) && trim(line)) {
        if (line.substr(0, 5) == "MACRO") { process_macro(); }
        else                              { tab_push(PROGRAM, line); }
    }

    for (size_t i = 0; i < COUNT; i++) { cout << op[i] << "\n\n"; }
}

// g++ ./macro_pass_1.cpp
// ./a.out < test_case_file
