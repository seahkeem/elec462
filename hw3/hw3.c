#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <dirent.h>
#include <pwd.h>
#include <curses.h>
#include <time.h>
#include <sys/time.h>
#include <sys/param.h>

#define SIZE 64

struct pid_stat{
    int     pid;
    char    user[10];
    char    stat;
    char    pr[5];
    char    TIME[10];
    char    command[100];
    long    ni;

    double          cpu_usage;
    unsigned long   virt;
    unsigned long   res;
    unsigned long   shr;
    unsigned long   utime;
    unsigned long   stime;

    struct pid_stat *next;
    struct pid_stat *prevent;
};

struct pid_stat*    head;
struct pid_stat*    tail;
struct timeval      start;
timer_t main_timer;
int     MEMTOT;
int     pos;
int     width;
int     height;
int     MAX_PROC;
long long memorys[2][8];

void set_signal(void);
void is_Num(char *name);
void set_timer(void);
void set_memtotal();
void print_data(double result);
static void ref_data(int signo);

int main(){
    char    ch;
    double  result;
    struct timeval end;

    head    = NULL;
    tail    = NULL;
    
    set_memtotal();
    set_signal();
    set_timer();
    initscr();

    chdir("/proc");
    print_data(0);
    gettimeofday(&start, NULL);
    keypad(stdscr, TRUE);

    while((ch = getch()) != 'q'){
        if(ch != 3 && ch != 2)
            continue;
        if(ch == 3 && pos >0)
            pos--;
        else if(ch == 2 && pos < MAX_PROC-1)
            pos++;
        clear();

        timer_delete(main_timer);
        gettimeofday(&end, NULL);
        result=(end.tv_sec - start.tv_sec)+((end.tv_usec - start.tv_usec)/1000000.0);
        print_data(result);
        gettimeofday(&start, NULL);
        set_timer();
    }
    endwin();
}

void ref_data(int signo){
    double result;
    struct timeval end;

    gettimeofday(&end, NULL);
    result=(end.tv_sec-start.tv_sec)+((end.tv_usec-start.tv_usec)/1000000.0);
    clear();
    print_data(result);
    gettimeofday(&start, NULL);
}

void set_signal(void){
    struct sigaction sa;

    sa.sa_handler   = ref_data;
    sa.sa_flags     = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, NULL);
}

void set_timer(void){
    struct itimerspec   it_spec;
    struct sigevent     sp;

    sp.sigev_notify=SIGEV_SIGNAL;
    sp.sigev_signo=SIGALRM;
    timer_create(CLOCK_REALTIME, &sp, &main_timer);

    it_spec.it_interval.tv_sec  = 3;
    it_spec.it_interval.tv_nsec = 0;
    it_spec.it_value.tv_sec     = 3;
    it_spec.it_value.tv_nsec    = 0;

    timer_settime(main_timer, 0, &it_spec, NULL);
}

int isNum(char* name){
    int i;

    for(i = 0; name[i] != '\0'; i++){
        if(name[i] < '0' || name[i] > '9')
            return 0;
    }
    return 1;
}

void set_memtotal(){
    FILE*   fp;
    char    token[SIZE];
    int c, i;
    if((fp = fopen("/proc/meminfo", "r")) == NULL){
        fprintf(stderr,"meminfo open error\n");
        exit(1);
    }
    while((c = fgetc(fp)) != '\n'){
        if(c >= '0' && c <= '9'){
            i = 0;
            token[i++] = c;
            c = fgetc(fp);
            while(c >= '0' && c <= '9'){
                token[i++] = c;
                c = fgetc(fp);
            }
            token[i] = '\0';
            break;
        }
    }
    fclose(fp);
    MEMTOT = atol(token);
}

