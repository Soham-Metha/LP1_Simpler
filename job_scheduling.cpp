#include <algorithm>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

struct job {
    int ID;
    int AT;
    int BT;
    int PR;

    int CT;
    int TAT;
    int WT;
};

vector<job> disp_tab = {
    { 0, 0, 100, 4 },
    { 1, 0, 90,  6 },
    { 2, 2, 5,   1 }
};

int timestamp = 0;

// -------------------------------------------------

#define update_CT_WT_TAT(idx)                                                \
    {                                                                        \
        disp_tab[idx].CT  = timestamp;                                       \
        disp_tab[idx].TAT = timestamp - disp_tab[idx].AT;                    \
        disp_tab[idx].WT  = timestamp - disp_tab[idx].AT - disp_tab[idx].BT; \
    }

void disp(const vector<job>& jobqueue)
{
    int avgTAT = 0, avgWT = 0;
    cout << "\nID\tAT\tBT\tPR\tCT\tTAT\tWT";
    for (auto entry : jobqueue) {
        cout << "\n" << entry.ID
             << "\t" << entry.AT << "\t" << entry.BT  << "\t" << entry.PR
             << "\t" << entry.CT << "\t" << entry.TAT << "\t" << entry.WT;
        avgTAT += entry.TAT;
        avgWT  += entry.WT;
    }

    cout << "\n\t\t AVG TAT: " << avgTAT / jobqueue.size() << "\t AVG WT: " << avgWT / jobqueue.size() << "\n";
}

// -------------------------------------------------

// shows "IDLE" untill 'job' arrives
#define IDLE(str, job)                                 \
    {                                                  \
        str += "  " + to_string(timestamp) + " IDLE "; \
        timestamp = job.AT;                            \
    }

// exec 'job' for 'etime' amt of time
#define EXEC(str, job, etime)                                                \
    {                                                                        \
        str += "  " + to_string(timestamp) + " P" + to_string(job.ID) + " "; \
        timestamp += etime;                                                  \
        job.BT -= etime;                                                     \
    }

// -------------------------------------------------

bool compare_AT(job A, job B)
{
    return A.AT < B.AT;
}

// -------------------------------------------------

string FCFS(vector<job> jobqueue)
{
    string op = "\n";

    for (int i = 0; i < jobqueue.size(); i++)                { auto& entry = jobqueue[i];
        if (entry.AT > timestamp)                            { IDLE(op, entry); }

        EXEC(op, entry, entry.BT);

        if (entry.BT <= 0)                                   { update_CT_WT_TAT(i);  }
    }

    return op + "  " + to_string(timestamp);
}

string RR(vector<job> jobqueue, int tq)
{
    string op         = "\n";
    int completed_cnt = 0;

round_start:
    for (int i = 0; i < jobqueue.size(); i++)                { auto& entry = jobqueue[i];
        if      (entry.BT <= 0)                              { continue; }
        else if (timestamp < entry.AT && completed_cnt == i) { IDLE(op, entry); }
        else if (timestamp < entry.AT)                       { break; }

        EXEC(op, entry, min(entry.BT, tq));

        if (entry.BT <= 0)                                   { update_CT_WT_TAT(i); completed_cnt += 1; }
    }

    if (completed_cnt < jobqueue.size()) goto round_start;

    return op;
}

// -------------------------------------------------

int main()
{
    sort(disp_tab.begin(), disp_tab.end(), compare_AT);

    timestamp = 0;
    cout << FCFS(disp_tab) << "\n";
    disp(disp_tab);

    timestamp = 0;
    cout << RR(disp_tab, 25) << "\n";
    disp(disp_tab);
}