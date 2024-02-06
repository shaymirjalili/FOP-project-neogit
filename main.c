#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#define invalid printf("Invalid command!")
#define MAX_GIT_PROJECTS 200
#define MAX_LENGTH 40
#define MAX_LINE_LENGTH 1000
//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular underline text
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background 
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define reset "\e[0m"
#define CRESET "\e[0m"
#define COLOR_RESET "\e[0m"
char cwd[1024];
char repositorypath[1024];
char tag[30][30];
/*int tracked_in_commit(char* filename){
    char path[1024];
    strcpy(path,current_config->repopath);
    strcat(path,"/commit/");
    strcat(path,branch);
    DIR* go_to_branch=opendir(path);
    struct dirent* ent;
    int exist=0;
    while((ent=readdir(go_to_branch))!=NULL)
        if(strcmp(ent->d_name,filename)==0){
            exist=1;
            break;
        }
    return exist;
}*/
//int modified(char* filepath,char* filename);
//int tracked(char* filename);
int is_empty(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}
int is_regular_file(const char *path){
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
    }
int isDirectory(const char *path) {
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}///////////////////////////////////////////////////////////////////////////
void run_hook(char* filename){
    int failed=0;
    char to_show[1024];
    strcpy(to_show,filename);
    for(int i=0;i<strlen(to_show);i++) if(to_show[i]=='^') to_show[i]='/';
    printf(BHBLU"\n%s:\n"reset,to_show);
    char filepath[1024]=".neogit/stage/";
    strcat(filepath,filename);
    //printf("%s\n",filepath);
    FILE* hooklist=fopen(".neogit/hooklist.txt","r");
    char line[MAX_LINE_LENGTH];
    while((fgets(line,MAX_LINE_LENGTH,hooklist))!=NULL){
        //printf("%s",line);
        if(strncmp(line,"todo-check",10)==0){
            printf(BHYEL "todo-check" BCYN "..........................."reset);
            if((strstr(filepath,".txt")!=NULL || strstr(filepath,".cpp")!=NULL )||strstr(filepath,".c")!=NULL){
            int passed=1;
            FILE* test=fopen(filepath,"r");
            char line1[MAX_LINE_LENGTH];
            while((fgets(line1,MAX_LINE_LENGTH,test))!=NULL){
                if(strstr(line1,"TODO")!=NULL){
                    passed=0;
                    break;
                }
            }
            fclose(test);
            if(passed==0){
                printf(BRED"FAILED\n"reset);
                failed=1;
            }
            if(passed) printf(BGRN "PASSED\n"reset);
            }
            else printf(BYEL"SKIPPED\n"reset);
                    
        }
        else if(strncmp("file-size-check",line,15)==0){
            printf(BCYN "file-size-check" BCYN "..........................."reset);
            int passed=1;
            struct stat st;
            stat(filepath, &st);
            int size = st.st_size;
            if(size>5000000) passed=0;
            if(passed==0){
                printf(BRED"FAILED\n"reset);
                failed=1;
            }
            if(passed) printf(BGRN "PASSED\n"reset);
        }
        else if(strncmp(line,"character-limit",15)==0){
            printf(BRED "character-limit" BCYN "..........................."reset);
            if((strstr(filepath,".txt")!=NULL || strstr(filepath,".cpp")!=NULL )||strstr(filepath,".c")!=NULL){
                int passed=1;
                FILE* test=fopen(filepath,"r");
                char line1[MAX_LINE_LENGTH];
                int total=0;
                while((fgets(line1,MAX_LINE_LENGTH,test))!=NULL){
                    total+=strlen(line1);
                }
                if(total>20000) passed=0;
                fclose(test);
                if(passed==0){
                    printf(BRED"FAILED\n"reset);
                    failed=1;
                }
                if(passed) printf(BGRN "PASSED\n"reset);
                }
            else printf(BYEL"SKIPPED\n"reset);
        }
        else if(strncmp(line,"balance-braces",14)==0){
            printf(BMAG "balance-braces" BCYN "..........................."reset);
            if((strstr(filepath,".txt")!=NULL || strstr(filepath,".cpp")!=NULL )||strstr(filepath,".c")!=NULL){
                int passed=1;
                int parenthesis=0;
                int braces=0;
                int brackets=0;
                FILE* test=fopen(filepath,"r");
                char c;
                while((c = fgetc(test)) != EOF){
                    if(c=='[') brackets++;      
                    else if(c==']') brackets--;
                    else if(c=='{') braces++;
                    else if(c=='}') braces--;
                    else if(c=='(') parenthesis++;
                    else if(c==')') parenthesis--;
                } 
                if(braces!=0 || brackets!=0 ||parenthesis!=0) passed=0;
                if(passed==0){
                    printf(BRED"FAILED\n"reset);
                    failed=1;
                }
                if(passed) printf(BGRN "PASSED\n"reset);
            }
            else printf(BYEL"SKIPPED\n"reset);
        }
    }
    if(failed){
        char new_filepath[1024]=".neogit/stage/";
        char new_filename[1024]="#";
        strcat(new_filename,filename);
        strcat(new_filepath,new_filename);
        rename(filepath,new_filepath);
    }
}

///////////////////////////////////////////////////////////////////////////
int run_add(char* filepath,char* filename){
    char realpath[1024]=".neogit/stage/";
    //strcat(realpath,filename);
    char fakepath[1024];
    strcpy(fakepath,filepath);
    for(int i=0;i<strlen(fakepath);i++) if(fakepath[i]=='/') fakepath[i]='^';
    //printf("%s",fakepath);
    strcat(realpath,fakepath);
    FILE* stage=fopen(realpath,"wb");
    FILE* workingdirfile=fopen(filepath,"rb");
    char buff[BUFSIZ];
    size_t m;
    while((m=fread(buff,1,BUFSIZ,workingdirfile))!=0)
        fwrite(buff,1,m,stage);
    fclose(stage);
    fclose(workingdirfile);
    FILE* lastadd=fopen(".neogit/lastadd.txt","a");
    fprintf(lastadd,"%s\n",fakepath);
    fclose(lastadd);
    FILE* list=fopen(".neogit/stagedlist.txt","a");
    fprintf(list,"%s\n",filename);
    fclose(list);
    return 0;
}
////////////////////////////////////////////////////////////////////////////
/*int run_add(char* filepath,char* filename){
    char filepathcpy[1024];
    strcpy(filepathcpy,filepath);
    char x[1024];
    strcpy(x,repositorypath);
    strcat(x,"/.neogit/lastadd.txt");
    FILE* lastadd=fopen(x,"a");
    char path[1024]=".neogit/stage/";
    for(int i=0;i<strlen(filepathcpy);i++) if(filepathcpy[i]=='/') filepathcpy[i]='^';
    strcat(path,filepathcpy);
    char buff[BUFSIZ];
    size_t m;
    FILE* stage=fopen(path,"wb");
    FILE* file=fopen(filepath,"rb");
    while((m=fread(buff,1,BUFSIZ,file))!=0)
        fwrite(buff,1,m,stage);
    fclose(stage);
    fclose(file);
    fprintf(lastadd,"%s\n",filepathcpy);
    fclose(lastadd);
    FILE* stagedlist=fopen(".neogit/stagedlist.txt","a");
    fprintf(stagedlist,"%s 0\n",filepath);
    fprintf(stagedlist,"%s\n",path);
    fclose(stagedlist);
       /* if(ident==0){
            char tmppath[1024]=".neogit/stage/";
            strcat(tmppath,"tmp");
            FILE* tmp=fopen(tmppath,"w");
            while((fgets(line,MAX_LINE_LENGTH,now))!=NULL)
                fputs(line,tmp);
            remove(path);
            char new[1024]=".neogit/stage/";
            rename(tmppath,path);
            fclose(prev);
            fclose(now);
            fprintf(lastadd,"%s\n",filename);
        }
        if(ident) return 1;
    }
    else{
        FILE* stage=fopen(path,"w");
        FILE* to_copy=fopen(filepath,"r");
        while((fgets(line,MAX_LINE_LENGTH,to_copy))!=NULL)
            fputs(line,stage);
        fclose(stage);
        fclose(to_copy);
        fprintf(lastadd,"%s\n",filename);
    }

    fclose(lastadd);
    FILE* list=fopen(".neogit/stagedlist.txt","r+");
    fprintf(list,"%s\n",filename);
    fprintf(list,"%s\n",filepath);
    fclose(list);
    return 0;*/
    