void print_uptime(){
    struct tm *tmbuf;
    FILE* fp;
    char token[SIZE];
    double avg1, avg2, avg3;
    int c, i;
    time_t now, t;

    if((fp = fopen("/proc/uptime", "r")) == NULL){
        fprintf(stderr,"uptime open error\n");
        exit(1);
    }
    i = 0;
    c = fgetc(fp);
    while(c >= '0' && c <= '9'){
        token[i++] = c;
        c = fgetc(fp);
    }
    token[i] = '\0';
    fclose(fp);

    now     = time(NULL);
    tmbuf   = localtime(&now);
    printw("hw3 - %02d:%02d:%02d ", tmbuf->tm_hour, tmbuf->tm_min, tmbuf->tm_sec);
    t       = atoi(token);

    if(t < 3600)
        printw("up %-2d min, ", t/60);
    else if(t < 86400)
        printw("up %d:%02d, ", t/3600, (t%3600)/60);
    else
        printw("up %d days, ", t/86400);

    if((fp = fopen("/var/run/utmp", "r")) == NULL){
        fprintf(stderr,"utmp open error\n");
        exit(1);
    }
    i = 0;
    while(!feof(fp)){
        c = fgetc(fp);
        if(c == ':'){
            while((c = fgetc(fp)) != ':');
            i++;
        }
    }
    fclose(fp);
    printw(" %d user,  ", i);
    if((fp = fopen("/proc/loadavg", "r")) == NULL){
        fprintf(stderr,"loadavg open error\n");
        exit(1);
    }
    fscanf(fp, "%lf %lf %lf", &avg1, &avg2, &avg3);
    fclose(fp);
    printw("load average: %.2f, %.2f, %.2f\n", avg1, avg2, avg3);
}

void print_meminfo(){
    char tmp[20], tmp2[10];
    int mem[8] ,tmp3;
    FILE* fp;
    int i;

    if((fp = fopen("/proc/meminfo", "r")) == NULL){
        fprintf(stderr,"meminfo open error\n");
        exit(1);
    }
    
    for(i = 0; i < 24; i++){
        fscanf(fp,"%s %d %s\n",tmp, &tmp3, tmp2);
        if(i < 5)
            mem[i] = (int)tmp3/1.024;
        else if(i == 14)
            mem[5] = (int)tmp3/1.024;
        else if(i == 15)
            mem[6] = (int)tmp3/1.024;
        else if(i == 23)
            mem[7] = (int)tmp3/1.024;
    }
    
    tmp3 = mem[3] + mem[4] + mem[7];
    printw("KiB Mem : %8d total, %8.d free, %8d used, %8d buff/cache\n",
           mem[0], mem[1], (mem[0]-mem[1]-tmp3), tmp3);
    printw("KiB Swap: %8d total, %8d free, %8d used, %8d avail Mem\n",
           mem[5], mem[6], (mem[5]-mem[6]), mem[2]);
    fclose(fp);
}

void set_cpu_stat(int flag){
    FILE* fp;
    int i;
    long long cpu_stat[8];
    char name[SIZE];

    fp = fopen("/proc/stat", "r");
    fscanf(fp, "%s %lld %lld %lld %lld %lld %lld %lld %lld", name, &cpu_stat[0], &cpu_stat[1], &cpu_stat[2], &cpu_stat[3]
            , &cpu_stat[4], &cpu_stat[5], &cpu_stat[6], &cpu_stat[7]);

    fclose(fp);
    if(flag == 0){
        for(i = 0; i < 8; i++){
            memorys[0][i] = cpu_stat[i];
            memorys[1][i] = 0;
        }
    }
    else{
        for(i = 0; i < 8; i++){
            memorys[1][i] = cpu_stat[i] - memorys[0][i];
            memorys[0][i] = cpu_stat[i];
        }
    }
}

