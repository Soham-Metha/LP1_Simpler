#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct macro_info {
    string NAME;
    string PP;
    string KP;
    string MDTP;
    string KPDTP;
};

vector<pair<string, string>> KPDT;
vector<macro_info> macro_list;
vector<string> MDT;

// ------------------------------------------------------------------------------

static inline bool trim(string& s)
{
    size_t start = s.find_first_not_of(" \t");
    size_t end   = s.find_last_not_of(" \t");
    s            = (start == string::npos) ? "" : s.substr(start, end - start + 1);
    return (start != string::npos);
}

void find_and_replace(string& str, const string& param, const string& idx)
{
    size_t pos = 0;
    while ((pos = str.find(param, pos)) != string::npos) {
        str.replace(pos, param.length(), idx);
        pos += idx.length();
    }
}

// ------------------------------------------------------------------------------

int get_offset(string param, int start, int cnt)
{
    for (size_t i = 0; i < cnt; i++) {
        if (KPDT[start + i - 1].first == param)
            return i;
    }
    return 0;
}

// ------------------------------------------------------------------------------

string handle_macro_call(string line, macro_info MNT_entry)
{
    istringstream iss(line);
    string buf, body = "", op = "";
    int pos_arg_cnt  = stoi(MNT_entry.PP);
    int key_arg_cnt  = stoi(MNT_entry.KP);
    int arg_cnt      = pos_arg_cnt + key_arg_cnt;
    int arg_lut_base = stoi(MNT_entry.KPDTP);
    int def_lut_base = stoi(MNT_entry.MDTP);

    string PNT[arg_cnt];

    getline(iss, buf, ' '); // discard macro name from stream

    for (int offset = 0; offset < key_arg_cnt; offset += 1) { PNT[pos_arg_cnt + offset] = KPDT[arg_lut_base + offset - 1].second; }
    for (int  param = 0;  param < pos_arg_cnt
        && getline(iss, buf, ',') && trim(buf); param += 1) { PNT[param]                = buf; }

    while (getline(iss, buf, ',') && trim(buf)) {
        size_t pos                = buf.find_first_of("=");
        string arg                = (pos == string::npos) ? buf : buf.substr(0, pos);
        string val                = (pos == string::npos) ? ""  : buf.substr(pos + 1);
        int offset                = get_offset(arg.substr(1), arg_lut_base, key_arg_cnt);
        PNT[pos_arg_cnt + offset] = val;
    }

    // for (int i = 0;                    i   < arg_cnt;    i++) { op   += "\n " + to_string(i) + "\t" + PNT[i]; }
    // op += "\n";
    for (int i = def_lut_base - 1; MDT[i] != "MEND"; i++) { body += "\n + " + MDT[i]; }
    for (int i = 0;                    i  < arg_cnt; i++) { find_and_replace(body, "(P," + to_string(i + 1) + ")", PNT[i]); }

    return op + body + "\n";
}

// ------------------------------------------------------------------------------

int main()
{
    string line, output = "";

    while (getline(cin, line) && trim(line)) { istringstream iss(line); macro_info tmp;
        iss >> tmp.NAME >> tmp.PP >> tmp.KP >> tmp.MDTP >> tmp.KPDTP;
        macro_list.push_back(tmp);
    }

    while (getline(cin, line) && trim(line)) { istringstream iss(line); string param, val;
        iss >> param >> val;
        KPDT.push_back({ param, val });
    }

    while (getline(cin, line) && trim(line)) { MDT.push_back(line); }

    while (getline(cin, line) && trim(line)) { /* do nothing for PNTAB ... */ }

    while (getline(cin, line) && trim(line)) { istringstream iss(line); string inst; bool is_macro_call = false;
        iss >> inst;
        for (auto entry : macro_list)
            if (entry.NAME == inst)          { output += handle_macro_call(line, entry);  is_macro_call = true; break; }
        if (!is_macro_call)                  { output += line; }
    }

    cout << output << "\n";
}

// g++ ./macro_pass_2.cpp
// ./a.out < test_case_file