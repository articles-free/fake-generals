#include <iostream>
#include <windows.h>
#include <conio.h>
#include <string>
#include <cstring>
#include <ctime>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <set>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <fstream>
#define KEY_DOWN(VK_NONAME) ((GetAsyncKeyState(VK_NONAME) & 0x8000) ? 1 : 0)
using std::cin;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::make_pair;
using std::map;
using std::max;
using std::min;
using std::ofstream;
using std::pair;
using std::queue;
using std::random_shuffle;
using std::set;
using std::sort;
using std::stack;
using std::string;
using std::stringstream;
using std::swap;
using std::vector;
struct node
{
    int belong, tmp, type;
    void tozero()
    {
        belong = tmp = type = 0;
        return;
    }
} mp[105][105];

int X = 15, Y = 15;     //地图的长和宽。如果以迷宫地图或端午地图游玩，请确保地图的长和宽相等
double wallpr = 0.05;   //墙的密度
double citypr = 0.05;   //城市的密度
double objectpr = 0.06; //道具的密度
int tpt = 120;          //每个回合后的等待时间。如果想体验原速，建议设为480
int gennum = 4;         //玩家的数量。当然只有沙雕Bot
int teamnum = 2;        //队伍的数量。如果以TDM模式游玩，请确保玩家数量能被队伍数量整除
int dq = 40;            //毒圈的扩散时间
int kttime = 100;       //空投的投放时间。请保证此变量大于10
int pointstime = 20;    //占领一个据点所需的时间