void print_data(double result){
    int i,j,c, process_uid;
    
    int is_null = 0;
    int ps_cnt  = 0;
    int run_cnt = 0;
    int stp_cnt = 0;
    int zom_cnt = 0;
    
    unsigned long long tmp, tot, totaltime, starttime;
    double uptime, ps_uptime, cpu_usage;
    long pr;
    FILE* fp;
    DIR* dirp;
    char tokens[26][SIZE], token[SIZE];
    char print_buffer[1024];
    
    struct stat statbuf;
    struct dirent* dp;
    struct pid_stat *new_data, *cur, *delete;
    struct passwd *pw;

    getmaxyx(stdscr,i, j);
    width = j;
    height = i;

    if(head == NULL && tail == NULL){
        set_cpu_stat(0);
        is_null = 1;
    }
    else
        set_cpu_stat(1);
    tot = 0;
    for(i = 0; i < 8; i++){
        if(is_null)
            tot += memorys[0][i];
        else
            tot += memorys[1][i];
    }
    result *= 100;
    cur = head;
    if((dirp = opendir("/proc")) == NULL){
        fprintf(stderr,"/proc open fail\n");
        exit(1);
    }
    while((dp = readdir(dirp)) != NULL){
        memset(tokens, 0, sizeof(tokens));
        if(isNum(dp->d_name)){
            if(stat(dp->d_name, &statbuf) < 0){
                fprintf(stderr,"%s stat error\n", dp->d_name);
                continue;
            }

            process_uid = statbuf.st_uid;
            pw = getpwuid(process_uid);

            if(strlen(pw->pw_name) > 8){
                strncpy(tokens[0],pw->pw_name, 7);
                strcat(tokens[0],"+");
            }
            else
                strcpy(tokens[0], pw->pw_name);
            chdir(dp->d_name);
            if((fp = fopen("stat", "r")) == NULL){
                fprintf(stderr,"stat open error\n");
                exit(1);
            }
            for(i = 1; i < 23; i++){
                j = 0;
                while((c = fgetc(fp)) != ' '){
                    if(c == '('){
                        while((c = fgetc(fp)) != ')')
                            tokens[i][j++] = c;
                    }
                    else
                        tokens[i][j++] = c;
                }
                tokens[i][j] = '\0';
            }
            fclose(fp);
            if((fp = fopen("statm","r")) == NULL){
                fprintf(stderr,"statm open error\n");
                endwin();
            }
            for(i = 23; i < 26; i++){
                j = 0;
                while((c = fgetc(fp)) != ' ')
                    tokens[i][j++] = c;
                tokens[i][j] = '\0';
            }
            fclose(fp);
            chdir("..");

            new_data            = (struct pid_stat*)malloc(sizeof(struct pid_stat));
            new_data->prevent   = NULL;
            new_data->next      = NULL;
            new_data->pid       = atoi(tokens[1]);
            strcpy(new_data->user, tokens[0]);
            new_data->ni    = atol(tokens[19]);
            new_data->virt  = strtoul(tokens[23], NULL, 10) * 4;
            new_data->res   = strtoul(tokens[24], NULL, 10) * 4;
            new_data->shr   = strtoul(tokens[25], NULL, 10) * 4;
            new_data->utime = strtoul(tokens[14], NULL, 10);
            new_data->stime = strtoul(tokens[15], NULL, 10);
            pr              = atol(tokens[18]);
            
            if(pr == -100)
                strcpy(new_data->pr, "rt");
            else
                sprintf(new_data->pr,"%ld",pr);
            if(is_null){
                if(head == NULL){
                    head    = new_data;
                    tail    = new_data;
                    cur     = new_data;
                }
                else{
                    cur->next = new_data;
                    new_data->prevent = cur;
                    cur = new_data;
                    tail = new_data;
                }
                totaltime = new_data->utime + new_data->stime;
                starttime = strtoull(tokens[22], NULL, 10);

                if((fp = fopen("/proc/uptime", "r")) == NULL){
                    fprintf(stderr,"/prco/uptime open error\n");
                    exit(1);
                }
                i = 0;
                while((c = fgetc(fp)) != ' ')
                    token[i++] = c;
                token[i] = '\0';
                uptime = atof(token);
                fclose(fp);

                ps_uptime = uptime - (starttime/HZ);
                cpu_usage = 100 * ((totaltime/HZ) / ps_uptime);

                c = (int)(cpu_usage * 10);
                cpu_usage = c/10.0;
                new_data->cpu_usage = cpu_usage;
            }
            else{
                while(cur != NULL && cur->pid < new_data->pid){
                    if(cur->prevent == NULL){
                        head = cur->next;
                        if(cur->next != NULL)
                            cur->next->prevent = NULL;
                    }
                    if(cur->next == NULL){
                        tail = cur->prevent;
                        if(cur->prevent != NULL)
                            cur->prevent->next = NULL;
                    }
                    if(cur->prevent != NULL && cur->next != NULL){
                        cur->prevent->next = cur->next;
                        cur->next->prevent = cur->prevent;
                    }
                    delete = cur;
                    cur = cur->next;
                    free(delete);
                }
                if(cur == NULL){
                    cpu_usage           = (double)(new_data->utime + new_data->stime) / result *100;
                    new_data->cpu_usage = cpu_usage;
                    tail->next          = new_data;
                    new_data->prevent   = tail;
                    tail                = new_data;
                }
                else{
                    if(new_data->pid == cur->pid){
                        cpu_usage = (double)((new_data->utime + new_data->stime)-(cur->utime + cur->stime)) / result *100;
                        new_data->cpu_usage = cpu_usage;

                        new_data->next      = cur->next;
                        new_data->prevent   = cur->prevent;

                        if(cur->next != NULL)
                            cur->next->prevent = new_data;
                        else
                            tail = new_data;
                        if(cur->prevent != NULL)
                            cur->prevent->next = new_data;
                        else
                            head = new_data;

                        delete = cur;
                        cur = cur->next;
                        free(delete);
                    }
                    else if(new_data->pid < cur->pid){
                        cpu_usage = (double)(new_data->utime + new_data->stime) / result *100;
                        new_data->cpu_usage = cpu_usage;

                        if(cur->prevent == NULL){
                            cur->prevent    = new_data;
                            new_data->next  = cur;
                            head            = new_data;
                        }
                        else{
                            cur->prevent->next  = new_data;
                            new_data->prevent   = cur->prevent;
                            cur->prevent        = new_data;
                            new_data->next      = cur;
                        }
                    }
                }

            }
            new_data->stat = tokens[3][0];
            if(tokens[3][0] == 'R')
                run_cnt++;
            else if(tokens[3][0] == 'Z')
                zom_cnt++;
            else if(tokens[3][0] == 'T')
                stp_cnt++;
            ps_cnt++;
            tmp = new_data->utime + new_data->stime;
            sprintf(new_data->TIME,"%lld:%02lld.%02lld",(tmp/HZ)/60, (tmp/HZ)%60, tmp%100);
            strcpy(new_data->command, tokens[2]);
        }
    }
    if(!is_null && cur != NULL){
        tail = cur->prevent;
        while(cur != NULL){
            delete = cur;
            cur = cur->next;
            free(delete);
        }
        tail->next = NULL;
    }
    print_uptime();
    printw("Tasks:%4d total,%4d running,%4d sleeping,%4d stopped,%4d zombie\n",ps_cnt, run_cnt, ps_cnt-run_cnt, stp_cnt, zom_cnt);
    if(is_null)
        printw("%%Cpu(s):  %2.1f us  %2.1f sy,  %2.1f ni, %2.1f id,  %2.1f wa, %2.1f hi, %2.1f si,  %2.1f st\n",
               (double)memorys[0][0] / tot *100,
               (double)memorys[0][2] / tot *100,
               (double)memorys[0][1] / tot *100,
               (double)memorys[0][3] / tot *100,
               (double)memorys[0][4] / tot *100,
               (double)memorys[0][5] / tot *100,
               (double)memorys[0][6] / tot *100,
               (double)memorys[0][7] / tot *100);
    else
        printw("%%Cpu(s):  %2.1f us  %2.1f sy,  %2.1f ni, %2.1f id,  %2.1f wa, %2.1f hi, %2.1f si,  %2.1f st\n",
               (double)memorys[1][0] / tot *100,
               (double)memorys[1][2] / tot *100,
               (double)memorys[1][1] / tot *100,
               (double)memorys[1][3] / tot *100,
               (double)memorys[1][4] / tot *100,
               (double)memorys[1][5] / tot *100,
               (double)memorys[1][6] / tot *100,
               (double)memorys[1][7] / tot *100);
    print_meminfo();
    printw("\n  PID USER      PR  NI    VIRT    RES    SHR S  %%CPU  %%MEM     TIME+ COMMAND\n");

    MAX_PROC = ps_cnt;
    if(pos > MAX_PROC)
        pos = MAX_PROC-1;
    ps_cnt  = 0;

    i   = 0;
    cur = head;
    while(cur != NULL){
        if(i-pos > height-9)
            break;
        if(i >= pos){
            memset(print_buffer, 0, sizeof(print_buffer));
            sprintf(print_buffer,"%5d %-8s %3s %3ld %7ld %6ld %6ld %c   %2.1f   %2.1f %9s ",
                    cur->pid, cur->user, cur->pr, cur->ni, cur->virt, cur->res, cur->shr,
                    cur->stat, cur->cpu_usage, ((cur->res*100)/(double)MEMTOT),cur->TIME);
            c = strlen(print_buffer);
            if(strlen(cur->command) > width-c-1){
                cur->command[width-c-2] = '+';
                strncat(print_buffer, cur->command, width-c-1);
                print_buffer[strlen(print_buffer)] = '\0';
            }
            else
                strcat(print_buffer, cur->command);
            printw("%s\n",print_buffer);
        }
        cur = cur->next;
        i++;
    }
}
