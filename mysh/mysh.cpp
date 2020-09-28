#include <iostream>
#include <vector>
#include <cstring>

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;


enum LexTag {
    SPACE, ARG, ARG_WITH_SPACE
};

const int MAX_ARGS_NUMBER = 256;

void doIt() {
    cout << '>';

    string line;
    getline(cin, line);

    LexTag status = SPACE;
    vector<string> _args;
    string _arg = "";

    for (int i = 0; i < line.length(); i++) {
        switch (line[i]) {
            case ' ' : {
                // 是空格

                if (status == ARG_WITH_SPACE) {
                    // 该参数吃空格
                    _arg += ' ';
                    break;
                }
            }
            case '\r' :
            case '\n' :
            case '\t' : {
                // 是空格

                if (status != SPACE) {
                    // 从非空格转到空格
                    // 产生新的参数

                    _args.push_back(_arg);
                    _arg = "";
                }
                // 其他情况不产生参数

                status = SPACE;
                break;
            }
            default : {
                // 不是空格

                if (status == SPACE) {
                    if (line[i] == '"') {
                        status = ARG_WITH_SPACE;
                    } else {
                        _arg += line[i];
                        status = ARG;
                    }
                } else if (status == ARG) {
                    _arg += line[i];
                } else if (status == ARG_WITH_SPACE) {

                    if (line[i] == '"') {
                        _args.push_back(_arg);
                        _arg = "";
                        status = SPACE;
                        break;
                    }

                    // TODO 理论上这里还要写转义的，我不想写了
                    _arg += line[i];
                }
                
                break;
            }
        }

        // cout << int(status);
    }

    if (_arg.length() != 0) _args.push_back(_arg);

    //cout << endl;
    //for (string s : _args) {
    //    cout << s << endl;
    //}

    char **_argvs = (char**) malloc( sizeof(char*) * MAX_ARGS_NUMBER );

    int argidx = 0;
    for (string s : _args) {

        char *_argv = new char[s.length() + 1];
        strcpy(_argv, s.c_str());

        _argvs[argidx ++] = _argv;
    }
		
	// 执行命令
	pid_t pid, wpid;
	int pstatus;

    pid = fork();
    if (pid == 0) {
        // 子进程用来执行外部命令hexadd
        if (execv(_argvs[0], _argvs) == -1) {
            cout << "internal error\n";
        }
    } else
    if (pid < 0) {
        // fork出错
        cout << "fork error\n";
    } else {
        // 父进程（即shell）在子进程执行完成前一直阻塞
        do {
            wpid = waitpid(pid, &pstatus, WUNTRACED);
        } while (!WIFEXITED(pstatus) && WIFSIGNALED(pstatus));  // 若子进程不是正常结束或者因为外部信号而结束，则父进程继续等待（换言之，即只有当子进程正常执行完毕，父进程才跳出等待状态）
    }



}

int main(){

    while (true) {
        doIt();
    }

}