int dir[4][2] = {{-1, 0}, {0, 1}, {1, 0}, {0, -1}};
int vis[105][105];
bool sight[105][105][105];
int Inteam[105];
int mode, mapmode;
bool teamdead[105];
bool starting = true;
map<int, int> objects, normalobjects;
int blindtimeremain[105];
int ktx, kty;
bool ktiscoming = false;
bool iskt[105][105];
bool ishavets[105];
int ktremaintime = -1;
int randnum(int l, int r)
{
    return rand() % (r - l + 1) + l;
}
set<int> ifteam[105];
int rm, fog[105][105];
int flagscore[105];
bool ifcanconvobject;
bool fvf;
bool isgz;
void convmap()
{
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            mp[i][j].tozero();
    return;
}
void gotoxy(int x, int y)
{
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    HANDLE hConsoleOut;
    hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(hConsoleOut, &csbiInfo);
    csbiInfo.dwCursorPosition.X = x;
    csbiInfo.dwCursorPosition.Y = y;
    SetConsoleCursorPosition(hConsoleOut, csbiInfo.dwCursorPosition);
    return;
}
string myto_string(int x)
{
    stringstream sst;
    sst << x;
    string ans;
    sst >> ans;
    return ans;
}
int order[105];
void congen()
{
    for (int i = 1; i <= gennum; i++)
        order[i] = i;
    random_shuffle(order + 1, order + gennum + 1);
    for (int i = 1; i <= gennum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != 0)
                continue;
            mp[px][py].type = 2;
            mp[px][py].belong = order[i];
            break;
        }
    }
    return;
}
void concit()
{
    int citynum = double(X * Y) * citypr;
    for (int i = 1; i <= citynum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != 0)
                continue;
            mp[px][py].type = 4;
            mp[px][py].tmp = randnum(35, 50);
            break;
        }
    }
    return;
}
void dfs(int cnt, int x, int y)
{
    vis[x][y] = cnt;
    for (int i = 0; i < 4; i++)
    {
        int px = x + dir[i][0], py = y + dir[i][1];
        if (px >= 1 && px <= X && py >= 1 && py <= Y && mp[px][py].type != 1 && !vis[px][py])
        {
            dfs(cnt, px, py);
        }
    }
    return;
}
int objectnum, aliveobjectnum;
bool checkwall()
{
    memset(vis, 0, sizeof(vis));
    int cnt = 0, tp = 0;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type != 1 && !vis[i][j])
            {
                cnt++;
                dfs(cnt, i, j);
            }
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == 2)
            {
                if (tp == 0)
                    tp = vis[i][j];
                else if (vis[i][j] != tp)
                    return false;
            }
    return true;
}
void convwall()
{
    int wallnum = double(X * Y) * wallpr;
    for (int i = 1; i <= wallnum; i++)
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != 0)
                continue;
            mp[px][py].type = 1;
            if (checkwall())
                break;
            mp[px][py].type = 0;
        }
    }
    return;
}
int etot, vtot, venum[105][105], id[1000005];
int playermaxhp[105];
struct edge
{
    int a, b, w, posa, posb;
} edges[1000005];
bool cmpe(const edge &s1, const edge &s2)
{
    return s1.w < s2.w;
}
int find(int x)
{
    if (x == id[x])
        return x;
    return id[x] = find(id[x]);
}
void convmaze()
{
    convmap();
    etot = vtot = 0;
    memset(venum, 0, sizeof(venum));
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (i % 2 == 0 && j % 2 == 0)
                mp[i][j].type = 1;
            else if (i % 2 == 1 && j % 2 == 1)
                venum[i][j] = vtot++;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
        {
            int tmp1 = i - 1, tmp3 = j - 1, tmp4 = j + 1, tmp2 = i + 1;
            if (i % 2 == 0 && j % 2 == 1)
            {
                venum[i][j] = etot;
                edges[etot] = (edge){venum[tmp1][j], venum[tmp2][j], 10 + randnum(1, 10), i, j};
                ++etot;
            }
            if (i % 2 == 1 && j % 2 == 0)
            {
                venum[i][j] = etot;
                edges[etot] = (edge){venum[i][tmp3], venum[i][tmp4], 10 + randnum(1, 10), i, j};
                ++etot;
            }
        }
    sort(edges, edges + etot, cmpe);
    for (int i = 0; i < vtot; i++)
        id[i] = i;
    for (int i = 0; i < etot; i++)
    {
        if (find(edges[i].a) != find(edges[i].b))
        {
            id[find(edges[i].a)] = id[(edges[i].b)];
            mp[edges[i].posa][edges[i].posb].type = 4;
            mp[edges[i].posa][edges[i].posb].tmp = 10;
        }
        else
        {
            mp[edges[i].posa][edges[i].posb].type = 1;
        }
    }
    int calcTimes = 0;
    for (int i = 1; i <= gennum; i++)
    {
        ++calcTimes;
        if (calcTimes >= 100)
        {
            convmaze();
            return;
        }
        int t1 = randnum(1, X), t2 = randnum(1, X);
        while (1)
        {
            t1 = randnum(1, X), t2 = randnum(1, X);
            int tmpcnt = 0;
            if (t1 - 1 >= 1)
            {
                if (mp[t1 - 1][t2].type != 1)
                {
                    tmpcnt++;
                }
            }
            if (t2 - 1 >= 1)
            {
                if (mp[t1][t2 - 1].type != 1)
                {
                    tmpcnt++;
                }
            }
            if (t1 + 1 <= X)
            {
                if (mp[t1 + 1][t2].type != 1)
                {
                    tmpcnt++;
                }
            }
            if (t2 + 1 <= X)
            {
                if (mp[t1][t2 + 1].type != 1)
                {
                    tmpcnt++;
                }
            }
            if (mp[t1][t2].type == 0 && tmpcnt == 1)
                break;
        }
        mp[t1][t2].belong = i;
        mp[t1][t2].type = 2;
    }
    for (int i = 1; i <= (X * Y) / 15; ++i)
    {
        int tryTime = 0;
        while (true)
        {
            ++tryTime;
            int x = randnum(1, X), y = randnum(1, X);
            if (tryTime >= 20)
            {
                break;
            }
            int flag = 0;
            for (int t1 = -1; t1 <= 1; ++t1)
            {
                for (int t2 = -1; t2 <= 1; ++t2)
                {
                    if (t1 == 0 && t2 == 0)
                        continue;
                    if (x + t1 > 0 && x + t1 <= X && y + t2 <= X)
                    {
                        if (mp[x + t1][y + t2].type == 2)
                        {
                            flag = 1;
                            break;
                        }
                    }
                }
                if (flag)
                    break;
            }
            if (flag || x % 2 == y % 2)
                continue;
            if (mp[x][y].type == 1)
            {
                mp[x][y].type = 4;
                mp[x][y].tmp = 10;
                break;
            }
        }
    }
    return;
}
int viss[105][105];
struct point
{
    int a, b, c;
};
int Astar(int x, int y, int tar_x, int tar_y)
{
    memset(viss, 0, sizeof(viss));
    queue<point> q;
    q.push((point){x, y, 0});
    viss[x][y] = 1;
    while (!q.empty())
    {
        int tx = q.front().a, ty = q.front().b, step = q.front().c;
        q.pop();
        for (int j = 0; j < 4; ++j)
        {
            int tx2 = tx + dir[j][0], ty2 = ty + dir[j][1];
            if (tx2 > X || ty2 > X || tx2 <= 0 || ty2 <= 0 || mp[tx2][ty2].type == 1 || viss[tx2][ty2])
                continue;
            viss[tx2][ty2] = 1;
            q.push((point){tx2, ty2, step + 1});
            if (tx2 == tar_x && ty2 == tar_y)
                return step + 1;
        }
    }
    return -1;
}
struct node2
{
    int a, b;
};
void convdragon()
{
    convmap();
    vector<node2> lst;
    int calcTimes = 0;
    for (int i = 1; i <= gennum; i++)
    {
        ++calcTimes;
        if (calcTimes >= 100)
        {
            convdragon();
            return;
        }
        int t1 = randnum(1, X - 2) + 1, t2 = randnum(1, X - 2) + 1;
        while (mp[t1][t2].type != 0 || (mp[t1 + 1][t2].type != 0 && mp[t1 - 1][t2].type != 0 && mp[t1][t2 + 1].type != 0 && mp[t1][t2 + 1].type != 0))
            t1 = randnum(1, X - 2) + 1, t2 = randnum(1, X - 2) + 1;
        if (i == 1)
        {
            mp[t1][t2].belong = i;
            mp[t1][t2].tmp = 100;
            mp[t1][t2].type = 2;
        }
        else
        {
            int flag = 0;
            for (int j = 0; j < (int)lst.size(); ++j)
            {
                if (Astar(t1, t2, lst[j].a, lst[j].b) > 6)
                {
                    continue;
                }
                flag = 1;
                --i;
                break;
            }
            if (flag == 0)
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 100;
                mp[t1][t2].type = 2;
            }
        }
        lst.push_back((node2){t1, t2});
    }
    for (int i = 1; i <= gennum; ++i)
    {
        for (int j = 1; j <= min(X * X / gennum / 10, 8); ++j)
        {
            int t1 = randnum(1, X - 2) + 1, t2 = randnum(1, X - 2) + 1;
            while (mp[t1][t2].belong != 0 || mp[t1][t2].type != 0)
            {
                t1 = randnum(1, X - 2) + 1;
                t2 = randnum(1, X - 2) + 1;
            }
            if (j == 1)
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 50;
                mp[t1][t2].type = 5;
            }
            else
            {
                mp[t1][t2].belong = i;
                mp[t1][t2].tmp = 5;
                mp[t1][t2].type = 3;
            }
        }
    }
    return;
}
int nowpri;
void SetColor(int ForeColor, int BackGroundColor, int pri)
{
    if (nowpri != 0 && pri < nowpri)
        return;
    nowpri = pri;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
    return;
}
void Setcolor(int ForeColor = 15, int BackGroundColor = 0)
{
    nowpri = 0;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon, ForeColor | BackGroundColor);
    return;
}
int cls[11] = {11, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
int foglevel;
struct News
{
    int opt, a, b, remtime;
} news[10005];
int newsl, newsr;
void displaynews()
{
    while (newsr - newsl > 5)
        newsl++;
    for (int i = newsl; i < newsr; i++)
    {
        if (news[i].opt == 1)
            printf("player%d拿到了team%d的旗帜\n", news[i].a, news[i].b);
        else if (news[i].opt == 2)
            printf("team%d的旗帜被重置了\n", news[i].a);
        else if (news[i].opt == 3)
            printf("player%d为team%d得分了\n", news[i].a, Inteam[news[i].a]);
        else if (news[i].opt == 4)
            printf("team%d占领了一个据点\n", news[i].a);
        else if (news[i].opt == 5)
            printf("team%d失去了一个据点\n", news[i].a);
        news[i].remtime--;
        if (news[i].remtime <= 0)
            newsl++;
    }
    return;
}
struct scoreboard
{
    int sco, id, lnd;
} score[105];
bool cmpsco(const scoreboard &s1, const scoreboard &s2)
{
    if (s1.sco != s2.sco)
        return s1.sco > s2.sco;
    return s1.id < s2.id;
}
int wd[105];
int ifgetflag[105];
void getnum(int x)
{
    if (x < 100)
        printf("%2d", x);
    else if (x >= 100 && x < 950)
        printf("%dH", int(round(double(x) / 100.0)));
    else if (x >= 950 && x < 9500)
        printf("%dK", int(round(double(x) / 1000.0)));
    else
        printf("9K");
    return;
}
int playeratk[105], playerac[105], playerfh[105];
int teampointsmatchscore[105], teampointsmatchland[105];
struct teamscore
{
    int score, pos;
} nflagscore[105];
bool cmpteam(const teamscore &s1, const teamscore &s2)
{
    return s1.score > s2.score;
}
void putmap(int sx, int sy, int id)
{
    gotoxy(0, 0);
    memset(score, 0, sizeof(score));
    if (!sight[id][sx][sy])
        sx = sy = 0;
    for (int i = 1; i <= gennum; i++)
        score[i].id = i;
    bool lft = false;
    bool lineprinted, colprinted = false;
    for (int i = (starting ? 1 : (X > 15 ? sx - 7 : 1)); i <= (starting ? 15 : (X > 15 ? sx + 7 : X)); i++)
    {
        lineprinted = false;
        if (i >= 1 && i <= X && !colprinted)
        {
            lft = false;
            for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
                if (j >= 1 && j <= Y)
                {
                    if (!lft)
                        printf("-"), lft = true;
                    printf("-----");
                }
                else
                {
                    if (!lft)
                        printf(" "), lft = true;
                    printf("     ");
                }
            printf("\n");
            colprinted = true;
        }
        for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
        {
            if (!lineprinted && (i < 1 || i > X))
                printf(" "), lineprinted = true;
            if (i < 1 || i > X || j < 1 || j > Y)
            {
                printf("     ");
                continue;
            }
            if (!lineprinted && i >= 1 && i <= X)
                printf("|"), lineprinted = true;
            if (fog[i][j])
                SetColor(0xd, 0xd, 2);
            if (sx == i && sy == j)
            {
                SetColor(0xc, 0, 1);
                if (mp[i][j].type == 0)
                {
                    if (iskt[i][j])
                        printf("▒▒▒▒");
                    else if (fog[i][j])
                        printf("████");
                    else
                        printf("    ");
                }
                else if (mp[i][j].type == 1)
                    printf("####");
                else if (mp[i][j].type == 2)
                {
                    if (ifgetflag[mp[i][j].belong])
                    {
                        printf("<");
                        getnum(mp[i][j].tmp);
                        printf(">");
                    }
                    else
                    {
                        if (mapmode == 7 || mapmode == 6 || mapmode == 5)
                            SetColor(cls[Inteam[mp[i][j].belong] % 11], 0, 100);
                        printf("{");
                        getnum(mp[i][j].tmp);
                        printf("}");
                    }
                }
                else if (mp[i][j].type == 3)
                {
                    printf(" ");
                    getnum(mp[i][j].tmp);
                    printf(" ");
                }
                else if (mp[i][j].type == 4)
                {
                    printf("[");
                    getnum(mp[i][j].tmp);
                    printf("]");
                }
                else if (mp[i][j].type == 5)
                {
                    printf("[");
                    getnum(mp[i][j].tmp);
                    printf("]");
                }
                else if (mp[i][j].type == 6)
                    printf(" +  ");
                else if (mp[i][j].type == 8)
                {
                    printf("<--l");
                }
                else if (mp[i][j].type == 7)
                {
                    printf("[O] ");
                }
                else if (mp[i][j].type == 9)
                    printf("( %d)", mp[i][j].belong);
                else if (mp[i][j].type == 10)
                    printf("(   )");
                else if (mp[i][j].type == 11)
                    printf("{+} ");
                else if (mp[i][j].type == 12)
                    printf("{O} ");
                else if (mp[i][j].type == 13)
                    printf("<==I");
                else if (mp[i][j].type == 14)
                    printf("[L] ");
                else if (mp[i][j].type == 15)
                    printf("[C] ");
                else if (mp[i][j].type == 16)
                    printf("{C} ");
                else if (mp[i][j].type == 17)
                    printf("{2X}");
                else if (mp[i][j].type == 18)
                    printf("[F]");
                else if (mp[i][j].type == 19)
                    printf("{F}");
                Setcolor();
            }
            else
            {
                if (mp[i][j].type == 0)
                {
                    if (iskt[i][j])
                        printf("▒▒▒▒");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 1)
                {
                    if (sight[id][i][j])
                        printf("####");
                    else
                        printf("????");
                }
                else if (mp[i][j].type == 2)
                {
                    if (ifgetflag[mp[i][j].belong])
                    {
                        SetColor(cls[mp[i][j].belong % 11], 0, 1);
                        printf("<");
                        getnum(mp[i][j].tmp);
                        printf(">");
                        Setcolor();
                    }
                    else if (sight[id][i][j])
                    {
                        SetColor(cls[mp[i][j].belong % 11], 0, 1);
                        if (mapmode == 7 || mapmode == 6 || mapmode == 5)
                            SetColor(cls[Inteam[mp[i][j].belong] % 11], 0, 100);
                        printf("{");
                        getnum(mp[i][j].tmp);
                        printf("}");
                        Setcolor();
                    }
                    else
                    {
                        if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j])
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                }
                else if (mp[i][j].type == 3)
                {
                    if (sight[id][i][j])
                    {
                        SetColor(cls[mp[i][j].belong % 11], 0, 1);
                        printf(" ");
                        getnum(mp[i][j].tmp);
                        printf(" ");
                        Setcolor();
                    }
                    else
                    {
                        if (fog[i][j])
                            printf("████");
                        else if (sight[id][i][j])
                            printf("    ");
                        else
                        {
                            SetColor(15, 0, 1);
                            printf("████");
                            Setcolor();
                        }
                    }
                }
                else if (mp[i][j].type == 4)
                {
                    if (sight[id][i][j])
                    {
                        printf("[");
                        getnum(mp[i][j].tmp);
                        printf("]");
                    }
                    else
                        printf("????");
                }
                else if (mp[i][j].type == 5)
                {
                    if (sight[id][i][j])
                    {
                        SetColor(cls[mp[i][j].belong % 11], 0, 1);
                        printf("[");
                        getnum(mp[i][j].tmp);
                        printf("]");
                        Setcolor();
                    }
                    else
                        printf("????");
                }
                else if (mp[i][j].type == 6)
                {
                    if (sight[id][i][j])
                        printf(" +  ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 8)
                {
                    if (sight[id][i][j])
                        printf("<--l");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 7)
                {
                    if (sight[id][i][j])
                        printf("[O] ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 9)
                    printf("( %d)", mp[i][j].belong);
                else if (mp[i][j].type == 10)
                    printf("(  )");
                else if (mp[i][j].type == 11)
                {
                    if (sight[id][i][j])
                        printf("{+} ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 12)
                {
                    if (sight[id][i][j])
                        printf("{O} ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 13)
                {
                    if (sight[id][i][j])
                        printf("<==I");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 14)
                {
                    if (sight[id][i][j])
                        printf("[L] ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 15)
                {
                    if (sight[id][i][j])
                        printf("[C] ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 16)
                {
                    if (sight[id][i][j])
                        printf("{C} ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 17)
                {
                    if (sight[id][i][j])
                        printf("{2X}");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 18)
                {
                    if (sight[id][i][j])
                        printf("[F] ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 19)
                {
                    if (sight[id][i][j])
                        printf("{F} ");
                    else if (fog[i][j])
                        printf("████");
                    else if (sight[id][i][j])
                        printf("    ");
                    else
                    {
                        SetColor(15, 0, 1);
                        printf("████");
                        Setcolor();
                    }
                }
                else if (mp[i][j].type == 20)
                {
                    if (mp[i][j].belong)
                        SetColor(cls[mp[i][j].belong % 11], 0, 100);
                    printf("[");
                    getnum(mp[i][j].tmp);
                    printf("]");
                    if (mp[i][j].belong)
                        Setcolor();
                }
            }
            if (fog[i][j])
                Setcolor();
            printf("|");
        }
        printf("\n");
        lft = false;
        for (int j = (starting ? 1 : (Y > 15 ? sy - 7 : 1)); j <= (starting ? 15 : (Y > 15 ? sy + 7 : Y)); j++)
            if (i >= 1 && i <= X && j >= 1 && j <= Y)
            {
                if (!lft)
                    printf("-"), lft = true;
                printf("-----");
            }
            else
            {
                if (!lft)
                    printf(" "), lft = true;
                printf("     ");
            }
        printf("\n");
    }
    if (starting)
        return;
    gotoxy(0, 2 * ((starting ? 15 : (X > 15 ? min(sx + 7, X) : X)) - (starting ? 1 : (X > 15 ? sx - 7 : 1)) + 1) + 1);
    for (int i = 1; i <= 9; i++)
        printf("                                                                                                    \n");
    gotoxy(0, 2 * ((starting ? 15 : (X > 15 ? min(sx + 7, X) : X)) - (starting ? 1 : (X > 15 ? sx - 7 : 1)) + 1) + 1);
    if (mapmode != 7)
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].type == 2 || mp[i][j].type == 3 || mp[i][j].type == 5)
                {
                    score[mp[i][j].belong].sco += mp[i][j].tmp;
                    score[mp[i][j].belong].lnd++;
                }
                else
                    ;
    else
    {
        for (int i = 1; i <= teamnum; i++)
            score[i].id = i, score[i].sco = teampointsmatchscore[i], score[i].lnd = teampointsmatchland[i];
    }
    if (fvf && !(mapmode == 7 || mapmode == 6 || mapmode == 5))
    {
        sort(score + 1, score + gennum + 1, cmpsco);
        bool visplayer[105];
        memset(visplayer, 0, sizeof(visplayer));
        for (int i = sx - 7; i <= sx + 7; i++)
            for (int j = sy - 7; j <= sy + 7; j++)
                if (i >= 1 && i <= X && j >= 1 && j <= Y && mp[i][j].belong != 0 && mp[i][j].tmp > 0 && sight[id][i][j])
                    visplayer[mp[i][j].belong] = true;
        for (int i = 1, j = 1; i <= gennum && j <= 5; i++)
            if (visplayer[score[i].id])
            {
                SetColor(cls[score[i].id % 11], 0, 1);
                printf("player%d    team%d\n", score[i].id, Inteam[score[i].id]);
                Setcolor();
                j++;
            }
    }
    if (mapmode != 6)
    {
        if (!fvf)
            sort(score + 1, score + gennum + 1, cmpsco);
        if (mapmode == 7)
        {
            for (int i = 1; i <= teamnum; i++)
            {
                SetColor(cls[score[i].id % 11], 0, 1);
                printf("team%d    %d %d\n", score[i].id, score[i].sco, score[i].lnd);
                Setcolor();
            }
        }
        else
        {
            int team1 = 0, team2 = 0;
            for (int i = 1; i <= gennum; i++)
            {
                if (!fvf)
                {
                    SetColor(cls[score[i].id % 11], 0, 1);
                    printf("player%d    %d %d", score[i].id, score[i].sco, score[i].lnd);
                    if (mode == 2)
                        printf("    team%d", Inteam[score[i].id]);
                    printf("\n");
                    Setcolor();
                }
                else
                {
                    if (score[i].sco > 0 && Inteam[score[i].id] == 1)
                        team1++;
                    else if (score[i].sco > 0 && Inteam[score[i].id] == 2)
                        team2++;
                }
            }
            if (fvf)
            {
                SetColor(cls[1], 0, 1);
                printf("team1    %d\n", team1);
                Setcolor();
                SetColor(cls[2], 0, 1);
                printf("team2    %d\n", team2);
                Setcolor();
            }
        }
    }
    else
    {
        for (int i = 1; i <= teamnum; i++)
            nflagscore[i].pos = i, nflagscore[i].score = flagscore[i];
        sort(nflagscore + 1, nflagscore + teamnum + 1, cmpteam);
        for (int i = 1; i <= teamnum; i++)
        {
            SetColor(cls[nflagscore[i].pos % 11], 0, 1);
            printf("team%d    %d\n", nflagscore[i].pos, nflagscore[i].score);
            Setcolor();
        }
    }
    if (mapmode == 5 || mapmode == 6 || mapmode == 7)
    {
        if (mapmode == 5)
            printf("毒圈还有 %d 回合扩散\n", rm);
        printf("当前玩家拥有的物品：剑%d， 护盾%d， 防毒面具%d\n", playeratk[id], playerac[id], playerfh[id]);
        if (ktremaintime > 0)
            printf("空投还有 %d 回合落地\n", ktremaintime);
        if (mapmode == 6 || mapmode == 7)
        {
            if (wd[id] != -1)
                printf("无敌时间还剩 %d 回合\n", wd[id]);
            displaynews();
        }
    }
    if (isgz)
        printf("您已阵亡，观战中……\n");
    return;
}
int alivegennum = gennum, aliveteamnum = teamnum;
struct Flg
{
    int sx, sy, belong;
    void conv()
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != 0)
                continue;
            mp[px][py].type = 9;
            mp[px][py].belong = belong;
            sx = px;
            sy = py;
            break;
        }
        return;
    }
} flg[105];
struct Player
{
    int selectedx, selectedy, playerid;
    bool halfselect, isbot;
    int sx, sy;
    int inteam;
    bool flag[105][105];
    queue<pair<int, int> > q;
    void botit()
    {
        sx = selectedx, sy = selectedy;
        q.push(make_pair(sx, sy));
        memset(flag, 0, sizeof(flag));
        flag[sx][sy] = true;
        return;
    }
    void respawn()
    {
        playerac[playerid] = playeratk[playerid] = playerfh[playerid] = 0;
        playermaxhp[playerid] = 100;
        ishavets[playerid] = false;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
            {
                if (mp[i][j].type == 2 && mp[i][j].belong == playerid)
                {
                    mp[i][j].tozero();
                    if (ifgetflag[playerid])
                    {
                        news[newsr].a = flg[ifgetflag[playerid]].belong;
                        news[newsr].opt = 2;
                        news[newsr].remtime = 50;
                        newsr++;
                        mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = 9, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                        ifgetflag[playerid] = 0;
                    }
                    int px, py;
                    while (1)
                    {
                        px = randnum(1, X), py = randnum(1, Y);
                        if (mp[px][py].type != 0 || (px == i && py == j))
                            continue;
                        mp[px][py].type = 2;
                        mp[px][py].belong = playerid;
                        mp[px][py].tmp = 100;
                        selectedx = px;
                        selectedy = py;
                        if (isbot)
                            botit();
                        break;
                    }
                    i = X + 1;
                    wd[playerid] = 10;
                    break;
                }
            }
        return;
    }
    void kil(int pid)
    {
        if (mode != 2)
        {
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].belong == pid)
                        mp[i][j].belong = playerid;
            alivegennum--;
            return;
        }
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == pid && mp[i][j].type == 2)
                    return;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == pid)
                    mp[i][j].belong = playerid;
        alivegennum--;
        return;
    }
    void moveup()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx - 1][selectedy].type != 1 && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx - 1][selectedy];
            if (mode == 2 && (dt->type == 2 || dt->type == 3 || dt->type == 5) && dt->belong != playerid && ifteam[Inteam[playerid]].find(dt->belong) != ifteam[Inteam[playerid]].end())
                return;
            if (mapmode == 5 && iskt[selectedx - 1][selectedy])
                return;
            if (dt->type == 20)
                return;
            int moved = (dp->tmp) - 1;
            if (mapmode != 5 && mapmode != 6 && mapmode != 7)
            {
                if (halfselect)
                    moved >>= 1, halfselect = false;
                dp->tmp -= moved;
                if (dt->belong == playerid)
                    dt->tmp += moved;
                else
                {
                    if (moved > dt->tmp)
                    {
                        dt->tmp = moved - dt->tmp;
                        if (dt->type == 2)
                            dt->type = 5, kil(dt->belong);
                        else
                        {
                            dt->belong = playerid;
                            if (dt->type == 0)
                                dt->type = 3;
                            if (dt->type == 4)
                                dt->type = 5;
                        }
                    }
                    else
                        dt->tmp -= moved;
                }
                selectedx--;
            }
            else
            {
                if (dt->type == 2)
                {
                    int dmg1 = max(0, int(double(dp->tmp) * (1.0 + double(playeratk[playerid]) / 10.0) - double(playerac[dt->belong]) * 10.0));
                    int dmg2 = max(0, int(double(dt->tmp) * (1.0 + double(playeratk[dt->belong]) / 10.0) - double(playerac[playerid]) * 10.0));
                    dp->tmp -= dmg2;
                    dt->tmp -= dmg1;
                    if (dp->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dp->type = 0, dp->tmp = 0, dp->belong = 0, alivegennum--, playeratk[dt->belong] += playeratk[dp->belong], playerac[dt->belong] += playerac[dp->belong], ishavets[dt->belong] &= ishavets[dp->belong], playerfh[dt->belong] += playerfh[dp->belong];
                    if (dt->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dt->type = 0, dt->tmp = 0, dt->belong = 0, alivegennum--, playeratk[dp->belong] += playeratk[dt->belong], playerac[dp->belong] += playerac[dt->belong], ishavets[dp->belong] &= ishavets[dt->belong], playerfh[dp->belong] += playerfh[dt->belong];
                }
                else
                {
                    if (dt->type == 6)
                        dp->tmp += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 7)
                        playerac[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 8)
                        playeratk[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 11)
                        dp->tmp += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 12)
                        playerac[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 13)
                        playeratk[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 14)
                        blindtimeremain[dp->belong] = 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 15)
                        playermaxhp[dp->belong] += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 16)
                        playermaxhp[dp->belong] += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 17)
                        ishavets[dp->belong] = true, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 18)
                        playerfh[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 19)
                        playerfh[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 9)
                    {
                        if (dt->belong != Inteam[playerid] && !ifgetflag[playerid])
                        {
                            ifgetflag[playerid] = dt->belong;
                            dt->type = 10;
                            news[newsr].a = playerid;
                            news[newsr].b = dt->belong;
                            news[newsr].opt = 1;
                            news[newsr].remtime = 50;
                            newsr++;
                        }
                        else if (dt->belong == Inteam[playerid] && selectedx - 1 == flg[dt->belong].sx && selectedy == flg[dt->belong].sy && ifgetflag[playerid])
                        {
                            mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = 9, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                            flagscore[Inteam[playerid]]++;
                            news[newsr].a = playerid;
                            news[newsr].opt = 3;
                            news[newsr].remtime = 50;
                            newsr++;
                            ifgetflag[playerid] = 0;
                        }
                        return;
                    }
                    else if (dt->type == 10)
                        return;
                    swap(*dt, *dp);
                }
                selectedx--;
            }
        }
        return;
    }
    void movedown()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx + 1][selectedy].type != 1 && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx + 1][selectedy];
            if (mode == 2 && (dt->type == 2 || dt->type == 3 || dt->type == 5) && dt->belong != playerid && ifteam[Inteam[playerid]].find(dt->belong) != ifteam[Inteam[playerid]].end())
                return;
            if (mapmode == 5 && iskt[selectedx + 1][selectedy])
                return;
            if (dt->type == 20)
                return;
            int moved = dp->tmp - 1;
            if (mapmode != 5 && mapmode != 6 && mapmode != 7)
            {
                if (halfselect)
                    moved >>= 1, halfselect = false;
                dp->tmp -= moved;
                if (dt->belong == playerid)
                    dt->tmp += moved;
                else
                {
                    if (moved > dt->tmp)
                    {
                        dt->tmp = moved - dt->tmp;
                        if (dt->type == 2)
                            dt->type = 5, kil(dt->belong);
                        else
                        {
                            dt->belong = playerid;
                            if (dt->type == 0)
                                dt->type = 3;
                            if (dt->type == 4)
                                dt->type = 5;
                        }
                    }
                    else
                        dt->tmp -= moved;
                }
                selectedx++;
            }
            else
            {
                if (dt->type == 2)
                {
                    int dmg1 = max(0, int(double(dp->tmp) * (1.0 + double(playeratk[playerid]) / 10.0) - double(playerac[dt->belong]) * 10.0));
                    int dmg2 = max(0, int(double(dt->tmp) * (1.0 + double(playeratk[dt->belong]) / 10.0) - double(playerac[playerid]) * 10.0));
                    dp->tmp -= dmg2;
                    dt->tmp -= dmg1;
                    if (dp->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dp->type = 0, dp->tmp = 0, dp->belong = 0, alivegennum--, playeratk[dt->belong] += playeratk[dp->belong], playerac[dt->belong] += playerac[dp->belong], ishavets[dt->belong] &= ishavets[dp->belong], playerfh[dt->belong] += playerfh[dp->belong];
                    if (dt->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dt->type = 0, dt->tmp = 0, dt->belong = 0, alivegennum--, playeratk[dp->belong] += playeratk[dt->belong], playerac[dp->belong] += playerac[dt->belong], ishavets[dp->belong] &= ishavets[dt->belong], playerfh[dp->belong] += playerfh[dt->belong];
                }
                else
                {
                    if (dt->type == 6)
                        dp->tmp += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 7)
                        playerac[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 8)
                        playeratk[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 11)
                        dp->tmp += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 12)
                        playerac[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 13)
                        playeratk[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 14)
                        blindtimeremain[dp->belong] = 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 15)
                        playermaxhp[dp->belong] += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 16)
                        playermaxhp[dp->belong] += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 17)
                        ishavets[dp->belong] = true, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 18)
                        playerfh[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 19)
                        playerfh[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 9)
                    {
                        if (dt->belong != Inteam[playerid] && !ifgetflag[playerid])
                        {
                            ifgetflag[playerid] = dt->belong;
                            dt->type = 10;
                            news[newsr].a = playerid;
                            news[newsr].b = dt->belong;
                            news[newsr].opt = 1;
                            news[newsr].remtime = 50;
                            newsr++;
                        }
                        else if (dt->belong == Inteam[playerid] && selectedx + 1 == flg[dt->belong].sx && selectedy == flg[dt->belong].sy && ifgetflag[playerid])
                        {
                            mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = 9, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                            flagscore[Inteam[playerid]]++;
                            news[newsr].a = playerid;
                            news[newsr].opt = 3;
                            news[newsr].remtime = 50;
                            newsr++;
                            ifgetflag[playerid] = 0;
                        }
                        return;
                    }
                    else if (dt->type == 10)
                        return;
                    swap(*dt, *dp);
                }
                selectedx++;
            }
        }
        return;
    }
    void moveleft()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx][selectedy - 1].type != 1 && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx][selectedy - 1];
            if (mode == 2 && (dt->type == 2 || dt->type == 3 || dt->type == 5) && dt->belong != playerid && ifteam[Inteam[playerid]].find(dt->belong) != ifteam[Inteam[playerid]].end())
                return;
            if (mapmode == 5 && iskt[selectedx][selectedy - 1])
                return;
            if (dt->type == 20)
                return;
            int moved = dp->tmp - 1;
            if (mapmode != 5 && mapmode != 6 && mapmode != 7)
            {
                if (halfselect)
                    moved >>= 1, halfselect = false;
                dp->tmp -= moved;
                if (dt->belong == playerid)
                    dt->tmp += moved;
                else
                {
                    if (moved > dt->tmp)
                    {
                        dt->tmp = moved - dt->tmp;
                        if (dt->type == 2)
                            dt->type = 5, kil(dt->belong);
                        else
                        {
                            dt->belong = playerid;
                            if (dt->type == 0)
                                dt->type = 3;
                            if (dt->type == 4)
                                dt->type = 5;
                        }
                    }
                    else
                        dt->tmp -= moved;
                }
                selectedy--;
            }
            else
            {
                if (dt->type == 2)
                {
                    int dmg1 = max(0, int(double(dp->tmp) * (1.0 + double(playeratk[playerid]) / 10.0) - double(playerac[dt->belong]) * 10.0));
                    int dmg2 = max(0, int(double(dt->tmp) * (1.0 + double(playeratk[dt->belong]) / 10.0) - double(playerac[playerid]) * 10.0));
                    dp->tmp -= dmg2;
                    dt->tmp -= dmg1;
                    if (dp->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dp->type = 0, dp->tmp = 0, dp->belong = 0, alivegennum--, playeratk[dt->belong] += playeratk[dp->belong], playerac[dt->belong] += playerac[dp->belong], ishavets[dt->belong] &= ishavets[dp->belong], playerfh[dt->belong] += playerfh[dp->belong];
                    if (dt->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dt->type = 0, dt->tmp = 0, dt->belong = 0, alivegennum--, playeratk[dp->belong] += playeratk[dt->belong], playerac[dp->belong] += playerac[dt->belong], ishavets[dp->belong] &= ishavets[dt->belong], playerfh[dp->belong] += playerfh[dt->belong];
                }
                else
                {
                    if (dt->type == 6)
                        dp->tmp += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 7)
                        playerac[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 8)
                        playeratk[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 11)
                        dp->tmp += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 12)
                        playerac[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 13)
                        playeratk[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 14)
                        blindtimeremain[dp->belong] = 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 15)
                        playermaxhp[dp->belong] += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 16)
                        playermaxhp[dp->belong] += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 17)
                        ishavets[dp->belong] = true, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 18)
                        playerfh[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 19)
                        playerfh[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 9)
                    {
                        if (dt->belong != Inteam[playerid] && !ifgetflag[playerid])
                        {
                            ifgetflag[playerid] = dt->belong;
                            dt->type = 10;
                            news[newsr].a = playerid;
                            news[newsr].b = dt->belong;
                            news[newsr].opt = 1;
                            news[newsr].remtime = 50;
                            newsr++;
                        }
                        else if (dt->belong == Inteam[playerid] && selectedx == flg[dt->belong].sx && selectedy - 1 == flg[dt->belong].sy && ifgetflag[playerid])
                        {
                            mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = 9, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                            flagscore[Inteam[playerid]]++;
                            news[newsr].a = playerid;
                            news[newsr].opt = 3;
                            news[newsr].remtime = 50;
                            newsr++;
                            ifgetflag[playerid] = 0;
                        }
                        return;
                    }
                    else if (dt->type == 10)
                        return;
                    swap(*dt, *dp);
                }
                selectedy--;
            }
        }
        return;
    }
    void moveright()
    {
        if (mp[selectedx][selectedy].belong == playerid && mp[selectedx][selectedy + 1].type != 1 && mp[selectedx][selectedy].tmp > 1)
        {
            node *dp = &mp[selectedx][selectedy], *dt = &mp[selectedx][selectedy + 1];
            if (mode == 2 && (dt->type == 2 || dt->type == 3 || dt->type == 5) && dt->belong != playerid && ifteam[Inteam[playerid]].find(dt->belong) != ifteam[Inteam[playerid]].end())
                return;
            if (mapmode == 5 && iskt[selectedx][selectedy + 1])
                return;
            if (dt->type == 20)
                return;
            int moved = dp->tmp - 1;
            if (mapmode != 5 && mapmode != 6 && mapmode != 7)
            {
                if (halfselect)
                    moved >>= 1, halfselect = false;
                dp->tmp -= moved;
                if (dt->belong == playerid)
                    dt->tmp += moved;
                else
                {
                    if (moved > dt->tmp)
                    {
                        dt->tmp = moved - dt->tmp;
                        if (dt->type == 2)
                            dt->type = 5, kil(dt->belong);
                        else
                        {
                            dt->belong = playerid;
                            if (dt->type == 0)
                                dt->type = 3;
                            if (dt->type == 4)
                                dt->type = 5;
                        }
                    }
                    else
                        dt->tmp -= moved;
                }
                selectedy++;
            }
            else
            {
                if (dt->type == 2)
                {
                    int dmg1 = max(0, int(double(dp->tmp) * (1.0 + double(playeratk[playerid]) / 10.0) - double(playerac[dt->belong]) * 10.0));
                    int dmg2 = max(0, int(double(dt->tmp) * (1.0 + double(playeratk[dt->belong]) / 10.0) - double(playerac[playerid]) * 10.0));
                    dp->tmp -= dmg2;
                    dt->tmp -= dmg1;
                    if (dp->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dp->type = 0, dp->tmp = 0, dp->belong = 0, alivegennum--, playeratk[dt->belong] += playeratk[dp->belong], playerac[dt->belong] += playerac[dp->belong], ishavets[dt->belong] &= ishavets[dp->belong], playerfh[dt->belong] += playerfh[dp->belong];
                    if (dt->tmp <= 0 && mapmode != 6 && mapmode != 7)
                        dt->type = 0, dt->tmp = 0, dt->belong = 0, alivegennum--, playeratk[dp->belong] += playeratk[dt->belong], playerac[dp->belong] += playerac[dt->belong], ishavets[dp->belong] &= ishavets[dt->belong], playerfh[dp->belong] += playerfh[dt->belong];
                }
                else
                {
                    if (dt->type == 6)
                        dp->tmp += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 7)
                        playerac[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 8)
                        playeratk[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 11)
                        dp->tmp += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 12)
                        playerac[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 13)
                        playeratk[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 14)
                        blindtimeremain[dp->belong] = 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 15)
                        playermaxhp[dp->belong] += 10, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 16)
                        playermaxhp[dp->belong] += 30, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 17)
                        ishavets[dp->belong] = true, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 18)
                        playerfh[dp->belong]++, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 19)
                        playerfh[dp->belong] += 3, dt->type = 0, aliveobjectnum--;
                    if (dt->type == 9)
                    {
                        if (dt->belong != Inteam[playerid] && !ifgetflag[playerid])
                        {
                            ifgetflag[playerid] = dt->belong;
                            dt->type = 10;
                            news[newsr].a = playerid;
                            news[newsr].b = dt->belong;
                            news[newsr].opt = 1;
                            news[newsr].remtime = 50;
                            newsr++;
                        }
                        else if (dt->belong == Inteam[playerid] && selectedx == flg[dt->belong].sx && selectedy + 1 == flg[dt->belong].sy && ifgetflag[playerid])
                        {
                            mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].type = 9, mp[flg[ifgetflag[playerid]].sx][flg[ifgetflag[playerid]].sy].belong = flg[ifgetflag[playerid]].belong;
                            flagscore[Inteam[playerid]]++;
                            news[newsr].a = playerid;
                            news[newsr].opt = 3;
                            news[newsr].remtime = 50;
                            newsr++;
                            ifgetflag[playerid] = 0;
                        }
                        return;
                    }
                    else if (dt->type == 10)
                        return;
                    swap(*dt, *dp);
                }
                selectedy++;
            }
        }
        return;
    }
    void playermove(char playeraction)
    {
        if (playeraction == 'W' && selectedx > 1)
            moveup();
        else if (playeraction == 'A' && selectedy > 1)
            moveleft();
        else if (playeraction == 'S' && selectedx < X)
            movedown();
        else if (playeraction == 'D' && selectedy < Y)
            moveright();
        return;
    }
    void changetarget()
    {
        int ansx = sx, ansy = sy, anss = mp[sx][sy].tmp;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == playerid && mp[i][j].tmp > anss)
                    anss = mp[i][j].tmp, ansx = i, ansy = j;
        sx = ansx;
        sy = ansy;
        selectedx = sx;
        selectedy = sy;
        q.push(make_pair(sx, sy));
        if (rand() % 2 == 0 || !fog[sx][sy])
            memset(flag, 0, sizeof(flag));
        flag[sx][sy] = true;
        return;
    }
    void botmove()
    {
        if (q.empty())
        {
            selectedx = sx, selectedy = sy;
            changetarget();
        }
        int x = q.front().first, y = q.front().second;
        q.pop();
        for (int i = rand() % 4; rand() % 10 != 0; i = rand() % 4)
        {
            int px = x + dir[i][0], py = y + dir[i][1];
            if (px >= 1 && px <= X && py >= 1 && py <= Y && mp[px][py].type != 1 && mp[x][y].tmp > 1 && !flag[px][py] && !fog[px][py])
            {
                flag[px][py] = true;
                q.push(make_pair(px, py));
                if (i == 0)
                    moveup();
                else if (i == 1)
                    moveright();
                else if (i == 2)
                    movedown();
                else
                    moveleft();
                break;
            }
        }
        return;
    }
} player[105];
struct Team
{
    Player *members[105];
    int membernum, teamid;
    void tozero()
    {
        membernum = teamid = 0;
        return;
    }
    void move(char playeraction)
    {
        for (int i = 1; i <= membernum; i++)
        {
            if (members[i]->isbot)
            {
                members[i]->botmove();
            }
            else
                members[i]->playermove(playeraction);
        }
        return;
    }
} team[105];
void teaming()
{
    for (int i = 1; i <= teamnum; i++)
    {
        team[i].tozero();
        team[i].teamid = i;
    }
    for (int i = 1; i <= gennum; i++)
    {
        player[i].inteam = (i + (gennum / teamnum) - 1) / (gennum / teamnum);
        ifteam[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].insert(i);
        team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].membernum++;
        Inteam[i] = (i + (gennum / teamnum) - 1) / (gennum / teamnum);
        team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].members[team[(i + (gennum / teamnum) - 1) / (gennum / teamnum)].membernum] = &player[i];
    }
    return;
}
void convobject()
{
    while (aliveobjectnum < objectnum)
    {
        int px, py, trytime = 0;
        if (mapmode == 7)
        {
            while (1)
            {
                px = randnum(1, X), py = randnum(1, Y);
                trytime++;
                if (mp[px][py].type == 20 || trytime > 100)
                    break;
            }
            if (trytime > 100)
                continue;
            int gx, gy;
            while (1)
            {
                gx = randnum(px - 3, px + 3);
                gy = randnum(py - 3, py + 3);
                trytime++;
                if ((gx >= 1 && gx <= X && gy >= 1 && gy <= Y && mp[gx][gy].type == 0) || trytime > 100)
                    break;
            }
            if (trytime > 100)
                continue;
            mp[gx][gy].type = normalobjects[randnum(1, 6)];
            aliveobjectnum++;
            continue;
        }
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            trytime++;
            if (mp[px][py].type == 0 || trytime > 100)
                break;
        }
        if (trytime > 100)
            continue;
        mp[px][py].type = normalobjects[randnum(1, 6)];
        aliveobjectnum++;
    }
    return;
}
void spawnkt()
{
    for (int trytime = 0; trytime < 100; trytime++)
    {
        int i = randnum(2, X - 1), j = randnum(2, Y - 1);
        bool flag = true;
        for (int k = i - 1; k <= i + 1; k++)
            for (int w = j - 1; w <= j + 1; w++)
                if (mp[k][w].type != 0)
                {
                    flag = false;
                    k = i + 2;
                    break;
                }
        if (flag)
        {
            ktx = i;
            kty = j;
            ktremaintime = 10;
            for (int k = i - 1; k <= i + 1; k++)
                for (int w = j - 1; w <= j + 1; w++)
                    iskt[k][w] = true;
            break;
        }
    }
    return;
}
void getkt()
{
    for (int i = ktx - 1; i <= ktx + 1; i++)
        for (int j = kty - 1; j <= kty + 1; j++)
        {
            mp[i][j].type = objects[randnum(1, 11)];
            iskt[i][j] = false;
        }
    return;
}
void pubgconv()
{
    for (int i = 1; i <= gennum; i++)
        playermaxhp[i] = 100;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == 2)
                mp[i][j].tmp = 100;
    return;
}
struct pnt
{
    int sx, sy, belong, tmp;
    void tozero()
    {
        sx = sy = belong = tmp = 0;
        return;
    }
    void conv()
    {
        int px, py;
        while (1)
        {
            px = randnum(1, X), py = randnum(1, Y);
            if (mp[px][py].type != 0)
                continue;
            mp[px][py].type = 20;
            mp[px][py].belong = belong;
            sx = px;
            sy = py;
            break;
        }
        return;
    }
} pnts[200];
void pointsmatchconv()
{
    for (int i = 1; i <= gennum; i++)
    {
        pnts[i].tozero();
        pnts[i].conv();
    }
    return;
}
int main()
{
    system("title generals");
    system("color 0f");
    normalobjects[1] = 6, normalobjects[2] = 7, normalobjects[3] = 8, normalobjects[4] = 14, normalobjects[5] = 15, normalobjects[6] = 18;
    objects[1] = 6, objects[2] = 7, objects[3] = 8, objects[4] = 11, objects[5] = 12, objects[6] = 13, objects[7] = 15, objects[8] = 16, objects[9] = 17, objects[10] = 18, objects[11] = 19;
    srand(time(NULL));
    convmap();
    printf("即将进行鼠标校准……\n");
    system("pause");
    mp[1][15].type = 3;
    mp[1][15].belong = 1;
    mp[1][15].tmp = 9;
    player[1].selectedx = 1, player[1].selectedy = 15;
    sight[1][1][15] = true;
    putmap(player[1].selectedx, player[1].selectedy, 1);
    printf("请将鼠标移到右上角的9……\n");
    system("pause");
    POINT p1, p2;
    GetCursorPos(&p1);
    mp[15][1].type = 3;
    mp[15][1].belong = 1;
    mp[15][1].tmp = 9;
    player[1].selectedx = 15, player[1].selectedy = 1;
    sight[1][1][15] = false;
    sight[1][15][1] = true;
    putmap(player[1].selectedx, player[1].selectedy, 1);
    printf("请将鼠标移到左下角的9……\n");
    system("pause");
    GetCursorPos(&p2);
    LONG xx1, yy1, xx2, yy2;
    xx1 = p1.y, yy1 = p2.x, xx2 = p2.y, yy2 = p1.x;
    double dx = double(xx2 - xx1) / 14.0;
    double dy = double(yy2 - yy1) / 14.0;
    system("cls");
    starting = false;
    printf("干得好。接下来请不要移动窗口，也不要调整窗口的大小。\n");
    system("pause");
    system("cls");
    while (1)
    {
        printf("选择模式：1 = Free For All， 2 = Team Deathmatch，3 = 50v50\n");
        scanf("%d", &mode);
        if (mode == 1 || mode == 2 || mode == 3)
            break;
    }
    if (mode == 3)
    {
        fvf = true;
        mode = 2;
        X = 99;
        Y = 99;
        gennum = 100;
        teamnum = 2;
        dq = 10;
        objectpr = 0.05;
        kttime = 20;
    }
    while (1)
    {
        printf("选择地图：1 = 随机地图， 2 = 空白地图， 3 = 迷宫地图， 4 = 端午地图， 5  = 吃鸡地图， 6 = 夺旗地图， 7 = 占点地图\n");
        scanf("%d", &mapmode);
        if (mode == 1 && mapmode == 6)
        {
            printf("抱歉，夺旗地图不支持FFA模式。\n");
            continue;
        }
        if (mode == 1 && mapmode == 7)
        {
            printf("抱歉，占点地图不支持FFA模式。\n");
            continue;
        }
        if (mapmode == 1 || mapmode == 2 || mapmode == 3 || mapmode == 4 || mapmode == 5 || mapmode == 6 || mapmode == 7)
            break;
    }
    int ifown;
    while (1)
    {
        printf("选择配置：1 = 默认配置， 2 = 读取配置文件, 3 = 自定义配置\n警告：在使用自定义配置前，请务必仔细阅读代码中的注释。\n");
        scanf("%d", &ifown);
        if (ifown == 1 || ifown == 2 || ifown == 3)
            break;
    }
    if (ifown == 2)
    {
        ifstream infile;
        infile.open("config", ios::in);
        infile >> X >> Y >> wallpr >> citypr >> objectpr >> tpt >> gennum >> teamnum >> dq >> kttime >> pointstime;
        infile.close();
        printf("读取配置文件成功！\n");
    }
    if (ifown == 3)
    {
        printf("请输入地图的长\n");
        scanf("%d", &X);
        printf("请输入地图的宽\n");
        scanf("%d", &Y);
        printf("请输入墙的密度\n");
        scanf("%lf", &wallpr);
        printf("请输入城市的密度\n");
        scanf("%lf", &citypr);
        printf("请输入道具的密度\n");
        scanf("%lf", &objectpr);
        printf("请输入每个回合后的等待时间\n");
        scanf("%d", &tpt);
        printf("请输入玩家的数量\n");
        scanf("%d", &gennum);
        printf("请输入队伍的数量\n");
        scanf("%d", &teamnum);
        printf("请输入毒圈的扩散时间\n");
        scanf("%d", &dq);
        printf("请输入空投的投放时间\n");
        scanf("%d", &kttime);
        printf("请输入占领一个据点所需的时间\n");
        scanf("%d", &pointstime);
        printf("是否保存配置文件？(1 = 是，2 = 否)\n");
        int tmp;
        scanf("%d", &tmp);
        if (tmp == 1)
        {
            ofstream outfile;
            outfile.open("config", ios::out | ios::trunc);
            outfile << X << endl
                    << Y << endl
                    << wallpr << endl
                    << citypr << endl
                    << objectpr << endl
                    << tpt << endl
                    << gennum << endl
                    << teamnum << endl
                    << dq << endl
                    << kttime << endl
                    << pointstime << endl;
            outfile.close();
            printf("保存配置文件成功！\n");
        }
    }
    system("cls");
    convmap();
    if (mapmode == 1 || mapmode == 5 || mapmode == 6 || mapmode == 7)
    {
        congen();
        if (mapmode != 5 && mapmode != 6 && mapmode != 7)
            concit();
        convwall();
    }
    else if (mapmode == 2)
        congen();
    else if (mapmode == 3)
        convmaze();
    else if (mapmode == 4)
        convdragon();
    memset(sight, 0, sizeof(sight));
    int keys[6] = {'W', 'S', 'A', 'D', 'Z', 'F'};
    int turn = 1;
    int currentplayer = 1;
    objectnum = int(double(X * Y) * objectpr);
    for (int k = 1; k <= gennum; k++)
    {
        player[k].playerid = k, player[k].halfselect = false, player[k].isbot = false;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].type == 2 && mp[i][j].belong == k)
                {
                    player[k].selectedx = i;
                    player[k].selectedy = j;
                    i = X + 1;
                    break;
                }
    }
    for (int i = 2; i <= gennum; i++)
    {
        player[i].isbot = true;
        player[i].botit();
    }
    if (mapmode == 5 || mapmode == 6 || mapmode == 7)
        pubgconv();
    if (mapmode == 7)
        pointsmatchconv();
    if (mode == 1)
    {
        while (alivegennum > 1)
        {
            rm = dq - turn % dq;
            putmap(player[currentplayer].selectedx, player[currentplayer].selectedy, currentplayer);
            char inp = ' ';
            bool ismouse = true;
            if (mapmode == 5)
            {
                if (ktremaintime > 0)
                    ktremaintime--;
                if (ktremaintime == 0)
                {
                    getkt();
                    ktremaintime = -1;
                }
            }
            for (int i = 1; i <= 20; i++)
            {
                for (int j = 0; j < 6; j++)
                    if (inp == ' ' && KEY_DOWN(keys[j]))
                        inp = keys[j];
                if (ismouse && KEY_DOWN(MOUSE_MOVED))
                {
                    GetCursorPos(&p1);
                    player[currentplayer].selectedx = int(round((double(p1.y) - double(xx1)) / dx)) + 1 + (X > 15 ? player[currentplayer].selectedx - 8 : 0);
                    player[currentplayer].selectedy = int(round((double(p1.x) - double(yy1)) / dy)) + 1 + (Y > 15 ? player[currentplayer].selectedy - 8 : 0);
                    ismouse = false;
                }
                Sleep(tpt / 20);
            }
            if (inp == 'Z')
                player[currentplayer].halfselect ^= 1;
            else if (inp == 'F')
            {
                while (1)
                {
                    currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                    bool flag = false;
                    for (int i = 1; i <= X; i++)
                        for (int j = 1; j <= Y; j++)
                            if (mp[i][j].type == 2 && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                            {
                                flag = true;
                                i = X + 1;
                                break;
                            }
                    if (flag)
                        break;
                }
            }
            else
                player[currentplayer].playermove(inp);
            for (int i = 1; i <= gennum; i++)
                if (player[i].isbot)
                    player[i].botmove();
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                {
                    if (((mp[i][j].type == 2 || mp[i][j].type == 5) && mapmode != 5 && mapmode != 6) || (mp[i][j].type == 2 && (mapmode == 5 || mapmode == 6) && mp[i][j].tmp < playermaxhp[mp[i][j].belong]))
                        mp[i][j].tmp++;
                    else if (mp[i][j].type == 3 && turn % 25 == 0)
                        mp[i][j].tmp++;
                    for (int k = 1; k <= gennum; k++)
                        sight[k][i][j] = false;
                    if ((mapmode == 5 || mapmode == 6) && mp[i][j].type == 2 && mp[i][j].tmp > playermaxhp[mp[i][j].belong])
                        mp[i][j].tmp = playermaxhp[mp[i][j].belong];
                }
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].belong)
                    {
                        sight[mp[i][j].belong][i][j] = true;
                        if (mapmode == 5 && blindtimeremain[mp[i][j].belong] > 0)
                            continue;
                        if (mapmode == 5 && mp[i][j].belong == currentplayer)
                            for (int k = i - 2 - ishavets[currentplayer]; k <= i + 2 + ishavets[currentplayer]; k++)
                                for (int w = j - 2 - ishavets[currentplayer]; w <= j + 2 + ishavets[currentplayer]; w++)
                                    if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                        sight[mp[i][j].belong][k][w] = true;
                                    else
                                        ;
                        else
                            for (int px = i - 1; px <= i + 1; px++)
                                for (int py = j - 1; py <= j + 1; py++)
                                {
                                    if (px >= 1 && px <= X && py >= 1 && py <= Y)
                                        sight[mp[i][j].belong][px][py] = true;
                                }
                    }
            if (mp[player[currentplayer].selectedx][player[currentplayer].selectedy].belong != currentplayer)
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == 2 && mp[i][j].belong == currentplayer)
                        {
                            player[currentplayer].selectedx = i;
                            player[currentplayer].selectedy = j;
                            i = X + 1;
                            break;
                        }
            if (rm == 1 && mapmode == 5 && aliveobjectnum < objectnum)
                convobject();
            if (mapmode == 5)
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (fog[i][j] && mp[i][j].type == 2 && mp[i][j].tmp >= 1)
                        {
                            mp[i][j].tmp -= int(double(foglevel < 5 ? 10 : 50) * (1.0 - double(playerfh[mp[i][j].belong]) * 0.02));
                            if (mp[i][j].tmp <= 0)
                                mp[i][j].type = mp[i][j].tmp = mp[i][j].belong = 0, alivegennum--;
                        }
            if (mapmode == 5 && rm == dq)
            {
                foglevel++;
                for (int i = foglevel; i <= X - foglevel + 1; i++)
                    for (int j = foglevel; j <= Y - foglevel + 1; j++)
                        if (i == foglevel || i == X - foglevel + 1 || j == foglevel || j == Y - foglevel + 1)
                            fog[i][j] = 1;
            }
            if (mapmode == 5)
                for (int i = 1; i <= gennum; i++)
                    if (blindtimeremain[i] > 0)
                        blindtimeremain[i]--;
                    else if (blindtimeremain[i] == 0)
                        blindtimeremain[i] = -1;
            if (mapmode == 5 && turn % kttime == 0)
                spawnkt();
            turn++;
        }
        int winner = 1;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].type == 2 && mp[i][j].tmp > 0)
                {
                    winner = mp[i][j].belong;
                    i = X + 1;
                    break;
                }
        string opt = "player" + myto_string(winner) + "赢了!";
        MessageBox(NULL, opt.c_str(), "欢呼", MB_OK);
        return 0;
    }
    teaming();
    if (mapmode == 6)
    {
        ifcanconvobject = true;
        for (int i = 1; i <= teamnum; i++)
        {
            flg[i].belong = i;
            flg[i].conv();
        }
    }
    if (mapmode == 7)
        ifcanconvobject = true;
    while (aliveteamnum > 1)
    {
        rm = dq - turn % dq;
        putmap(player[currentplayer].selectedx, player[currentplayer].selectedy, currentplayer);
        char inp = ' ';
        if (mapmode == 6 || mapmode == 7)
        {
            for (int i = 1; i <= gennum; i++)
                if (wd[i] > 0)
                    playerac[i] = 1000000, wd[i]--;
                else if (wd[i] == 0)
                    playerac[i] = 0, wd[i] = -1;
        }
        if (mapmode == 5)
        {
            if (ktremaintime > 0)
                ktremaintime--;
            if (ktremaintime == 0)
            {
                getkt();
                ktremaintime = -1;
            }
        }
        bool ismouse = true;
        for (int i = 1; i <= 20; i++)
        {
            for (int j = 0; j < 6; j++)
                if (inp == ' ' && KEY_DOWN(keys[j]))
                    inp = keys[j];
            if (ismouse && KEY_DOWN(MOUSE_MOVED))
            {
                GetCursorPos(&p1);
                player[currentplayer].selectedx = int(round((double(p1.y) - double(xx1)) / dx)) + 1 + (X > 15 ? player[currentplayer].selectedx - 8 : 0);
                player[currentplayer].selectedy = int(round((double(p1.x) - double(yy1)) / dy)) + 1 + (Y > 15 ? player[currentplayer].selectedy - 8 : 0);
                ismouse = false;
            }
            Sleep(tpt / 20);
        }
        if (inp == 'Z')
            player[currentplayer].halfselect ^= 1;
        else if (inp == 'F')
        {
            while (1)
            {
                currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                bool flag = false;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == 2 && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                        {
                            flag = true;
                            i = X + 1;
                            break;
                        }
                if (flag)
                    break;
            }
        }
        for (int i = 1; i <= teamnum; i++)
            team[i].move(inp);
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
            {
                if (((mp[i][j].type == 2 || mp[i][j].type == 5) && mapmode != 5 && mapmode != 6 && mapmode != 7) || (mp[i][j].type == 2 && (mapmode == 5 || mapmode == 6 || mapmode == 7) && mp[i][j].tmp < playermaxhp[mp[i][j].belong]))
                    mp[i][j].tmp++;
                else if (mp[i][j].type == 3 && turn % 25 == 0)
                    mp[i][j].tmp++;
                for (int k = 1; k <= gennum; k++)
                    sight[k][i][j] = false;
                if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && mp[i][j].type == 2 && mp[i][j].tmp > playermaxhp[mp[i][j].belong])
                    mp[i][j].tmp = playermaxhp[mp[i][j].belong];
            }
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong)
                {
                    for (int k = 1; k <= team[player[mp[i][j].belong].inteam].membernum; k++)
                        sight[team[player[mp[i][j].belong].inteam].members[k]->playerid][i][j] = true;
                    if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && blindtimeremain[mp[i][j].belong] > 0)
                        continue;
                    if ((mapmode == 5 || mapmode == 6 || mapmode == 7) && player[mp[i][j].belong].inteam == player[currentplayer].inteam)
                        for (int k = i - 2 - ishavets[currentplayer]; k <= i + 2 + ishavets[currentplayer]; k++)
                            for (int w = j - 2 - ishavets[currentplayer]; w <= j + 2 + ishavets[currentplayer]; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y)
                                    for (int p = 1; p <= team[player[mp[i][j].belong].inteam].membernum; p++)
                                        sight[team[player[mp[i][j].belong].inteam].members[p]->playerid][k][w] = true;
                                else
                                    ;
                    for (int px = i - 1; px <= i + 1; px++)
                        for (int py = j - 1; py <= j + 1; py++)
                        {
                            if (px >= 1 && px <= X && py >= 1 && py <= Y)
                                for (int k = 1; k <= team[player[mp[i][j].belong].inteam].membernum; k++)
                                    sight[team[player[mp[i][j].belong].inteam].members[k]->playerid][px][py] = true;
                        }
                }
        if (mp[player[currentplayer].selectedx][player[currentplayer].selectedy].belong != currentplayer)
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == 2 && mp[i][j].belong == currentplayer)
                    {
                        player[currentplayer].selectedx = i;
                        player[currentplayer].selectedy = j;
                        i = X + 1;
                        break;
                    }
        if (rm == 1 && mapmode == 5 && aliveobjectnum < objectnum && aliveobjectnum < int(double(X * Y) * objectpr) && (!fvf || (fvf && foglevel % 3 == 0)))
            convobject();
        if ((mapmode == 6 || mapmode == 7) && ifcanconvobject && aliveobjectnum < objectnum)
        {
            ifcanconvobject = false;
            convobject();
        }
        if (mapmode == 5)
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (fog[i][j] && mp[i][j].type == 2 && mp[i][j].tmp >= 1)
                    {
                        mp[i][j].tmp -= int(double(foglevel < 5 ? 10 : 50) * (1.0 - double(playerfh[mp[i][j].belong]) * 0.02));
                        if (mp[i][j].tmp <= 0)
                            mp[i][j].type = mp[i][j].tmp = mp[i][j].belong = 0, alivegennum--;
                    }
        if (mapmode == 5 && rm == dq)
        {
            foglevel++;
            for (int i = foglevel; i <= X - foglevel + 1; i++)
                for (int j = foglevel; j <= Y - foglevel + 1; j++)
                    if (i == foglevel || i == X - foglevel + 1 || j == foglevel || j == Y - foglevel + 1)
                        fog[i][j] = 1;
        }
        if (mapmode != 6 && mapmode != 7)
            for (int i = 1; i <= teamnum; i++)
            {
                bool ok = false;
                if (teamdead[i])
                    continue;
                for (int j = 1; j <= X; j++)
                    for (int k = 1; k <= Y; k++)
                        if (player[mp[j][k].belong].inteam == i)
                        {
                            ok = true;
                            j = X + 1;
                            break;
                        }
                if (!ok)
                    aliveteamnum--, teamdead[i] = 1;
            }
        if (mapmode == 6 || mapmode == 7)
        {
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == 2 && mp[i][j].tmp <= 0)
                    {
                        if (mapmode == 7)
                            teampointsmatchscore[Inteam[mp[i][j].belong]] = max(0, teampointsmatchscore[Inteam[mp[i][j].belong]] - 10);
                        player[mp[i][j].belong].respawn();
                    }
            if (mapmode == 6)
                for (int i = 1; i <= teamnum; i++)
                    if (flagscore[i] >= 10)
                    {
                        string opt = "team" + myto_string(i) + "赢了!";
                        MessageBox(NULL, opt.c_str(), "欢呼", MB_OK);
                        return 0;
                    }
        }
        if (mapmode == 5 || mapmode == 6 || mapmode == 7)
            for (int i = 1; i <= gennum; i++)
                if (blindtimeremain[i] > 0)
                    blindtimeremain[i]--;
                else if (blindtimeremain[i] == 0)
                    blindtimeremain[i] = -1;
        bool isget = false;
        for (int i = 1; i <= X; i++)
            for (int j = 1; j <= Y; j++)
                if (mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                {
                    isget = true;
                    i = X + 1;
                    break;
                }
        if (!isget)
        {
            isgz = true;
            while (1)
            {
                currentplayer = (currentplayer == gennum ? 1 : currentplayer + 1);
                bool flag = false;
                for (int i = 1; i <= X; i++)
                    for (int j = 1; j <= Y; j++)
                        if (mp[i][j].type == 2 && mp[i][j].belong == currentplayer && mp[i][j].tmp > 0)
                        {
                            flag = true;
                            i = X + 1;
                            break;
                        }
                if (flag)
                    break;
            }
        }
        if (mapmode == 5 && turn % kttime == 0)
            spawnkt();
        if (mapmode == 7)
        {
            for (int i = 1; i <= teamnum; i++)
                teampointsmatchland[i] = 0;
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                    if (mp[i][j].type == 20 && mp[i][j].belong != 0)
                        teampointsmatchland[mp[i][j].belong]++;
            for (int i = 1; i <= teamnum; i++)
                teampointsmatchscore[i] += teampointsmatchland[i];
            for (int i = 1; i <= teamnum; i++)
                if (teampointsmatchscore[i] >= 1000)
                {
                    string opt = "team" + myto_string(i) + "赢了!";
                    MessageBox(NULL, opt.c_str(), "欢呼", MB_OK);
                    return 0;
                }
            for (int i = 1; i <= X; i++)
                for (int j = 1; j <= Y; j++)
                {
                    if (mp[i][j].type == 20 && mp[i][j].belong != 0 && mp[i][j].tmp > 0)
                    {
                        bool foundplayer = false, foundenemy = false;
                        for (int k = i - 2; k <= i + 2; k++)
                            for (int w = j - 2; w <= j + 2; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y && mp[k][w].type == 2 && mp[k][w].tmp > 0)
                                {
                                    if (Inteam[mp[k][w].belong] == mp[i][j].belong)
                                        foundplayer = true;
                                    else
                                        foundenemy = true;
                                }
                        if (foundenemy && !foundplayer)
                        {
                            mp[i][j].tmp--;
                            if (mp[i][j].tmp <= 0)
                            {
                                news[newsr].a = mp[i][j].belong;
                                news[newsr].opt = 5;
                                news[newsr].remtime = 50;
                                newsr++;
                                mp[i][j].tmp = 0;
                                mp[i][j].belong = 0;
                            }
                        }
                        if (foundplayer && !foundenemy && mp[i][j].tmp < pointstime)
                            mp[i][j].tmp++;
                    }
                    else if (mp[i][j].type == 20 && mp[i][j].belong == 0 && mp[i][j].tmp < pointstime)
                    {
                        int playerteam = -1;
                        bool sameteam = true;
                        for (int k = i - 2; k <= i + 2; k++)
                        {
                            for (int w = j - 2; w <= j + 2; w++)
                                if (k >= 1 && k <= X && w >= 1 && w <= Y && mp[k][w].type == 2 && mp[k][w].tmp > 0)
                                {
                                    if (playerteam == -1)
                                        playerteam = Inteam[mp[k][w].belong];
                                    else if (playerteam != Inteam[mp[k][w].belong])
                                    {
                                        sameteam = false;
                                        break;
                                    }
                                }
                            if (!sameteam)
                                break;
                        }
                        if (sameteam && playerteam != -1)
                        {
                            mp[i][j].tmp++;
                            if (mp[i][j].tmp >= pointstime)
                            {
                                news[newsr].a = playerteam;
                                news[newsr].opt = 4;
                                news[newsr].remtime = 50;
                                newsr++;
                                mp[i][j].tmp = pointstime;
                                mp[i][j].belong = playerteam;
                                ifcanconvobject = true;
                            }
                        }
                    }
                }
        }
        turn++;
    }
    int winner = 1;
    for (int i = 1; i <= X; i++)
        for (int j = 1; j <= Y; j++)
            if (mp[i][j].type == 2 && mp[i][j].tmp > 0)
            {
                winner = player[mp[i][j].belong].inteam;
                i = X + 1;
                break;
            }
    string opt = "team" + myto_string(winner) + "赢了!";
    MessageBox(NULL, opt.c_str(), "欢呼", MB_OK);
    return 0;
}