/*int tracked(char* filepath){
    DIR* stagedir=opendir(".neogit/satge");
    struct dirent* stgentry;
    while((entry=readdir(stagedir))!=NULL){
        if(strcmp(entry->d_name,filename)==0){
            closedir(stagedir);
            return 1;
        }
    }
    closedir(stagedir);
    return 0;
}*/
int modified(char* filepath1,char* filepath2){//should use realpath() to identify filepath
    FILE      *pFile1,*pFile2;
    long      lSize1, lSize2;               // file length
    int       i=0;
    char      tmp1, tmp2;

    pFile1 = fopen(filepath1,"rb");
    pFile2 = fopen(filepath2,"rb");

    // obtain file size:
    fseek (pFile1 , 0 , SEEK_END);
    lSize1 = ftell (pFile1);
    rewind (pFile1);

    // obtain file size:
    fseek (pFile2 , 0 , SEEK_END);
    lSize2 = ftell (pFile2);
    rewind (pFile2);

    if (lSize1 != lSize2) return 0;
    
    for (i=0;i<lSize1;i++) {
    fread(&tmp1, 1, 1, pFile1);
    fread(&tmp2, 1, 1, pFile2);
    if (tmp1 != tmp2) return 0; 
        
    }
    return 1;
}
// char* get_file_name(char* filepath){
//     while(strchr(filepath,'/')!=NULL){
//         filepath=strchr(filepath,'/');
//         int n=strlen(filepath);
//         for(int i=1;i<n;i++){
//             filepath[i-1]=filepath[i];
//         }
//         //printf("%s\n",filepath);
//         filepath[n-1]='\0';
//     }
//     return filepath;
// }
#ifdef _DEBUG_
int main()
{
    int argc=2;
    char const *argv[]={"neogit","init"};
#else
int main(int argc,char const *argv[]){
#endif
    //printf("&&&");
    char globalpath[1024]="/mnt/c/c_test/globals.txt";
    char tmpglobpath[1024]="/mnt/c/c_test/tmpglob.txt";
    char *argvtmp[10];
    for(int i=0;i<10;i++)
        argvtmp[i]=malloc(10*sizeof(char));
    int argctmp=argc;
    for(int i=0;i<argc;i++){
        strcpy(argvtmp[i],argv[i]);
    }
    //for (int i=0;i<argctmp;i++) printf("%s\n",argvtmp[i]);
    if(argctmp<2)
        invalid;
    if(strcmp(argvtmp[0],"neogit")!=0)
        invalid;

    //                                  //check if a repository exists?
    char tmp_cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) return 1;
    int exists = 0;
    struct dirent *entry;
    do {
        // find .neogit
        DIR *dir = opendir(".");
        if (dir == NULL) 
            perror("Error opening current directory");

        while ((entry = readdir(dir)) != NULL) {
            if ((entry->d_type) == 4 && strcmp(entry->d_name, ".neogit") == 0){
              exists = 1;  
              char actualpath[1024];
              strcpy(repositorypath,realpath(entry->d_name,actualpath));
              //printf("%s\n",repositorypath);
            }

        }
        closedir(dir);

        // update current working directory
        if (getcwd(tmp_cwd, sizeof(tmp_cwd)) == NULL) return 1;

        // change cwd to parent
        if (strcmp(tmp_cwd, "/") != 0) {
            if (chdir("..") != 0) return 1;
        }

    } while (strcmp(tmp_cwd, "/") != 0);
    // return to the initial cwd
    if (chdir(cwd) != 0) return 1;
    //printf("$$$$$$$$$$$$$$");
    if (exists==0 && (strcmp(argvtmp[1],"init")!=0 && (strcmp(argvtmp[1],"config")!=0 || strcmp(argvtmp[2],"--global")!=0))){
        perror("neogit repository has not been initialized yet. use \"neogit init\" to do so.\n");
    }
    else if(exists==0 && strcmp(argvtmp[1],"init")==0){
        if(argctmp!=2) invalid;
        else{
            mkdir(".neogit",0755);
            FILE* global=fopen(globalpath,"r");
            char line[MAX_LINE_LENGTH];
            int email;
            int username;
            int alias;
            char tmp_str[50];
            char string[50];
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d %s",tmp_str,&email,string);
            if(email==0) printf("please enter your email: neogit config (--global) user.email \"your email@gmail.com\"\n");
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d %s",tmp_str,&username,string);
            if(username==0)
                printf("please enter your username: neogit config (--global) user.name \"your username\"\n");
            FILE* tempp=fopen(".neogit/config.txt","a");
            fgets(line,MAX_LINE_LENGTH,global);
            sscanf(line,"%s %d",tmp_str,&alias);
            fprintf(tempp,"user.email 0\n");
            fprintf(tempp,"user.name 0\n");
            fprintf(tempp,"alias 0\n");
            fclose(tempp);
            mkdir(".neogit/stage",0755);
            mkdir(".neogit/commit",0755);
            FILE* log=fopen(".neogit/log.txt","w");
            fclose(log);
            mkdir(".neogit/commit/master",0755);
            FILE* lastadd=fopen(".neogit/lastadd.txt","w");
            fclose(lastadd);
            FILE* commitcount=fopen(".neogit/commitcount.txt","w");
            fprintf(commitcount,"1234");
            fclose(commitcount);
            FILE* branch=fopen(".neogit/branch.txt","w");
            fprintf(branch,"master");
            fclose(branch);
            FILE* taglist=fopen(".neogit/taglist.txt","w");
            fclose(taglist);
            FILE* hooklist=fopen(".neogit/hooklist.txt","w");
            fclose(hooklist);
            FILE* stagedlist=fopen(".neogit/stagedlist.txt","w");
            fclose(stagedlist);
            FILE* commitlist=fopen(".neogit/commitlist.txt","w");
            fclose(commitlist);
            printf("initialized your repository successfully!\n");
        }
    }
    else if(exists==1 && strcmp(argvtmp[1],"init")==0)
        printf("neogit repository already exists!\n");
    //printf("********************");

    //Let's check for alias commands
    FILE* global=fopen(globalpath,"r");
    char line[MAX_LINE_LENGTH];
    if(exists==1){
        FILE* config=fopen(".neogit/config.txt","r");
        while((fgets(line,MAX_LINE_LENGTH-1,config))!=NULL){
            if(strncmp("alias",line,5)==0){
                char alias[30];
                char equiv[30];
                while((fgets(alias,29,config))!=NULL){
                    alias[strlen(alias)-1]='\0';
                    fgets(equiv,29,config);
                    equiv[strlen(equiv)-1]='\0';
                    if(strcmp(alias,argvtmp[1])==0){
                        char* pch=strtok(equiv," \0\n");
                        argctmp--;
                        while(pch){
                            strcpy(argvtmp[argctmp++],pch);
                            pch=strtok(NULL," \0\n");
                        }
                        break;
                    }
                }
                break;
            }
        }
        fclose(config);
    }
    //printf("11111111111111");
    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
        if(strncmp("alias",line,5)==0){
            char alias[30];
            char equiv[30];
            while((fgets(alias,29,global))!=NULL){
                alias[strlen(alias)-1]='\0';
                fgets(equiv,29,global);
                equiv[strlen(equiv)-1]='\0';
                if(strcmp(alias,argvtmp[1])==0){
                    char* pch=strtok(equiv," \0\n");
                    argctmp--;
                    while(pch){
                        printf("%s\n",pch);
                        strcpy(argvtmp[argctmp++],pch);
                        pch=strtok(NULL," \0\n");
                    }
                    break;
                }
            }
            break;
        }
    }
    fclose(global);
    //printf("&&&&&&");
    //config command:
    if(strcmp(argvtmp[1],"config")==0){
        if(strcmp(argvtmp[2],"--global")==0){
            FILE* global=fopen(globalpath,"r");
            FILE* tmpglob=fopen(tmpglobpath,"w");
            char line[MAX_LINE_LENGTH];
            if(strcmp(argvtmp[3],"user.name")==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.name",line,9)==0)
                            fprintf(tmpglob,"user.name 1 %s\n",argvtmp[4]);
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set username \"%s\" globally!\n",argvtmp[4]);
                }
            }
            else if(strcmp(argvtmp[3],"user.email")==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("user.email",line,9)==0)
                            fprintf(tmpglob,"user.email 1 %s\n",argvtmp[4]);
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set email \"%s\" globally!\n",argvtmp[4]);
                }
            }
            else if(strncmp(argvtmp[3],"alias.",6)==0){
                if(argctmp!=5) invalid;
                else{
                    while((fgets(line,MAX_LINE_LENGTH,global))!=NULL){
                        if(strncmp("alias",line,5)==0){
                            int n;
                            sscanf(line,"alias %d",&n);
                            fprintf(tmpglob,"alias %d\n", n+1);
                            char* alias=calloc(20,sizeof(char));
                            alias=strstr(argvtmp[3],".");
                            int len=strlen(alias);
                            for(int i=1;i<len;i++)
                                alias[i-1]=alias[i];
                            alias[len-1]='\0';
                            strcat(alias,"\n");
                            fputs(alias,tmpglob);
                            fputs(argvtmp[4],tmpglob);
                            fprintf(tmpglob,"\n");
                        }
                        else fputs(line,tmpglob);
                    }
                    remove(globalpath);
                    rename(tmpglobpath,globalpath);
                    fclose(global);
                    printf("set alias globally!\n");
                }
            }
        }
        else if(strcmp(argvtmp[2],"--global")!=0){
            if(argctmp!=4) invalid;
            else{
                FILE* local=fopen(".neogit/config.txt","r");
                FILE* tmplocal=fopen(".neogit/configtmp.txt","w");
                char line[MAX_LINE_LENGTH];
            if(strcmp(argvtmp[2],"user.name")==0){
                    while((fgets(line,MAX_LINE_LENGTH,local))!=NULL){
                        if(strncmp("user.name",line,9)==0)
                            fprintf(tmplocal,"user.name 1 %s\n",argvtmp[3]);
                        else fprintf(tmplocal,"%s",line);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set username \"%s\" for your project!\n",argvtmp[3]);
            }
            else if(strcmp(argvtmp[3],"user.email")==0){
                    while((fgets(line,MAX_LINE_LENGTH,local))!=NULL){
                        if(strncmp("user.email",line,9)==0)
                            fprintf(tmplocal,"user.email 1 %s\n",argvtmp[3]);
                        else fprintf(tmplocal,"%s",line);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set email \"%s\" for your project!\n",argvtmp[3]);
            }
            else if(strncmp(argvtmp[2],"alias.",6)==0){
                    while((fgets(line,MAX_LINE_LENGTH,local))!=NULL){
                        if(strncmp("alias",line,5)==0){
                            int n;
                            sscanf(line,"alias %d",&n);
                            fprintf(tmplocal,"alias %d\n", n+1);
                            char* alias=calloc(20,sizeof(char));
                            alias=strstr(argvtmp[2],".");
                            int len=strlen(alias);
                            for(int i=1;i<len;i++)
                                alias[i-1]=alias[i];
                            alias[len-1]='\0';
                            strcat(alias,"\n");
                            fputs(alias,tmplocal);
                            fputs(argvtmp[3],tmplocal);
                            fprintf(tmplocal,"\n");
                        }
                        else fprintf(tmplocal,"%s",line);
                    }
                    remove(".neogit/config.txt");
                    rename(".neogit/configtmp.txt",".neogit/config.txt");
                    fclose(local);
                    printf("set alias!\n");
            }

            }
        }

    }
     else if(strcmp(argvtmp[1],"add")==0){
        FILE* historyofadds=fopen(".neogit/lastadd.txt","w");
        char filename[1024];
        if(argctmp==3){
            if(is_regular_file(argvtmp[2])==1){
                char check_path[1024]=".neogit/stage/";
                strcat(check_path,argvtmp[2]);
                strcpy(filename,argvtmp[2]);
                run_add(argvtmp[2],filename);
                printf("staged the file \"%s\"!\n",argvtmp[2]);
            } 
            else if(isDirectory(argvtmp[2])==1){
                struct dirent* ent;
                DIR* dir=opendir(argvtmp[2]);
                char filepath[1024];
                int staged=0;
                while((ent=readdir(dir))!=NULL){
                 if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    char check_path[1024]=".neogit/stage/";
                    strcat(check_path,ent->d_name);
                    char newpath[1024];
                    strcpy(newpath,argvtmp[2]);
                    strcat(newpath,"/");
                    strcat(newpath,ent->d_name);
                    strcpy(filepath,argvtmp[2]);
                    strcat(filepath,"/");
                    strcat(filepath,ent->d_name);
                    run_add(filepath,ent->d_name); 
                 }
                }
                printf("staged the directory \"%s\"!\n",argvtmp[2]);
                closedir(dir);
            }
            else{
                printf("couldn't find file/directory \"%s\"!\n",argvtmp[2]);
            }
        }
        else if(strcmp(argvtmp[2],"-f")==0){
            if(argctmp<4) invalid;
            else{
                for(int i=3;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\"!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])==1){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[i]);
                    char filepath[1024];
                    while((ent=readdir(dir))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){

                        strcpy(filepath,argvtmp[i]);
                        strcat(filepath,"/");
                        strcat(filepath,ent->d_name);
                        run_add(filepath,ent->d_name); 
                    }
                    }
                    printf("staged the directory \"%s\"!\n",argvtmp[i]);
                    closedir(dir);
                    }
                    else{
                        printf("couldn't find file/directory \"%s\"!\n",argvtmp[i]);
                    }
                }
            }
        }
        else if(strcmp(argvtmp[2],"-f")!=0 && strcmp(argvtmp[2],"-n")!=0 && argctmp>3){
            for(int i=2;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    //strcpy(filename,get_file_name(filename));
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\" successfully!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])){
                        struct dirent* ent;
                        DIR* dir=opendir(argvtmp[i]);
                            while((ent=readdir(dir))!=NULL){
                            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                            strcpy(filename,argvtmp[i]);
                            //strcpy(filename,get_file_name(filename));
                            char path[1024];
                            strcpy(path,argvtmp[i]);
                            strcat(path,"/");
                            strcat(path,ent->d_name);
                            run_add(path,ent->d_name); 
                            }
                            } 
                        closedir(dir);
                        printf("staged the directory \"%s\" successfully!\n",argvtmp[i]);
                    }

                }
            
        }
        else if(strcmp(argvtmp[2],"-n")==0 && strcmp(argvtmp[3],"1")==0){
            if(argctmp!=4) invalid;
            else{
                DIR* current=opendir(".");
                struct dirent* read_current_dir;
                while((read_current_dir=readdir(current))!=NULL){
                    if(strcmp(read_current_dir->d_name,".")!=0 && strcmp(read_current_dir->d_name,"..")!=0 && strcmp(read_current_dir->d_name,"src")!=0 && strcmp(read_current_dir->d_name,".git")!=0){
                        if(read_current_dir->d_type==4){
                            DIR* innerdir=opendir(read_current_dir->d_name);
                            struct dirent* read_inner;
                            while((read_inner=readdir(innerdir))!=NULL){
                                if(strcmp(read_inner->d_name,".")!=0 && strcmp(read_inner->d_name,"..")!=0){
                                    int staged=1;
                                    if(read_inner->d_type==8){
                                        char getname[1024];
                                        strcpy(getname,read_current_dir->d_name);
                                        strcat(getname,"/");
                                        strcat(getname,read_inner->d_name);
                                        char raw[1024]=".neogit/stage/";
                                        strcat(raw,read_inner->d_name);
                                        if(is_regular_file(raw)!=1) staged=0;
                                        else{
                                            // char raw2[1024];
                                            // strcpy(raw2,read_current_dir->d_name);
                                            // strcat(raw2,"/");
                                            // strcat(raw2,read_inner->d_name);
                                            if(modified(raw,getname)==0)
                                                staged=0;
                                        }

                                    }
                                    if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_inner->d_name);
                                    if(staged==1) printf("name of file: %s --> status: staged\n\n",read_inner->d_name);
                                }
                            }
                        }
                        else if(read_current_dir->d_type==8){
                            int staged=1;
                            char raw[1024]=".neogit/stage/";
                            strcat(raw,read_current_dir->d_name);
                            if(is_regular_file(raw)!=1) staged=0;
                            else{
                                char raw2[1024];
                                strcpy(raw2,read_current_dir->d_name);
                                if(modified(raw,raw2)==0)
                                staged=0;
                                }
                            if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_current_dir->d_name);
                            if(staged==1) printf("name of file: %s --> status: staged\n\n",read_current_dir->d_name);
                            
                         }
                    }
                }
            }


        }
    }
    /*else if(strcmp(argvtmp[1],"add")==0){
        FILE* historyofadds=fopen(".neogit/lastadd.txt","w");
        char filename[1024];
        if(argctmp==3){
            if(is_regular_file(argvtmp[2])){
                strcpy(filename,argvtmp[2]);
                strcpy(filename,get_file_name(filename));
                run_add(argvtmp[2],filename);
            } 
            else if(isDirectory(argvtmp[2])){
                struct dirent* ent;
                DIR* dir=opendir(argvtmp[2]);
                while((ent=readdir(dir))!=NULL){
                 if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    char filepath[1024];
                    strcpy(filepath,argvtmp[2]);
                    strcat(filepath,"/");
                    strcat(filepath,ent->d_name);
                    run_add(filepath,ent->d_name); 

                 }
                }
                printf("staged the directory \"%s\"!\n",argvtmp[2]);
                closedir(dir);
            }
            else{
                printf("couldn't find file/directory \"%s\"!\n",argvtmp[2]);
            }
        }
        else if(strcmp(argvtmp[2],"-f")==0){
            if(argctmp<4) invalid;
            else{
                for(int i=3;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    strcpy(filename,get_file_name(filename));
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\"!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[i]);
                    char filepath[1024];
                    while((ent=readdir(dir))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                        strcpy(filepath,argvtmp[i]);
                        strcat(filepath,"/");
                        strcat(filepath,ent->d_name);
                        run_add(filepath,ent->d_name); 
                    }
                    }
                    printf("staged the directory \"%s\"!\n",argvtmp[i]);
                    closedir(dir);
                    }
                    else{
                        printf("couldn't find file/directory \"%s\"!\n",argvtmp[i]);
                    }
                }
            }
        }
        else if(strcmp(argvtmp[2],"-f")!=0 && strcmp(argvtmp[2],"-n")!=0 && argctmp>3){
            for(int i=2;i<argctmp;i++){
                    if(is_regular_file(argvtmp[i])){
                    strcpy(filename,argvtmp[i]);
                    strcpy(filename,get_file_name(filename));
                    run_add(argvtmp[i],filename);
                    printf("staged the file \"%s\" successfully!\n",argvtmp[i]);
                    } 
                    else if(isDirectory(argvtmp[i])){
                        struct dirent* ent;
                        DIR* dir=opendir(argvtmp[i]);
                            while((ent=readdir(dir))!=NULL){
                            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                            strcpy(filename,argvtmp[i]);
                            strcpy(filename,get_file_name(filename));
                            char path[1024];
                            strcpy(path,argvtmp[i]);
                            strcat(path,"/");
                            strcat(path,ent->d_name);
                            run_add(path,ent->d_name); 
                            }
                            } 
                        closedir(dir);
                        printf("staged the directory \"%s\" successfully!\n",argvtmp[i]);
                    }

                }
            
        }
        else if(strcmp(argvtmp[2],"-n")==0 && strcmp(argvtmp[3],"1")==0){
            if(argctmp!=4) invalid;
            else{
                DIR* current=opendir(".");
                struct dirent* read_current_dir;
                while((read_current_dir=readdir(current))!=NULL){
                    int staged=1;
                    if(strcmp(read_current_dir->d_name,".")!=0 && strcmp(read_current_dir->d_name,"..")!=0){
                        if(read_current_dir->d_type==4){
                            DIR* innerdir=opendir(read_current_dir->d_name);
                            struct dirent* read_inner;
                            while((read_inner=readdir(innerdir))!=NULL){
                                if(strcmp(read_inner->d_name,".")!=0 && strcmp(read_inner->d_name,"..")!=0){
                                    if(read_inner->d_type==8){
                                        char getname[1024];
                                        strcpy(getname,read_inner->d_name);
                                        //printf("::%s\n",read_inner->d_name);
                                        char raw[1024]=".neogit/stage/";
                                        strcat(raw,read_current_dir->d_name);
                                        strcat(raw,"^");
                                        strcat(raw,read_inner->d_name);
                                        FILE* f=fopen(raw,"r");
                                        if(is_regular_file(f)==0)
                                            staged=0;
                                        else{
                                            char raw2[1024];
                                            strcpy(raw2,read_current_dir->d_name);
                                            strcat(raw2,"/");
                                            strcat(raw2,read_inner->d_name);
                                            if(modified(raw,raw2))
                                                staged=0;
                                        }

                                    }
                                    if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_inner->d_name);
                                    if(staged==1) printf("name of file: %s --> status: staged\n\n",read_inner->d_name);
                                }
                            }
                        }
                        else if(read_current_dir->d_type==8){
                            char raw[1024]=".neogit/stage/";
                            strcat(raw,read_current_dir->d_name);
                            FILE* f=fopen(raw,"r");
                            if(is_regular_file(raw)==0)
                                staged=0;
                            else{
                                char raw2[1024];
                                strcpy(raw2,read_current_dir->d_name);
                                if(modified(raw,raw2))
                                staged=0;
                                }
                            if(staged==0) printf("name of file: %s --> status: unstaged\n\n",read_current_dir->d_name);
                            if(staged==1) printf("name of file: %s --> status: staged\n\n",read_current_dir->d_name);
                            
                         }
                    }
                }
            }


        }
    }*/
    else if(strcmp(argvtmp[1],"reset")==0){
        if(argctmp!=3) invalid;
        else{
            if(strcmp(argvtmp[2],"-undo")==0){
                char line[MAX_LINE_LENGTH];
                FILE* f=fopen(".neogit/lastadd.txt","r");
                while((fgets(line,MAX_LINE_LENGTH,f))!=NULL){
                    char filename[100];
                    line[strlen(line)-1]='\0';
                    strcpy(filename,line);
                    char del[1024]=".neogit/stage/";
                    strcat(del,filename);
                    remove(del);
                    for(int i=0;i<strlen(line);i++) if(line[i]=='^') line[i]='/';
                    printf("unstaged file \"%s\"!\n",line);
                }
                fclose(f);
                // FILE* stagedlisttmp=fopen(".neogit/stagedlist.txt","w");
                // FILE* stagedlist=fopen(".neogit/stagedlist.txt","r");
                // while((fgets(line,MAX_LINE_LENGTH,stagedlist))!=NULL){
                //     line[strlen(line)-1]='\0';
                //     char path[1024]=".neogit/stage/";
                //     strcat(path,line);
                //     if(is_regular_file(path)==0) continue;
                //     else
                //         fprintf(stagedlisttmp,"%s\n",line);   
                // }
            }
            else{
                if(isDirectory(argvtmp[2])){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[2]);
                    while((ent=readdir(dir))!=NULL){
                        if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                          char del[1024]=".neogit/stage/";
                          strcat(del,argvtmp[2]);
                          strcat(del,"^");
                          strcat(del,ent->d_name);
                          remove(del);  
                        }
                    }
                    printf("unstaged directory \"%s\"!\n",argvtmp[2]);
                }
                else if(is_regular_file(argvtmp[2])){
                    char del[1024]=".neogit/stage/";
                    strcat(del,argvtmp[2]);
                    remove(del);
                    printf("unstaged file \"%s\"!\n",argvtmp[2]); 
                }
            }
        FILE* stagedlisttmp=fopen(".neogit/stagedlisttmp.txt","w");
        FILE* stagedlist=fopen(".neogit/stagedlist.txt","r");
        while((fgets(line,MAX_LINE_LENGTH,stagedlist))!=NULL){
            line[strlen(line)-1]='\0';
            char path[1024]=".neogit/stage/";
            strcat(path,line);
            if(is_regular_file(path)==0) continue;
            else
                fprintf(stagedlisttmp,"%s\n",line);   
        }
        remove(".neogit/stagedlist.txt");
        rename(".neogit/stagedlisttmp.txt",".neogit/stagedlist.txt");
        fclose(stagedlist);
        fclose(stagedlisttmp);
        FILE* lastadd=fopen(".neogit/lastadd.txt","w");
        fclose(lastadd);
        }
        
    }
    /*else if(strcmp(argvtmp[1],"reset")==0){
        if(argctmp!=3) invalid;
        else{
            if(strcmp(argvtmp[2],"-undo")==0){
                FILE* f=fopen(".neogit/lastadd.txt","r");
                char line[MAX_LINE_LENGTH];
                while((fgets(line,MAX_LINE_LENGTH,f))!=NULL){
                    char filename[100];
                    line[strlen(line)-1]='\0';
                    remove(line);
                    char* pch=strtok(line,"^");
                    char* filn;
                    while(pch){
                        filn=pch;
                        pch=strtok(NULL,"^");
                    }
                    printf("unstaged file \"%s\"!\n",filn);
                }
                fclose(f);
            }
            else{
                if(isDirectory(argvtmp[2])){
                    struct dirent* ent;
                    DIR* dir=opendir(argvtmp[2]);
                    while((ent=readdir(dir))!=NULL){
                        if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                          char del[1024]=".neogit/stage/";
                          strcat(del,argvtmp[2]);
                          strcat(del,"^");
                          strcat(del,ent->d_name);
                          remove(del);  
                        }
                    }
                    printf("unstaged directory \"%s\"!\n",argvtmp[2]);
                }
                else if(is_regular_file(argvtmp[2])){
                    char del[1024]=".neogit/stage/";
                    strcat(del,argvtmp[2]);
                    remove(del);
                    printf("unstaged file \"%s\"!\n",argvtmp[2]); 
                }
            }
        }
    }*/
    else if(strcmp(argvtmp[1],"set")==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0){
        if(strlen(argvtmp[3])>72){
            printf("your message shouln't have more than 72 charachters!\n");
            return 1;
        }
        else{
            FILE* shortcut=fopen(".neogit/shortcut.txt","r");
            if(shortcut==NULL){
                FILE* sh=fopen(".neogit/shortcut.txt","w");
                fprintf(sh,"%s",argvtmp[3]);
                fclose(sh);
            }
            else{
                fclose(shortcut);
                FILE* sh=fopen(".neogit/shortcut.txt","w");
                fprintf(sh,"%s",argvtmp[3]);
                fclose(sh);
            }
            printf("set the shortcut message as \"%s\".\n",argvtmp[3]);
        }
        
    }
    else if(strcmp(argvtmp[1],"replace")==0 && strcmp(argvtmp[2],"-m")==0 && strcmp(argvtmp[4],"-s")==0 && strcmp(argvtmp[5],"shortcut-name")==0){
        if(strlen(argvtmp[3])>72){
            printf("your message shouln't have more than 72 charachters!\n");
            return 1;
        }
        else{
            if(is_regular_file(".neogit/shortcut.txt")==0){
                printf("there is no shortcut to replace!\n");
                return 1;
            }
            else{
                FILE* sh=fopen(".neogit/shortcut.txt","w");
                fprintf(sh,"%s",argvtmp[3]);
                fclose(sh);
            }
            printf("changed the shortcut message as \"%s\".\n",argvtmp[3]);
        }
    }
    else if(strcmp(argvtmp[1],"remove")==0 && strcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0){
            if(is_regular_file(".neogit/shortcut.txt")==0){
                printf("there is no shortcut to remove!\n");
                return 1;
            }
            else{
                remove(".neogit/shortcut.txt");
            }
            printf("removed the shortcut message successfully!\n");
    }
    else if(strcmp(argvtmp[1],"commit")==0){
        char message[72];
        int commit_ID;
        char branch[30];
        char username[50];
        char tmp[20];
        int n=0;
        int user;
        if(strcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0){
            if(is_regular_file(".neogit/shortcut.txt")==0){
                printf("there is no shortcut message available!\n");
                return 1;
            }
            else{
                FILE* shortcut=fopen(".neogit/shortcut.txt","r");
                fgets(message,72,shortcut);
            }
        }
        else{
            if(argctmp<4 || strcmp(argvtmp[2],"-m")!=0 ){
                printf("you have to enter a message to make a commit!\n");
                return 1;
            }
            else if(strlen(argvtmp[3])>72){
                printf("your message shouln't have more than 72 charachters!\n");
                return 1;
            }
            else{
                strcpy(message,argvtmp[3]);
            }
        }
                FILE* commitcount=fopen(".neogit/commitcount.txt","r");
                FILE* commitcounttmp=fopen(".neogit/commitcounttmp.txt","w");
                char line[MAX_LINE_LENGTH];
                fgets(line,MAX_LINE_LENGTH,commitcount);
                sscanf(line,"%d",&commit_ID);
                fprintf(commitcounttmp,"%d",++commit_ID);
                remove(".neogit/commitcount.txt");
                rename(".neogit/commitcounttmp.txt",".neogit/commitcount.txt");
                fclose(commitcount);
                fclose(commitcounttmp);
                FILE* branchf=fopen(".neogit/branch.txt","r");
                fgets(line,MAX_LINE_LENGTH,branchf);
                strcpy(branch,line);
                fclose(branchf);
                FILE* config=fopen(".neogit/config.txt","r");
                fgets(line,MAX_LINE_LENGTH,config);
                fgets(line,MAX_LINE_LENGTH,config);
                sscanf(line,"%s %d %s",tmp,&user,username);
                if(user==0){
                    FILE* globalconfig=fopen(globalpath,"r");
                    fgets(line,MAX_LINE_LENGTH,globalconfig);
                    fgets(line,MAX_LINE_LENGTH,globalconfig);
                    sscanf(line,"%s %d %s",tmp,&user,username);
                    fclose(globalconfig);
                }
                fclose(config);
                //FILE* list=fopen(".neogit/stagedlist.txt","r");
                //char filename[MAX_LINE_LENGTH];
                FILE* commitlisttmp=fopen(".neogit/commitlisttmp.txt","w");
                FILE* committree=fopen(".neogit/committree.txt","a");
                DIR* stagedir=opendir(".neogit/stage");
                struct dirent* ent;
                while((ent=readdir(stagedir))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                        if(strncmp(ent->d_name,"#",1)==0){
                            char to_show[1024];
                            strcpy(to_show,ent->d_name);
                            for(int i=0;i<strlen(to_show);i++) to_show[i]=to_show[i+1];
                            for(int i=0;i<strlen(to_show);i++) if(to_show[i]=='^') to_show[i]='/';
                            printf("file \"%s\" hasn't passed the hooks!\n",to_show);
                            return 1;
                        }
                        char fileinstagepath[1024]=".neogit/stage/";
                        strcat(fileinstagepath,ent->d_name);
                        FILE* stage=fopen(fileinstagepath,"r");
                        char commitpath[1024]=".neogit/commit/";
                        strcat(commitpath,branch);
                        strcat(commitpath,"/");
                        strcat(commitpath,ent->d_name);
                        if(isDirectory(commitpath)==0)
                            mkdir(commitpath,0755);
                        DIR* commitdir=opendir(commitpath);
                        strcat(commitpath,"/");
                        char commitidstr[50];
                        sprintf(commitidstr,"%d",commit_ID);
                        strcat(commitpath,commitidstr);
                        strcat(commitpath,".txt");
                        FILE* new_commit=fopen(commitpath,"w");
                        char line[MAX_LINE_LENGTH];
                        while((fgets(line,MAX_LINE_LENGTH,stage))!=NULL)
                            fprintf(new_commit,"%s",line);
                        fclose(stage);
                        fclose(new_commit);
                        closedir(commitdir);
                        n++;
                        remove(fileinstagepath);
                        fprintf(commitlisttmp,"%d %s %s\n",commit_ID,branch,ent->d_name);
                }
                }
                FILE* commitlist=fopen(".neogit/commitlist.txt","r");
                while((fgets(line,MAX_LINE_LENGTH,commitlist))!=NULL)
                    fprintf(commitlisttmp,"%s",line);
                remove(".neogit/commitlist.txt");
                rename(".neogit/commitlisttmp.txt",".neogit/commitlist.txt");
                fclose(commitlist);
                fclose(commitlisttmp);
                FILE* tmpp=fopen(".neogit/stagedlist.txt","w");
                fclose(tmpp);
                if(n==0) {
                    printf("there is no file to commit!\n");
                    return 1;
                }
                char timedate[1000];
                time_t t = time(NULL);                          ////time here
                struct tm tm = *localtime(&t);
                sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+1, tm.tm_sec+30);  
                FILE* log=fopen(".neogit/log.txt","r");
                FILE* tmplog=fopen(".neogit/logtmp.txt","w");
                fprintf(tmplog,"#commit ID: %d\n time:%s message: %s\n author: %s\n branch: %s\n number of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                printf("commited successfully!\ncommit ID: %d\ntime:%smessage: %s\nauthor: %s\nbranch: %s\nnumber of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                fprintf(committree,"%d %s\n",commit_ID%100,branch);
                while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                    fputs(line,tmplog);
                remove(".neogit/log.txt");
                rename(".neogit/logtmp.txt",".neogit/log.txt");
                fclose(log);
                fclose(tmplog);
                fclose(committree);
                
        }
    /*else if(strcmp(argvtmp[1],"commit")==0){
        //printf("%s\n",repositorypath);
        printf("**");
        char message[72];
        int commit_ID;
        char branch[30];
        char username[50];
        char tmp[20];
        int n=0;
        int user;
        //printf("1");
        if(strcmp(argvtmp[2],"-s")==0 && strcmp(argvtmp[3],"shortcut-name")==0){
            if(is_regular_file(".neogit/shortcut.txt")==0){
                printf("there is no shortcut message available!\n");
                return 1;
            }
            else{
                FILE* shortcut=fopen("shortcut.txt","r");
                fgets(message,72,shortcut);
                fclose(shortcut);
            }
        }
        else{
            if(argctmp<4 || strcmp(argvtmp[2],"-m")!=0 ){
                printf("you have to enter a message to make a commit!\n");
                return 1;
            }
            else if(strlen(argvtmp[3])>72){
                printf("your message shouln't have more than 72 charachters!\n");
                return 1;
            }
            else{
                strcpy(message,argvtmp[3]);
            }
        }
        //printf("2");
        // FILE* commitcount=fopen(".neogit/commitcount.txt","r");
        // FILE* commitcounttmp=fopen(".neogit/commitcounttmp.txt","w");
        // char line[MAX_LINE_LENGTH];
        // fgets(line,MAX_LINE_LENGTH,commitcount);
        // sscanf(line,"%d",&commit_ID);
        // fprintf(commitcounttmp,"%d",commit_ID+1);
        // remove(".neogit/commitcount.txt");
        // rename(".neogit/commitcounttmp.txt",".neogit/commitcount.txt");
        // fclose(commitcount);
        // fclose(commitcounttmp);
        // FILE* branchf=fopen(".neogit/branch.txt","r");
        // fscanf(branchf,"%s",branch);
        // fclose(branchf);
        // FILE* config=fopen(".neogit/config.txt","r");
        // fgets(line,MAX_LINE_LENGTH,config);
        // fgets(line,MAX_LINE_LENGTH,config);
        // sscanf(line,"%s %d %s",tmp,&user,username);
        // if(user==0){
        //     FILE* globalconfig=fopen(globalpath,"r");
        //     fgets(line,MAX_LINE_LENGTH,globalconfig);
        //     fgets(line,MAX_LINE_LENGTH,globalconfig);
        //     sscanf(line,"%s %d %s",tmp,&user,username);
        //     fclose(globalconfig);
        // }
        // fclose(config);
        FILE* commitlist=fopen(".neogit/commitlist.txt","r");
        FILE* commitlisttmp=fopen(".neogit/commitlisttmp.txt","w");
        FILE* stagedlist=fopen(".neogit/stagedlist.txt","r");
        char realfilepath[1024];
        char path[1024];
        while((fgets(realfilepath,1024,stagedlist))!=NULL){
            fgets(path,1024,stagedlist);
            realfilepath[strlen(realfilepath)-1]='\0';
            int hook;
            char pathh[1024];
            sscanf(realfilepath,"%s %d",pathh,&hook);
            switch (hook){
            case 1:
                printf("can't commit file \"%s\". it did't pass todo-check hook!\n",realfilepath);
                return 1;
                break;
            case 2:
                printf("can't commit file \"%s\". it did't pass file-size-check hook!\n",realfilepath);
                return 1;
                break;
            case 3:
                printf("can't commit file \"%s\". it did't pass character-limit hook!\n",realfilepath);
                return 1;
                break;
            case 4:
                printf("can't commit file \"%s\". it did't pass balance-braces hook!\n",realfilepath);
                return 1;
                break;
            case 0:
               break;
            }
            path[strlen(path)-1]='\0';
            char line[MAX_LINE_LENGTH];
            fprintf(commitlisttmp,"%d %s %s %s\n",commit_ID,branch,realfilepath,path);
            while((fgets(line,MAX_LINE_LENGTH,commitlist))!=NULL)
                fprintf(commitlisttmp,"%s",line);
            char* format=strchr(realfilepath,'.');
            char myfilepath[1024]=".neogit/commit/";
            strcat(myfilepath,branch);
            strcat(myfilepath,"/");
            strcat(myfilepath,path);
            if(isDirectory(myfilepath)==0)
                mkdir(myfilepath,0755);
            DIR* commitfolder=opendir(myfilepath);
            strcat(myfilepath,"/");
            char commitstr[50];
            sprintf(commitstr,"%d",commit_ID);
            strcat(myfilepath,commitstr);
            strcat(myfilepath,format);
            //printf("%s",myfilepath);
            FILE* new_commit=fopen(myfilepath,"wb");
            FILE* stage=fopen(path,"rb");
            char buff[BUFSIZ];
            size_t m;
            while((m=fread(buff,1,BUFSIZ,stage))!=0)
                fwrite(buff,1,m,new_commit);
            fclose(new_commit);
            fclose(stage);
            n++;
        }
        fclose(stagedlist);
        remove(".neogit/commitlist.txt");
        rename(".neogit/commitlisttmp.txt",".neogit/commitlist.txt");
        fclose(commitlist);
        fclose(commitlisttmp);
                /*DIR* stage=opendir(".neogit/stage");
                struct dirent* ent;
                while((ent=readdir(stage))!=NULL){
                    if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                        char filename[1024];
                        strcpy(filename,ent->d_name);
                        char filepath[1024]=".neogit/commit/";
                        strcat(filepath,branch);
                        strcat(filepath,"/");
                        strcat(filepath,ent->d_name);
                        DIR* commitfolder=opendir(filepath);
                        if(commitfolder==NULL)
                            mkdir(filepath,0755);
                        strcat(filepath,"/");
                        char commitstr[50];
                        sprintf(commitstr,"%d",commit_ID);
                        strcat(filepath,commitstr);
                        strcat(filepath,".txt");
                        FILE* new_commit=fopen(filepath,"w");
                        char stagedfilepath[1024]=".neogit/stage/";
                        strcat(stagedfilepath,ent->d_name);
                        FILE* stagedfile=fopen(filepath,"r");
                        char line[MAX_LINE_LENGTH];
                        while((fgets(line,MAX_LINE_LENGTH,stagedfile))!=NULL)
                            fputs(line,new_commit);
                        remove(stagedfilepath);
                        n++;
                        fclose(stagedfile);
                        closedir(commitfolder);
                    }
                }
                
                /*FILE* list=fopen(".neogit/stagedlist.txt","r");
                char filename[MAX_LINE_LENGTH];
                char filepath[MAX_LINE_LENGTH];
                while((fgets(filename,MAX_LINE_LENGTH,list))!=NULL){
                    fgets(filepath,MAX_LINE_LENGTH,list);
                    filename[strlen(filename)-1]='\0';
                    filepath[strlen(filepath)-1]='\0';
                    FILE* stage=fopen(filepath,"r");
                    char commitpath[1024]=".neogit/commit/";
                    FILE* branch=fopen(".neogit/branch.txt","r");
                    char branchname[30];
                    fgets(branchname,30,branch);
                    fclose(branch);
                    strcat(commitpath,branchname);
                    strcat(commitpath,"/");
                    strcat(commitpath,filename);
                    DIR* commitdir=opendir(commitpath);
                    if(commitdir==NULL)
                        mkdir(commitpath,0755);
                    strcat(commitpath,"/");
                    char commitidstr[50];
                    sprintf(commitidstr,"%d",commit_ID);
                    strcat(filepath,commitidstr);
                    strcat(filepath,".txt");
                    FILE* new_commit=fopen(filepath,"w");
                    char line[MAX_LINE_LENGTH];
                    while((fgets(line,MAX_LINE_LENGTH,stage))!=NULL)
                        fprintf(new_commit,"%s",line);
                    fclose(stage);
                    fclose(new_commit);
                    closedir(commitdir);
                    n++;
                }
                fclose(list);*/
                /*if(n==0){
                    printf("there is no file to commit!\n");
                    return 1;
                }
                FILE* tmpp=fopen(".neogit/stagedlist.txt","w");//formating staged list
                fclose(tmpp);
                FILE* tmppp=fopen(".neogit/lastadd.txt","w");//firmating last add
                fclose(tmppp);
                FILE* commitcount=fopen(".neogit/commitcount.txt","r");
                FILE* commitcounttmp=fopen(".neogit/commitcounttmp.txt","w");
                char line[MAX_LINE_LENGTH];
                fgets(line,MAX_LINE_LENGTH,commitcount);
                sscanf(line,"%d",&commit_ID);
                fprintf(commitcounttmp,"%d",commit_ID+1);
                remove(".neogit/commitcount.txt");
                rename(".neogit/commitcounttmp.txt",".neogit/commitcount.txt");
                fclose(commitcount);
                fclose(commitcounttmp);
                FILE* branchf=fopen(".neogit/branch.txt","r");
                fscanf(branchf,"%s",branch);
                fclose(branchf);
                FILE* config=fopen(".neogit/config.txt","r");
                fgets(line,MAX_LINE_LENGTH,config);
                fgets(line,MAX_LINE_LENGTH,config);
                sscanf(line,"%s %d %s",tmp,&user,username);
                if(user==0){
                    FILE* globalconfig=fopen(globalpath,"r");
                    fgets(line,MAX_LINE_LENGTH,globalconfig);
                    fgets(line,MAX_LINE_LENGTH,globalconfig);
                    sscanf(line,"%s %d %s",tmp,&user,username);
                    fclose(globalconfig);
                }
                fclose(config);
                char timedate[1000];
                time_t t = time(NULL);                          ////time here
                struct tm tm = *localtime(&t);
                sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+1, tm.tm_sec+30);  
                FILE* log=fopen(".neogit/log.txt","r");
                FILE* tmplog=fopen(".neogit/logtmp.txt","w");
                fprintf(tmplog,"#commit ID: %d\n time:%s message: %s\n author: %s\n branch: %s\n number of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                printf("commited successfully!\ncommit ID: %d\ntime:%s message: %s\nauthor: %s\nbranch: %s\nnumber of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
                while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                    fputs(line,tmplog);
                remove(".neogit/log.txt");
                rename(".neogit/logtmp.txt",".neogit/log.txt");
                fclose(log);
                fclose(tmplog);
            
        }*/
    else if(strcmp(argvtmp[1],"log")==0){
        if(argctmp==2){
            FILE* log=fopen(".neogit/log.txt","r");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,log))!=NULL)
                printf("%s",line);
        }
        else if(strcmp(argvtmp[2],"-n")==0 && argctmp==4){
            int n=atoi(argvtmp[3]);
            int i=0;
            FILE* log=fopen(".neogit/log.txt","r");
            char line[MAX_LINE_LENGTH];
            while(i<n){
                char number[10];
                fgets(line,MAX_LINE_LENGTH,log);
                sscanf(line," %s",number);
                if(strcmp(number,"number")==0) i++;
                printf("%s",line);
            }
        }
        else if(strcmp(argvtmp[2],"-branch")==0){
            int exist=0;
            DIR* dir=opendir(".neogit/commit");
            struct dirent* ent;
            while((ent=readdir(dir))!=NULL)
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0)
                    if(strcmp(ent->d_name,argvtmp[3])==0){
                        exist=1;
                        break;
                    }
            closedir(dir);
            if(argctmp!=4){
                printf("you forgot to type the branch name!\n");
            }
            else if(exist==0){
                printf("branch \"%s\" doesn't exist!\n",argvtmp[3]);
            }
            else{
                char branch[20];
                strcpy(branch,argvtmp[3]);
                FILE* log=fopen(".neogit/log.txt","r");
                char line1[MAX_LINE_LENGTH];
                while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                   char line2[MAX_LINE_LENGTH]; 
                   char line3[MAX_LINE_LENGTH];
                   char line4[MAX_LINE_LENGTH];
                   char line5[MAX_LINE_LENGTH];
                   char line6[MAX_LINE_LENGTH];
                   fgets(line2,MAX_LINE_LENGTH,log);
                   fgets(line3,MAX_LINE_LENGTH,log);
                   fgets(line4,MAX_LINE_LENGTH,log);
                   fgets(line5,MAX_LINE_LENGTH,log);
                   fgets(line6,MAX_LINE_LENGTH,log);
                   char chert[10];
                   char branchname[10];
                   sscanf(line5," %s %s\n",chert,branchname);
                   if(strcmp(branchname,branch)==0){
                    printf("%s",line1);
                    printf("%s",line2);
                    printf("%s",line3);
                    printf("%s",line4);
                    printf("%s",line5);
                    printf("%s",line6);
                   }
                }
                fclose(log);
            }
        }
        else if(strcmp(argvtmp[2],"-author")==0){
            FILE* log=fopen(".neogit/log.txt","r");
            char line1[MAX_LINE_LENGTH];
            while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                char line2[MAX_LINE_LENGTH]; 
                char line3[MAX_LINE_LENGTH];
                char line4[MAX_LINE_LENGTH];
                char line5[MAX_LINE_LENGTH];
                char line6[MAX_LINE_LENGTH];
                fgets(line2,MAX_LINE_LENGTH,log);
                fgets(line3,MAX_LINE_LENGTH,log);
                fgets(line4,MAX_LINE_LENGTH,log);
                fgets(line5,MAX_LINE_LENGTH,log);
                fgets(line6,MAX_LINE_LENGTH,log);
                char chert[10];
                char author[10];
                sscanf(line4," %s %s\n",chert,author);
                if(strcmp(author,argvtmp[3])==0){
                    printf("%s",line1);
                    printf("%s",line2);
                    printf("%s",line3);
                    printf("%s",line4);
                    printf("%s",line5);
                    printf("%s",line6);
                }
                }
            fclose(log);
        }
        else if(strcmp(argvtmp[2],"-search")==0){
            char words[10][10];
            char* pch=strtok(argvtmp[3]," \0");
            int j=0;
            while(pch){
                strcpy(words[j++],pch);
                pch=strtok(NULL," \0");
            }
            FILE* log=fopen(".neogit/log.txt","r");
            char line1[MAX_LINE_LENGTH];
            while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
                char line2[MAX_LINE_LENGTH]; 
                char line3[MAX_LINE_LENGTH];
                char line4[MAX_LINE_LENGTH];
                char line5[MAX_LINE_LENGTH];
                char line6[MAX_LINE_LENGTH];
                fgets(line2,MAX_LINE_LENGTH,log);
                fgets(line3,MAX_LINE_LENGTH,log);
                fgets(line4,MAX_LINE_LENGTH,log);
                fgets(line5,MAX_LINE_LENGTH,log);
                fgets(line6,MAX_LINE_LENGTH,log);
                int print=0;
                for(int i=0;i<j;i++) {
                    if(strstr(line3,words[i])!=NULL) print=1;
                }
                if(print){
                    printf("%s",line1);
                    printf("%s",line2);
                    printf("%s",line3);
                    printf("%s",line4);
                    printf("%s",line5);
                    printf("%s",line6);
                }
                }
            fclose(log);
        }
        else if(strcmp(argvtmp[2],"-since")==0 || strcmp(argvtmp[2],"-before")==0){
            int yearidx;
            int monthidx;
            int dayidx;
            int houridx;
            int minidx;
            int secidx;
            sscanf(argvtmp[3],"%d-%d-%d %d:%d:%d",&yearidx,&monthidx,&dayidx,&houridx,&minidx,&secidx); 
            //printf("%d-%d-%d %d:%d:%d",yearidx,monthidx,dayidx,houridx,minidx,secidx);
            //unsigned long long secondsidx=yearidx*365*24*3600+monthidx*30*24*3600+dayidx*24*3600+houridx*3600+minidx*60+secidx;
            //printf("%lld\n",secondsidx);
            FILE* log=fopen(".neogit/log.txt","r");
            int n=0;
            char line1[MAX_LINE_LENGTH];
            while((fgets(line1,MAX_LINE_LENGTH,log))!=NULL){
            int bigger=0;
            char line2[MAX_LINE_LENGTH]; 
            char line3[MAX_LINE_LENGTH];
            char line4[MAX_LINE_LENGTH];
            char line5[MAX_LINE_LENGTH];
            char line6[MAX_LINE_LENGTH];
            fgets(line2,MAX_LINE_LENGTH,log);
            fgets(line3,MAX_LINE_LENGTH,log);
            fgets(line4,MAX_LINE_LENGTH,log);
            fgets(line5,MAX_LINE_LENGTH,log);
            fgets(line6,MAX_LINE_LENGTH,log);
            char chert[30];
            char date[30];
            char time[30];
            sscanf(line2," %s %s %s\n",chert,date,time);
            //printf(" chert:%s date:%s time:%s\n",chert,date,time);
            int year;
            int month;
            int day;
            int hour;
            int min;
            int sec;
            sscanf(date,"%d-%d-%d",&year,&month,&day); 
            sscanf(time,"%d:%d:%d",&hour,&min,&sec);
            // printf("%d-%d-%d",year,month,day);
            // printf("%d:%d:%d",hour,min,sec);
            if(year>yearidx) bigger=1;
            else if(year==yearidx){
                if(month>monthidx) bigger=1;
                else if(month==monthidx){
                    if(day>dayidx) bigger=1;
                    else if(day==dayidx){
                        if(hour>houridx) bigger=1;
                        else if(hour==houridx){
                            if(min>minidx) bigger=1;
                            else if(min==minidx){
                                if(sec>=secidx) bigger=1;
                            }
                        }
                    }
                }
            }
            if(strcmp(argvtmp[2],"-since")==0){
                if(bigger){
                    printf("%s",line1);
                    printf("%s",line2);
                    printf("%s",line3);
                    printf("%s",line4);
                    printf("%s",line5);
                    printf("%s",line6);
                }
            }
            else if(strcmp(argvtmp[2],"-before")==0){
               if(bigger==0){
                    printf("%s",line1);
                    printf("%s",line2);
                    printf("%s",line3);
                    printf("%s",line4);
                    printf("%s",line5);
                    printf("%s",line6);
                } 
            }
        }
    }
    
    }

    
    else if(strcmp(argvtmp[1],"status")==0){
        DIR* curren_dir=opendir(".");
        struct dirent* ent;
        while((ent=readdir(curren_dir))!=NULL){
            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0 && ent->d_type==8){
                //printf("%s: ",ent->d_name);
                char done=0;
                char mode;
                char dirpath[1024];
                strcpy(dirpath,repositorypath);
                strcat(dirpath,"/stage");
                DIR* stage=opendir(dirpath);
                struct dirent* stage_ent;
                while((stage_ent=readdir(stage))!=NULL && done==0){//check it in stage
                   if(strcmp(stage_ent->d_name,".")!=0 && strcmp(stage_ent->d_name,"..")!=0){
                    char changed[1024];
                    strcpy(changed,stage_ent->d_name);
                    for(int i=0;i<strlen(changed);i++) if(changed[i]=='^') changed[i]='/';
                    if(strcmp(changed,ent->d_name)==0){
                        char stagepath[1024];
                        strcpy(stagepath,dirpath);
                        strcat(stagepath,"/");
                        strcat(stagepath,changed);
                        char actualpath [1024];
                        if(modified(realpath(ent->d_name,actualpath),stagepath)==0){
                            printf("%s :+M\n",ent->d_name);
                            done=1;
                            break;
                        }
                        else done=1;   
                    }
                   }
                }
                char commitpath[1024];
                strcpy(commitpath,repositorypath);
                strcat(commitpath,"/commit");
                char branch[30];
                char tmppath[1024];
                strcpy(tmppath,repositorypath);
                strcat(tmppath,"/branch.txt");
                FILE* branchf=fopen(tmppath,"r");
                fgets(branch,30,branchf);
                strcat(commitpath,"/");
                strcat(commitpath,branch);
                strcat(commitpath,"/");
                strcat(commitpath,ent->d_name);
                if(isDirectory(commitpath) && done==0){//check it in commit
                    struct dirent* getfirst;
                    DIR* dir=opendir(commitpath);
                    while((getfirst=readdir(dir))!=NULL){
                        if(strcmp(getfirst->d_name,".")!=0 && strcmp(getfirst->d_name,"..")!=0){
                            char path[1024];
                            strcpy(path,commitpath);
                            strcat(path,"/");
                            strcat(path,getfirst->d_name);
                            char actualpath[1024];
                            if(modified(path,realpath(ent->d_name,actualpath))==0){
                                printf("%s :+M\n",ent->d_name);
                                done=1;
                            }
                            else done=1;
                            break;
                        }
                    }
                }
                if(done==0){
                    printf("%s :-A\n",ent->d_name);
                }       
            }
        }
        //int deleted=1;
        char line[1000];
        char filepath[1024];
        char tmppath[1024];
        strcpy(tmppath,repositorypath);
        strcat(tmppath,"/stagedlist.txt");
        FILE* stagelist=fopen(tmppath,"r");
        /*while((fgets(line,1000,stagelist))!=NULL){
            int deleted=1;
            fgets(line,1000,stagelist);
            fgets(filepath,1024,stagelist);
            //printf("$%s\n",filepath);
            filepath[strlen(filepath)-1]='\0';
            if(is_regular_file(filepath)) {
                deleted=0; 
                break;
            }
            if(deleted){
            printf("%s: +D\n",filepath);
        }
        }*/
        fclose(stagelist);
        char branch[30];
        char dirpath[1024]=".neogit/commit/";
        FILE* fbranch=fopen(".neogit/branch.txt","r");
        fscanf(fbranch,"%s",branch);
        strcat(dirpath,branch);
        DIR* stagedir=opendir(dirpath);
        struct dirent* enti;
        while((enti=readdir(stagedir))!=NULL){
            if(strcmp(enti->d_name,".")!=0 && strcmp(enti->d_name,"..")!=0){
                int deleted=1;
                char real[1024];
                strcpy(real,enti->d_name);
                for(int i=0;i<strlen(real);i++) if(real[i]=='^') real[i]='/';
                if(is_regular_file(real)){
                    deleted=0;
                }
                if(deleted){
                printf("%s: +D\n",real);
                }
            }
        }
        
        
    }
    else if(strcmp(argvtmp[1],"branch")==0){
        if(argctmp==2){
            printf("list of branches in your repository:\n");
            char pth[1024];
            strcpy(pth,repositorypath);
            strcat(pth,"/commit");
            DIR* commit=opendir(pth);
            struct dirent* ent;
            while((ent=readdir(commit))!=NULL)
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0)
                    printf("   %s\n",ent->d_name);
        }
        else if(argctmp==3){
            int exist=0;
            char pth[1024];
            strcpy(pth,repositorypath);
            strcat(pth,"/commit");
            DIR* commit=opendir(pth);
            struct dirent* ent;
            while((ent=readdir(commit))!=NULL)
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    if(strcmp(argvtmp[2],ent->d_name)==0){
                        exist=1;
                        break;
                    }
                }
            if(exist){
                printf("this branch already exists, try another name!\n");
                return 1;
            }
            strcat(pth,"/");
            strcat(pth,argvtmp[2]);
            mkdir(pth,0755);//folder for new branch
            char previ[1024]=".neogit/commit/";
            FILE* branchf=fopen(".neogit/branch.txt","r");
            char branch[30];
            fscanf(branchf,"%s",branch);
            fclose(branchf);
            strcat(previ,branch);
            DIR* prevdir=opendir(previ);
            struct dirent* t;
            while((t=readdir(prevdir))!=NULL){
                if(strcmp(t->d_name,".")!=0 && strcmp(t->d_name,"..")!=0){
                    char to_create[1024];
                    strcpy(to_create,pth);
                    strcat(to_create,"/");
                    strcat(to_create,t->d_name);
                    mkdir(to_create,0755);
                    char prev[1024];
                    strcpy(prev,previ);
                    strcat(prev,"/");
                    strcat(prev,t->d_name);
                    DIR* get_first=opendir(prev);
                    struct dirent* ent_first;
                    char biigest[30];
                    ent_first=readdir(get_first);
                    while(ent_first!=NULL){
                        if(strcmp(ent_first->d_name,".")!=0 && strcmp(ent_first->d_name,"..")!=0){
                            strcpy(biigest,ent_first->d_name); 
                        }
                        ent_first=readdir(get_first);
                    }
                    strcat(prev,"/");
                    strcat(prev,biigest);
                    FILE* to_copy=fopen(prev,"rb");
                    char pthcpy[1024];
                    strcpy(pthcpy,pth);
                    strcat(pthcpy,"/");
                    strcat(pthcpy,t->d_name);
                    strcat(pthcpy,"/");
                    strcat(pthcpy,biigest);
                    FILE* to_paste=fopen(pthcpy,"wb");
                    char buff[BUFSIZ];
                    size_t n;
                    while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                        fwrite(buff,1,n,to_paste);
                    }
                    fclose(to_paste);
                    fclose(to_copy);
                }
            }

            printf("created branch \"%s\" successfully!\n",argvtmp[2]);

        }
    }
    else if(strcmp(argvtmp[1],"checkout")==0){
        char branch[30];
        if(strcmp(argvtmp[2],"HEAD")==0){
            FILE* fbranch=fopen(".neogit/branch.txt","r");
            fgets(branch,30,fbranch);
            fclose(fbranch);
            char head_path[1024]=".neogit/commit/";
            strcat(head_path,branch);
            DIR* searchbranch=opendir(head_path);
            struct dirent* t;
            while((t=readdir(searchbranch))!=NULL){
                if(strcmp(t->d_name,".")!=0 && strcmp(t->d_name,"..")!=0){
                    char tmppath[1024];
                    strcpy(tmppath,head_path);
                    strcat(tmppath,"/");
                    strcat(tmppath,t->d_name);
                    DIR* get_first=opendir(tmppath);
                    struct dirent* ent_first;
                    char biigest[30];
                    ent_first=readdir(get_first);
                    while(ent_first!=NULL){
                        if(strcmp(ent_first->d_name,".")!=0 && strcmp(ent_first->d_name,"..")!=0){
                            strcpy(biigest,ent_first->d_name); 
                        }
                        ent_first=readdir(get_first);
                    }
                    strcat(tmppath,"/");
                    strcat(tmppath,biigest);
                    FILE* to_copy=fopen(tmppath,"rb");
                    char path[1024];
                    strcpy(path,t->d_name);
                    for(int i=0;i<strlen(path);i++) if(path[i]=='^') path[i]='/';
                    FILE* to_paste=fopen(path,"wb");
                    char buff[BUFSIZ];
                    size_t n;
                    while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                        fwrite(buff,1,n,to_paste);
                    }
                    fclose(to_paste);
                    fclose(to_copy);
                }
            }
            return 1;
        }
        char path[1024];
        strcpy(path,repositorypath);
        strcat(path,"/stagedlist.txt");
        char line[MAX_LINE_LENGTH];
        FILE* test=fopen(path,"r");
        if(fgets(line,MAX_LINE_LENGTH,test)!=NULL){
            printf("you can't checkout without commiting files in your staging area!\n");
            return 1;
        }
        fclose(test);
        int commit_ID=atoi(argvtmp[2]);
        char line1[MAX_LINE_LENGTH];
        if(commit_ID==0){
            FILE* commitlist=fopen(".neogit/commitlist.txt","r");
            while((fgets(line1,MAX_LINE_LENGTH,commitlist))!=NULL){
                int tmp_commit_ID;
                char filepath[1024];
                sscanf(line1,"%d %s %s\n",&tmp_commit_ID,branch,filepath);
                if(strcmp(branch,argvtmp[2])==0){//this way we find the commit ID we were looking for
                    commit_ID=tmp_commit_ID;
                    break;
                }
            }
            fclose(commitlist);
        }
        else if(commit_ID!=0){
            FILE* commitlist=fopen(".neogit/commitlist.txt","r");
            while((fgets(line1,MAX_LINE_LENGTH,commitlist))!=NULL){
                int tmp_commit_ID;
                char filepath[1024];
                sscanf(line1,"%d %s %s\n",&tmp_commit_ID,branch,filepath);///////////////////////////////////////////////?
                if(tmp_commit_ID==commit_ID) break;//this way we find the branch name
            }
            fclose(commitlist);
        }
        char to_search[1024]=".neogit/commit/";
        strcat(to_search,branch);
        DIR* search_commits=opendir(to_search);
        struct dirent* ent;
        while((ent=readdir(search_commits))!=NULL){
            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                char tmppath[1024];
                strcpy(tmppath,to_search);
                strcat(tmppath,"/");
                strcat(tmppath,ent->d_name);
                DIR* search_for_id=opendir(tmppath);
                struct dirent* ent_ID;
                ent_ID=readdir(search_for_id);
                int last=atoi(ent_ID->d_name);
                char go_to[30];
                strcpy(go_to,ent_ID->d_name);
                while(ent_ID!=NULL){
                    if(strcmp(ent_ID->d_name,".")!=0 && strcmp(ent_ID->d_name,"..")!=0){
                        int n=atoi(ent_ID->d_name);
                        if(n<=commit_ID && n>last){
                            last=n;
                            strcpy(go_to,ent_ID->d_name);
                        }
                    }
                    ent_ID=readdir(search_for_id);
                }
                strcat(tmppath,"/");
                strcat(tmppath,go_to);
                FILE* to_copy=fopen(tmppath,"rb");
                char aslipath[1024];
                strcpy(aslipath,ent->d_name);
                for(int i=0;i<strlen(aslipath);i++) if(aslipath[i]=='^') aslipath[i]='/';
                FILE* to_paste=fopen(aslipath,"wb");
                char buff[BUFSIZ];
                size_t n;
                while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                    fwrite(buff,1,n,to_paste);
                }
                fclose(to_paste);
                fclose(to_copy);
                
            }
        }
        FILE* branchf=fopen(".neogit/branch.txt","w");
        fprintf(branchf,"%s",branch);
        fclose(branchf);
    }///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    else if(strcmp(argvtmp[1],"revert")==0){
        //printf("*****");
        int merge_error=0;
        char message[72];
        int commit_ID;
        int ID_TO_GO;
        char branch[30];
        char username[50];
        char tmp[20];
        int n=0;
        int user;
        int mesasge_avail=0;
        FILE* commitcount=fopen(".neogit/commitcount.txt","r");
        fscanf(commitcount,"%d",&commit_ID);//this is the previous commit ID
        fclose(commitcount);
        //printf("%d",commit_ID);
        if(strcmp(argvtmp[2],"-n")==0){
            if(argctmp==3){
                ID_TO_GO=commit_ID;
            }
            else ID_TO_GO=atoi(argvtmp[3]);   
        }
        else if(strcmp(argvtmp[2],"-m")==0){
            strcpy(message,argvtmp[3]);
            mesasge_avail=1;
            if(strncmp(argvtmp[4],"HEAD_",5)==0){
                int n;
                sscanf(argvtmp[4],"HEAD_%d",&n);
                ID_TO_GO=commit_ID-n;
                printf("back: %d",n);
            }
            else ID_TO_GO=atoi(argvtmp[4]);
        }
        else if(strcmp(argvtmp[2],"-m")!=0){
            if(strncmp(argvtmp[2],"HEAD_",5)==0){
                int back;
                sscanf(argvtmp[2],"HEAD_%d",&back);
                ID_TO_GO=commit_ID-back;
                printf("back: %d",back);
            }
            else{
                ID_TO_GO=atoi(argvtmp[2]);
            }
        }
        if(strcmp(argvtmp[2],"-n")!=0){
            FILE* log=fopen(".neogit/log.txt","r");
            char line[MAX_LINE_LENGTH];
            char line2[MAX_LINE_LENGTH];
            char line3[MAX_LINE_LENGTH];
            char line4[MAX_LINE_LENGTH];
            char line5[MAX_LINE_LENGTH];
            char line6[MAX_LINE_LENGTH];
            char cher[20];
            int tmp_ID;
            //printf("ID_TO_GO:%d\n",ID_TO_GO);
            while((fgets(line,MAX_LINE_LENGTH,log))!=NULL){
                //printf("*****%s\n",line);
                fgets(line2,MAX_LINE_LENGTH,log);
                fgets(line3,MAX_LINE_LENGTH,log);
                fgets(line4,MAX_LINE_LENGTH,log);
                fgets(line5,MAX_LINE_LENGTH,log);
                fgets(line6,MAX_LINE_LENGTH,log);
                sscanf(line,"#commit ID: %d\n",&tmp_ID);
                //printf("%d\n",tmp_ID);
                if(tmp_ID==ID_TO_GO){
                    if(mesasge_avail==0){; 
                        sscanf(line3,"%s %s\n",cher,message);
                    }
                    char cher[20];
                    sscanf(line5,"%s %s",cher,branch);
                    printf("yess");
                }
            }
            fclose(log);
        }
        //printf("message:%s\n",message);
        commit_ID++;
        int tmp_ID;
        char cher[30];
        char chert[30];
        FILE* commitlist=fopen(".neogit/commitlist.txt","r");
        char line[MAX_LINE_LENGTH];
        while((fgets(line,MAX_LINE_LENGTH,commitlist))!=NULL){
            if(strncmp(line,"merge",5)==0) merge_error=1;
            sscanf(line,"%d",&tmp_ID);
            if(ID_TO_GO==tmp_ID) break;
        }
        if(merge_error){
            printf(BRED "merge error!\n");
            return 1;
        }
        fclose(commitlist);
        FILE* branchf=fopen(".neogit/branch.txt","w");
        fprintf(branchf,"%s",branch);
        fclose(branchf);
        FILE* commitlisttmp=fopen(".neogit/commitlisttmp.txt","w");
        FILE* committree=fopen(".neogit/committree.txt","a");
        char to_search[1024]=".neogit/commit/";
        strcat(to_search,branch);
        printf("%s",to_search);
        DIR* search_commits=opendir(to_search);
        struct dirent* t;
        while((t=readdir(search_commits))!=NULL){
            if(strcmp(t->d_name,".")!=0 && strcmp(t->d_name,"..")!=0){
                char tmppath[1024];
                strcpy(tmppath,to_search);
                strcat(tmppath,"/");
                strcat(tmppath,t->d_name);//it may include ^'s
                DIR* search_for_id=opendir(tmppath);
                struct dirent* ent_ID;
                ent_ID=readdir(search_for_id);
                int last=atoi(ent_ID->d_name);
                char go_to[30];
                strcpy(go_to,ent_ID->d_name);
                while(ent_ID!=NULL){
                    if(strcmp(ent_ID->d_name,".")!=0 && strcmp(ent_ID->d_name,"..")!=0){
                        int n=atoi(ent_ID->d_name);
                        if(n<=ID_TO_GO && n>last){
                            last=n;
                            strcpy(go_to,ent_ID->d_name);
                        }
                    }
                    ent_ID=readdir(search_for_id);
                }

                strcat(tmppath,"/");
                strcat(tmppath,go_to);
                FILE* to_copy=fopen(tmppath,"rb");
                char aslipath[1024];
                strcpy(aslipath,t->d_name);
                for(int i=0;i<strlen(aslipath);i++) if(aslipath[i]=='^') aslipath[i]='/';
                FILE* to_paste=fopen(aslipath,"wb");
                char buff[BUFSIZ];
                size_t m;
                while((m=fread(buff,1,BUFSIZ,to_copy))!=0){
                    fwrite(buff,1,m,to_paste);
                }
                fclose(to_paste);
                fclose(to_copy);
                if(strcmp(argvtmp[2],"-n")!=0){
                char new_path[1024];
                strcpy(new_path,to_search);
                strcat(new_path,"/");
                strcat(new_path,t->d_name);
                strcat(new_path,"/");
                char cur_ID[10];
                sprintf(cur_ID,"%d",commit_ID);
                strcat(new_path,cur_ID);
                char* format=strchr(t->d_name,'.');
                strcat(new_path,format);
                printf("tmppath: %s\n",tmppath);
                printf("new_path: %s\n",new_path);
                FILE* to_copy2=fopen(tmppath,"rb");
                FILE* to_paste2=fopen(new_path,"wb");
                char bufff[BUFSIZ];
                size_t q;
                while((q=fread(bufff,1,BUFSIZ,to_copy2))!=0){
                    fwrite(bufff,1,q,to_paste2);
                }
                fclose(to_copy2);
                fclose(to_paste2);
                }
                n++;
                if(strcmp(argvtmp[2],"-n")!=0){
                    fprintf(commitlisttmp,"%d %s %s\n",commit_ID,branch,t->d_name);
                }
            }
        }
        FILE* commitlist2=fopen(".neogit/commitlist.txt","r");
        while((fgets(line,MAX_LINE_LENGTH,commitlist2))!=NULL)
            fprintf(commitlisttmp,"%s",line);
        remove(".neogit/commitlist.txt");
        rename(".neogit/commitlisttmp.txt",".neogit/commitlist.txt");
        fclose(commitlist2);
        fclose(commitlisttmp);
        if(strcmp(argvtmp[2],"-n")==0 ) return 1;
        FILE* config=fopen(".neogit/config.txt","r");
        fgets(line,MAX_LINE_LENGTH,config);
        fgets(line,MAX_LINE_LENGTH,config);
        sscanf(line,"%s %d %s",tmp,&user,username);
        if(user==0){
            FILE* globalconfig=fopen(globalpath,"r");
            fgets(line,MAX_LINE_LENGTH,globalconfig);
            fgets(line,MAX_LINE_LENGTH,globalconfig);
            sscanf(line,"%s %d %s",tmp,&user,username);
            fclose(globalconfig);
        }
        fclose(config);
        FILE* commitcount2=fopen(".neogit/commitcount.txt","w");
        fprintf(commitcount2,"%d",commit_ID);
        fclose(commitcount2);
        /*char dirpath[1024]=".neogit/commit/";
        strcat(dirpath,branch);
        DIR* commmit=opendir(dirpath);
        struct dirent* ent;
        while((ent=readdir(commmit))!=NULL){
            if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                char new_path[1024];
                char* format;
                format=strchr(ent->d_name,'.');
                strcpy(new_path,dirpath);
                strcat(new_path,"/");
                strcat(new_path,ent->d_name);
                DIR* commmit_in=opendir(new_path);
                struct dirent* in_ent;
                while((in_ent=readdir(commmit_in))!=NULL){
                    if(strcmp(in_ent->d_name,".")!=0 && strcmp(in_ent->d_name,"..")!=0){
                        if(ID_TO_GO==atoi(in_ent->d_name)){
                            char old_commit[1024];
                            strcpy(old_commit,new_path);
                            strcat(old_commit,"/");
                            strcat(old_commit,in_ent->d_name);
                            FILE* to_copy=fopen(old_commit,"rb");
                            char new_commit[1024];
                            strcpy(new_commit,new_path);
                            strcat(new_commit,"/");
                            char cur_ID[20];
                            sprintf(cur_ID,"%d",commit_ID);
                            strcat(new_commit,cur_ID);
                            strcat(new_commit,format);
                            FILE* to_paste=fopen(new_commit,"wb");
                            char buff[BUFSIZ];
                            size_t n;
                            while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                                fwrite(buff,1,n,to_paste);
                            }
                            fclose(to_paste);
                            fclose(to_copy);
                        }
                    }
                }
            }
        }*/
        FILE* commitcount3=fopen(".neogit/commitcount.txt","w");
        fprintf(commitcount3,"%d",commit_ID);
        fclose(commitcount3);
        char timedate[1000];
        time_t r = time(NULL);
        struct tm tm = *localtime(&r);
        sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour+1, tm.tm_min+1, tm.tm_sec+30);  
        FILE* log1=fopen(".neogit/log.txt","r");
        FILE* tmplog=fopen(".neogit/logtmp.txt","w");
        fprintf(tmplog,"#commit ID: %d\n time:%s message: %s\n author: %s\n branch: %s\n number of files commited: %d\n",commit_ID,timedate,message,username,branch,n);
        fprintf(committree,"%d %s\n",commit_ID%100,branch);
        printf("new commit!\ncommit ID: %d\ntime:%smessage: %s\nauthor: %s\nbranch: %s\n",commit_ID,timedate,message,username,branch);
        while((fgets(line,MAX_LINE_LENGTH,log1))!=NULL)
            fputs(line,tmplog);
        remove(".neogit/log.txt");
        rename(".neogit/logtmp.txt",".neogit/log.txt");
        fclose(log1);
        fclose(tmplog);
        fclose(committree);
    }
    else if(strcmp(argvtmp[1],"tag")==0){
        if(argctmp==2){
            int total=0;
            FILE* taglist=fopen(".neogit/taglist.txt","r");
            while((fgets(line,MAX_LINE_LENGTH,taglist))!=NULL){
                char chert[20];
                sscanf(line,"%s %s\n",chert,tag[total++]);
                for(int i=0;i<4;i++) (fgets(line,MAX_LINE_LENGTH,taglist));
            }
            fclose(taglist);
            for(int i=0;i<total;i++)
            for(int j=i+i;j<total;j++)
                if(strcmp(tag[i],tag[j])>0){
                    char tmp[30];
                    strcpy(tmp,tag[i]);
                    strcpy(tag[i],tag[j]);
                    strcpy(tag[j],tmp);
                }
            printf("list of all tags:\n");
            for(int i=0;i<total;i++) printf("%s\n",tag[i]);
        }
        else if(strcmp(argvtmp[2],"show")==0){
            FILE* taglist=fopen(".neogit/taglist.txt","r");
            while((fgets(line,MAX_LINE_LENGTH,taglist))!=NULL){
                char tagnm[20];
                char chert[25];
                sscanf(line,"%s %s\n",chert,tagnm);
                if(strcmp(argvtmp[3],tagnm)==0){
                    printf("%s",line);
                    for(int i=0;i<4;i++){
                    fgets(line,MAX_LINE_LENGTH,taglist);
                    printf("%s",line);
                    }

                }
                else{
                    for(int i=0;i<4;i++) 
                    fgets(line,MAX_LINE_LENGTH,taglist);
                }
            }
        }
        else if(strcmp(argvtmp[2],"-a")==0){
            char message[72];
            int message_available=0;
            int commit_ID;
            int commit_ID_available=0;
            int overwrite=0;
            char username[50];
            char email[50];
            int email_avail;
            int user;
            char tmp[50];
            char tmp_tagname[10];
            for(int i=4;i<argctmp;i++){
                if(strcmp(argvtmp[i],"-m")==0)
                    {strcpy(message,argvtmp[i+1]);
                    message_available=1;
                    }
                if(strcmp(argvtmp[i],"-c")==0){
                    commit_ID=atoi(argvtmp[i+1]);
                    commit_ID_available=1;
                }
                if(strcmp(argvtmp[i],"-f")==0)
                    overwrite=1;
            }
            if(commit_ID_available==0){
                FILE* commitcount=fopen(".neogit/commitcount.txt","r");
                fscanf(commitcount,"%d",&commit_ID);
                commit_ID--;
                fclose(commitcount);
            }
            FILE* config=fopen(".neogit/config.txt","r");
            fgets(line,MAX_LINE_LENGTH,config);
            sscanf(line,"%s %d %s",tmp,&email_avail,email);
            if(email_avail==0){
                FILE* globalconfig=fopen(globalpath,"r");
                fgets(line,MAX_LINE_LENGTH,globalconfig);
                sscanf(line,"%s %d %s",tmp,&email_avail,email);
                fclose(globalconfig);
            }
            fgets(line,MAX_LINE_LENGTH,config);
            sscanf(line,"%s %d %s",tmp,&user,username);
            if(user==0){
                FILE* globalconfig=fopen(globalpath,"r");
                fgets(line,MAX_LINE_LENGTH,globalconfig);
                fgets(line,MAX_LINE_LENGTH,globalconfig);
                sscanf(line,"%s %d %s",tmp,&user,username);
                fclose(globalconfig);
            }
            fclose(config);
            FILE* taglisttmp=fopen(".neogit/taglisttmp.txt","w");
            FILE* taglist=fopen(".neogit/taglist.txt","r");
            while((fgets(line,MAX_LINE_LENGTH,taglist))!=NULL){
                sscanf(line,"%s %s\n",tmp,tmp_tagname);
                if(strcmp(tmp_tagname,argvtmp[3])==0 && overwrite==0){
                    printf("this tag name already exists!\n");
                    return 1;
                }
                else if(strcmp(tmp_tagname,argvtmp[3])==0 && overwrite){
                    char line2[MAX_LINE_LENGTH];
                    char line3[MAX_LINE_LENGTH];
                    char line4[MAX_LINE_LENGTH];
                    char line5[MAX_LINE_LENGTH];
                    fgets(line2,MAX_LINE_LENGTH,taglist);
                    fgets(line3,MAX_LINE_LENGTH,taglist);
                    fgets(line4,MAX_LINE_LENGTH,taglist);
                    fgets(line5,MAX_LINE_LENGTH,taglist);
                    fprintf(taglisttmp,"tag %s\n",argvtmp[3]);
                    fprintf(taglisttmp,"commit %d\n",commit_ID);
                    fprintf(taglisttmp,"Author: %s %s\n",username,email);
                    char timedate[1000];
                    time_t t = time(NULL);                          
                    struct tm tm = *localtime(&t);
                    sprintf(timedate," %d-%02d-%02d %02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min+1, tm.tm_sec+30);  
                    fprintf(taglisttmp,"Date: %s",timedate);
                    fprintf(taglisttmp,"Message: %s\n",message);
                    printf("added tag \"%s\" successfully!\n",argvtmp[3]);
                }
                else{
                    fprintf(taglisttmp,"%s",line);
                }
            }
            
            
            remove(".neogit/taglist.txt");
            rename(".neogit/taglisttmp.txt",".neogit/taglist.txt");
            fclose(taglist);
            fclose(taglisttmp);

        }
    }
    else if(strcmp(argvtmp[1],"pre-commit")==0){
        if(strcmp(argvtmp[2],"hooks")==0 && strcmp(argvtmp[3],"list")==0){
            printf("todo-check\neof-blank-space\nformat-check\nbalance-braces\nindentation-check\nstatic-error-check\nfile-size-check\ncharacter-limit\ntime-limit\n");
        }
        else if(strcmp(argvtmp[2],"applied")==0){
            FILE* hooklist=fopen(".neogit/hooklist.txt","r");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,hooklist))!=NULL){
                printf("%s",line);
            }
            fclose(hooklist);
        }
        else if(strcmp(argvtmp[2],"add")==0){
            FILE* hooklist=fopen(".neogit/hooklist.txt","a");
            fprintf(hooklist,"%s\n",argvtmp[4]);
            fclose(hooklist);
        }
        else if(strcmp(argvtmp[2],"remove")==0){
            FILE* hooklist=fopen(".neogit/hooklist.txt","r");
            FILE* hooklisttmp=fopen(".neogit/hooklisttmp.txt","w");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,hooklist))!=NULL){
                char tmp_hook[30];
                sscanf(line,"%s",tmp_hook);
                if(strcmp(tmp_hook,argvtmp[4])==0) continue;
                fprintf(hooklisttmp,"%s",line);   
            }
            remove(".neogit/hooklist.txt");
            rename(".neogit/hooklisttmp.txt",".neogit/hooklist.txt");
            fclose(hooklist);
            fclose(hooklisttmp);
        }
        else if(argctmp==2){
            DIR* stage=opendir(".neogit/stage");
            struct dirent* ent;
            while((ent=readdir(stage))!=NULL){
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    run_hook(ent->d_name);
                }
                }
            }   
        else if(strcmp(argvtmp[2],"-f")==0){
            for(int i=3;i<argctmp;i++){
                int found=0;
                if(is_regular_file(argvtmp[i])==0){
                    printf("file \"%s\" doesn't exist!\n",argvtmp[i]);
                    continue;
                }
                char copy[1024];
                strcpy(copy,argvtmp[i]);
                for(int j=0;j<strlen(copy);j++) if(copy[j]=='/') copy[j]='^';
                char stagepath[1024]=".neogit/stage/";
                strcat(stagepath,copy);
                //printf("%s\n",stagepath);
                if(is_regular_file(stagepath)==0){
                    printf("file \"%s\" isn't staged!\n",argvtmp[i]);
                    continue;
                }
                else{
                    run_hook(argvtmp[i]);
                }
            }  
        }   
    }
    else if(strcmp(argvtmp[1],"grep")==0){
        int need_line=0;
        int commit_id=0;
        int count=0;
        for(int i=6;i<argctmp;i++) if(strcmp(argvtmp[i],"-n")==0) need_line=1;
        for(int i=6;i<argctmp;i++) if(strcmp(argvtmp[i],"-c")==0) commit_id=atoi(argvtmp[i+1]);
        if(commit_id==0){
            FILE* target=fopen(argvtmp[3],"r");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,target))!=NULL){
                count++;
                char* ptr=strstr(line,argvtmp[5]);
                if(ptr!=NULL){
                    if(need_line) printf(BGRN"line< %d >:\n"reset,count);
                    int stop=1;
                    int flag=-1;
                    for(int i=0;i<strlen(line)-strlen(argvtmp[5]);i++){
                        int stop=1;
                        for(int j=0;j<strlen(argvtmp[5]);j++){
                            if(argvtmp[5][j]!=line[i+j]){
                                stop=0;
                            }    
                        }
                        if(stop) flag=i;
                    }
                    if(flag!=-1){
                        int head=0;
                        while(head<flag)
                            printf("%c",line[head++]);
                        printf(BMAG"%s"reset, argvtmp[5]);
                        printf("%s",line+flag+strlen(argvtmp[5]));
                    }
                }
            }
        }
        else{
            for(int i=0;i<strlen(argvtmp[3]);i++) if(argvtmp[3][i]=='/') argvtmp[3][i]=='^';
            FILE* commitlist=fopen(".neogit/commitlist.txt","r");
            char line1[MAX_LINE_LENGTH];
            char branch[30];
            while((fgets(line1,MAX_LINE_LENGTH,commitlist))!=NULL){
                int tmp_ID;
                sscanf(line1,"%d %s",&tmp_ID,branch);
                if(tmp_ID==commit_id) break;
            }
            char path[1024]=".neogit/commit/";
            strcat(path,branch);
            strcat(path,"/");
            strcat(path,argvtmp[3]);
            strcat(path,"/");
            char cur_ID[20];
            sprintf(cur_ID,"%d",commit_id);
            strcat(path,cur_ID);
            char* format=strchr(argvtmp[3],'.');
            strcat(path,format);
            fclose(commitlist);
            FILE* target=fopen(path,"r");
            char line[MAX_LINE_LENGTH];
            while((fgets(line,MAX_LINE_LENGTH,target))!=NULL){
                count++;
                char* ptr=strstr(line,argvtmp[5]);
                if(ptr!=NULL){
                    if(need_line) printf(BGRN"line< %d >:\n"reset,count);
                    int stop=1;
                    int flag=-1;
                    for(int i=0;i<strlen(line)-strlen(argvtmp[5]);i++){
                        int stop=1;
                        for(int j=0;j<strlen(argvtmp[5]);j++){
                            if(argvtmp[5][j]!=line[i+j]){
                                stop=0;
                            }    
                        }
                        if(stop) flag=i;
                    }
                    if(flag!=-1){
                        int head=0;
                        while(head<flag)
                            printf("%c",line[head++]);
                        printf(BMAG"%s"reset, argvtmp[5]);
                        printf("%s",line+flag+strlen(argvtmp[5]));
                    }
                }
            }
        }
    }
    else if(strcmp(argvtmp[1],"diff")==0){
        if(strcmp(argvtmp[2],"-f")==0){
            int end=1000;
            //printf("8**");
            char line[MAX_LINE_LENGTH];
            int start=1;
            int count1,count2;
            if(strcmp(argvtmp[5],"-line1")==0 && strcmp(argvtmp[7],"-line2")==0){
                end=atoi(argvtmp[6]);
                start=atoi(argvtmp[8]);
            }
                FILE* file1=fopen(argvtmp[3],"r");
                FILE* file2=fopen(argvtmp[4],"r");
                //printf("ikgve");
                for(int i=1;i<start;i++) fgets(line,MAX_LINE_LENGTH,file1);
                for(int i=1;i<start;i++) fgets(line,MAX_LINE_LENGTH,file2);
                count1=start;
                count2=start;
                char line1[100];
                char line2[100];
                char line1cpy[100];
                char line2cpy[100];
                printf(BWHT"«««««\n"reset);
                while((fgets(line1,MAX_LINE_LENGTH,file1))!=NULL&&count1<=end||(fgets(line2,MAX_LINE_LENGTH,file2))!=NULL && count2<=end){
                    count1++;
                    count2++;
                    if(count1<=end && count2<=end){
                    strcpy(line1cpy,line1);
                    strcpy(line2cpy,line2);
                    while(is_empty(line1)){
                        fgets(line1,MAX_LINE_LENGTH,file1);
                        strcpy(line1cpy,line1);
                        count1++;
                    }
                    while(is_empty(line2)){
                        fgets(line2,MAX_LINE_LENGTH,file2);
                        strcpy(line2cpy,line2);
                        count2++;
                    }
                    for (int t=0;t<1000;t++)for (int i=0;i<strlen(line1cpy);i++) if(line1cpy[i]==' ' && (line1cpy[i+1]==' ' || line1cpy[i+1]=='\0')) for(int j=i;j<strlen(line1cpy)+1;j++) line1cpy[j]=line1cpy[j+1];
                    for (int t=0;t<1000;t++)for (int i=0;i<strlen(line2cpy);i++) if(line2cpy[i]==' ' && (line2cpy[i+1]==' ' || line2cpy[i+1]=='\0')) for(int j=i;j<strlen(line2cpy)+1;j++) line2cpy[j]=line2cpy[j+1];
                    if(count1<=end || count2<=end){
                        if(strcmp(line1cpy,line2cpy)!=0){
                            //printf(BWHT"«««««\n"reset);
                            printf(BWHT"<%s>-<%d>\n"reset,argvtmp[3],count1);
                            printf(BBLU"%s\n"reset,line1);
                            printf(BWHT"<%s>-<%d>\n"reset,argvtmp[4],count2);
                            printf(BCYN"%s\n"reset,line2);
                            //printf("%d",strlen(line2));
                            //printf(BWHT"»»»»»\n"reset);

                        }
                    }
                    }
                
                }
                fclose(file2);
                fclose(file1);
                printf(BWHT"»»»»»\n"reset);
        }
        else if(strcmp(argvtmp[2],"-c")==0){
            printf(BWHT"«««««\n"reset);
            char dirpath1[1024]=".neogit/commit/";
            char dirpath[1024]=".neogit/commit/";
            FILE* branchf=fopen(".neogit/branch.txt","r");
            char branch[30];
            fscanf(branchf,"%s",branch);
            fclose(branchf);
            strcat(dirpath1,branch);
            strcat(dirpath,branch);
            strcat(dirpath,"/");
            DIR* commitdir=opendir(dirpath1);
            struct dirent* ent;
            while((ent=readdir(commitdir))!=NULL){
                if(strcmp(ent->d_name,".")!=0 && strcmp(ent->d_name,"..")!=0){
                    char* format;
                    format=strchr(ent->d_name,'.');
                    char path1[1024];
                    char path2[1024];
                    strcpy(path1,dirpath);
                    strcpy(path2,dirpath);
                    strcat(path1,ent->d_name);
                    strcat(path2,ent->d_name);
                    strcat(path1,"/");
                    strcat(path2,"/");
                    strcat(path1,argvtmp[3]);
                    strcat(path2,argvtmp[4]);
                    strcat(path1,format);
                    strcat(path2,format);
                    //printf("path1:%s\npath2:%s\n",path1,path2);
                    if(is_regular_file(path1)!=1 && is_regular_file(path2)==1){
                        FILE* file2=fopen(path2,"r");
                        char line[MAX_LINE_LENGTH];
                        char to_show1[1024];
                        strcpy(to_show1,ent->d_name);
                        for(int i=0;i<strlen(to_show1);i++) if(to_show1[i]=='^') to_show1[i]='/';
                        printf(BWHT"<%s-%s>\n"reset,to_show1,argvtmp[4]);
                        while((fgets(line,MAX_LINE_LENGTH,file2))!=NULL) printf(BBLU"%s\n"reset,line);
                        fclose(file2);
                    }
                    else if(is_regular_file(path2)!=1 && is_regular_file(path1)==1){
                        FILE* file1=fopen(path1,"r");
                        char line[MAX_LINE_LENGTH];
                        char to_show2[1024];
                        strcpy(to_show2,ent->d_name);
                        for(int i=0;i<strlen(to_show2);i++) if(to_show2[i]=='^') to_show2[i]='/';
                        printf(BWHT"<%s-%s>\n"reset,to_show2,argvtmp[3]);
                        while((fgets(line,MAX_LINE_LENGTH,file1))!=NULL) printf(BCYN"%s\n"reset,line);
                        fclose(file1);
                    }
                    else if(is_regular_file(path2)==1 && is_regular_file(path1)==1){
                        char line1[100];
                        char line2[100];
                        char line1cpy[100];
                        char line2cpy[100];
                        FILE* File1=fopen(path1,"r");
                        FILE* File2=fopen(path2,"r");
                        int count1=0;
                        int count2=0;
                        while((fgets(line1,MAX_LINE_LENGTH,File1))!=NULL||(fgets(line2,MAX_LINE_LENGTH,File2))!=NULL){
                            count1++;
                            count2++;
                            strcpy(line1cpy,line1);
                            strcpy(line2cpy,line2);
                            while(is_empty(line1)){
                                fgets(line1,MAX_LINE_LENGTH,File1);
                                strcpy(line1cpy,line1);
                                count1++;
                            }
                            while(is_empty(line2)){
                                fgets(line2,MAX_LINE_LENGTH,File2);
                                strcpy(line2cpy,line2);
                                count2++;
                            }
                            for (int t=0;t<1000;t++)for (int i=0;i<strlen(line1cpy);i++) if(line1cpy[i]==' ' && (line1cpy[i+1]==' ' || line1cpy[i+1]=='\0')) for(int j=i;j<strlen(line1cpy)+1;j++) line1cpy[j]=line1cpy[j+1];
                            for (int t=0;t<1000;t++)for (int i=0;i<strlen(line2cpy);i++) if(line2cpy[i]==' ' && (line2cpy[i+1]==' ' || line2cpy[i+1]=='\0')) for(int j=i;j<strlen(line2cpy)+1;j++) line2cpy[j]=line2cpy[j+1];
                            if(strcmp(line1cpy,line2cpy)!=0){
                                char to_show2[1024];
                                strcpy(to_show2,ent->d_name);
                                for(int i=0;i<strlen(to_show2);i++) if(to_show2[i]=='^') to_show2[i]='/';
                                char to_show1[1024];
                                strcpy(to_show1,ent->d_name);
                                for(int i=0;i<strlen(to_show1);i++) if(to_show1[i]=='^') to_show1[i]='/';
                                printf(BWHT"<%s-%s>-<%d>\n"reset,to_show1,argvtmp[3],count1);
                                printf(BBLU"%s\n"reset,line1);
                                printf(BWHT"<%s-%s>-<%d>\n"reset,to_show2,argvtmp[4],count2);
                                printf(BCYN"%s\n"reset,line2);
                            }
                            
                
                    }
                    fclose(File1);
                    fclose(File2);
                    }
                    else if(is_regular_file(path2)!=1 && is_regular_file(path1)!=1) continue;
                    
                }

            }
           printf(BWHT"»»»»»\n"reset); 
        }
    }
    else if(strcmp(argvtmp[1],"merge")==0){
        char dir1[1024]=".neogit/commit/";
        strcat(dir1,argvtmp[3]);
        char dir2[1024]=".neogit/commit/";
        strcat(dir2,argvtmp[4]);
        char new_b_name[70]=".neogit/commit/";
        strcat(new_b_name,argvtmp[3]);
        strcat(new_b_name,"-");
        strcat(new_b_name,argvtmp[4]);
        mkdir(new_b_name,0755);
        DIR* dire1=opendir(dir1);
        struct dirent* ent1;
        while((ent1=readdir(dire1))!=NULL){
            if(strcmp(ent1->d_name,".")!=0 && strcmp(ent1->d_name,"..")!=0){
            char check[1024];
            strcpy(check,dir2);
            strcat(check,"/");
            strcat(check,ent1->d_name);
            if(isDirectory(check)!=1){
                char new_indir[1024];
                strcpy(new_indir,new_b_name);
                strcat(new_indir,"/");
                strcat(new_indir,ent1->d_name);
                mkdir(new_indir,0755);
                DIR* find=opendir(check);
                struct dirent* in_ent;
                char biigest[30];
                in_ent=readdir(find);
                while(in_ent!=NULL){
                    if(strcmp(in_ent->d_name,".")!=0 && strcmp(in_ent->d_name,"..")!=0){
                        strcpy(biigest,in_ent->d_name); 
                    }
                    in_ent=readdir(find);
                }
                strcat(check,"/");
                strcat(check,biigest);
                FILE* to_copy=fopen(check,"rb");
                strcat(new_indir,"/");
                strcat(new_indir,biigest);
                FILE* to_paste=fopen(new_indir,"wb");
                char buff[BUFSIZ];
                size_t n;
                while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                    fwrite(buff,1,n,to_paste);
                }
                fclose(to_paste);
                fclose(to_copy);
            }
            else{
                char version1[1024];
                char version2[1024];
                strcpy(version1,dir1);
                strcpy(version2,dir2);
                strcat(version1,"/");
                strcat(version2,"/");
                strcat(version1,ent1->d_name);
                strcat(version2,ent1->d_name);
                DIR* find1=opendir(version1);
                struct dirent* in_ent1;
                char biigest[30];
                in_ent1=readdir(find1);
                while(in_ent1!=NULL){
                    if(strcmp(in_ent1->d_name,".")!=0 && strcmp(in_ent1->d_name,"..")!=0){
                        strcpy(biigest,in_ent1->d_name); 
                    }
                    in_ent1=readdir(find1);
                }
                strcat(version1,"/");
                strcat(version1,biigest);
                DIR* find2=opendir(version2);
                struct dirent* in_ent2;
                in_ent2=readdir(find2);
                while(in_ent2!=NULL){
                    if(strcmp(in_ent2->d_name,".")!=0 && strcmp(in_ent2->d_name,"..")!=0){
                        strcpy(biigest,in_ent2->d_name); 
                    }
                    in_ent2=readdir(find2);
                }
                strcat(version2,"/");
                strcat(version2,biigest);
                char line1[100];
                char line2[100];
                char line1cpy[100];
                char line2cpy[100];
                //printf("%s\n%s\n",version1,version2);
                if(modified(version1,version2)){
                    char make[1024];
                    strcpy(make,new_b_name);
                    strcat(make,"/");
                    strcat(make,ent1->d_name);
                    mkdir(make,0755);
                    strcat(make,"/");
                    strcat(make,biigest);
                    FILE* makee=fopen(make,"wb");
                    FILE* to_copy=fopen(version1,"rb");
                    char buff[BUFSIZ];
                    size_t n;
                    while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                        fwrite(buff,1,n,makee);
                    }
                    fclose(makee);
                    fclose(to_copy);
                    continue;
                }
                FILE* File1=fopen(version1,"r");
                FILE* File2=fopen(version2,"r");
                int count1=0;
                int count2=0;
                char to_show[1024];
                strcpy(to_show,ent1->d_name);
                for(int i=0;i<strlen(to_show);i++) if(to_show[i]=='^') to_show[i]='/';
                char BUff[1024];
                printf(BWHT"««««««««««"reset BMAG"%s"reset BWHT"»»»»»»»»»»\n"reset,realpath(to_show,BUff));
                while((fgets(line1,MAX_LINE_LENGTH,File1))!=NULL||(fgets(line2,MAX_LINE_LENGTH,File2))!=NULL){
                    count1++;
                    count2++;
                    strcpy(line1cpy,line1);
                    strcpy(line2cpy,line2);
                    while(is_empty(line1)){
                        fgets(line1,MAX_LINE_LENGTH,File1);
                        strcpy(line1cpy,line1);
                        count1++;
                    }
                    while(is_empty(line2)){
                        fgets(line2,MAX_LINE_LENGTH,File2);
                        strcpy(line2cpy,line2);
                        count2++;
                    }
                    for (int t=0;t<1000;t++)for (int i=0;i<strlen(line1cpy);i++) if(line1cpy[i]==' ' && (line1cpy[i+1]==' ' || line1cpy[i+1]=='\0')) for(int j=i;j<strlen(line1cpy)+1;j++) line1cpy[j]=line1cpy[j+1];
                    for (int t=0;t<1000;t++)for (int i=0;i<strlen(line2cpy);i++) if(line2cpy[i]==' ' && (line2cpy[i+1]==' ' || line2cpy[i+1]=='\0')) for(int j=i;j<strlen(line2cpy)+1;j++) line2cpy[j]=line2cpy[j+1];
                    if(strcmp(line1cpy,line2cpy)!=0){
                        char to_show2[1024];
                        strcpy(to_show2,ent1->d_name);
                        for(int i=0;i<strlen(to_show2);i++) if(to_show2[i]=='^') to_show2[i]='/';
                        char to_show1[1024];
                        strcpy(to_show1,ent1->d_name);
                        for(int i=0;i<strlen(to_show1);i++) if(to_show1[i]=='^') to_show1[i]='/';
                        printf(BWHT"<%s-%s>-<%d>\n"reset,to_show1,argvtmp[3],count1);
                        printf(BBLU"%s\n"reset,line1);
                        printf(BWHT"<%s-%s>-<%d>\n"reset,to_show2,argvtmp[4],count2);
                        printf(BCYN"%s\n"reset,line2);
                    }
                            
                
            }
            fclose(File1);
            fclose(File2);
            }
        }

        }
        DIR* dire2=opendir(dir2);
        struct dirent* ent2;
        while((ent2=readdir(dire1))!=NULL){
            if(strcmp(ent2->d_name,".")!=0 && strcmp(ent2->d_name,"..")!=0){
            char check[1024];
            strcpy(check,dir1);
            strcat(check,"/");
            strcat(check,ent1->d_name);
            if(isDirectory(check)!=1){
                char new_indir[1024];
                strcpy(new_indir,new_b_name);
                strcat(new_indir,"/");
                strcat(new_indir,ent1->d_name);
                mkdir(new_indir,0755);
                DIR* find=opendir(check);
                struct dirent* in_ent;
                char biigest[30];
                in_ent=readdir(find);
                while(in_ent!=NULL){
                    if(strcmp(in_ent->d_name,".")!=0 && strcmp(in_ent->d_name,"..")!=0){
                        strcpy(biigest,in_ent->d_name); 
                    }
                    in_ent=readdir(in_ent);
                }
                strcat(check,biigest);
                FILE* to_copy=fopen(check,"rb");
                strcat(new_indir,biigest);
                FILE* to_paste=fopen(new_indir,"wb");
                char buff[BUFSIZ];
                size_t n;
                while((n=fread(buff,1,BUFSIZ,to_copy))!=0){
                    fwrite(buff,1,n,to_paste);
                }
                fclose(to_paste);
                fclose(to_copy);
            }
        }
        
        }

    }
}