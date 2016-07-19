#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<dirent.h>
#include<time.h>
#include<string.h>
#include<pwd.h>
#include <grp.h>
int main(int argc,char* argv[])
{
	if(argc!=2)
	{
		printf("error args!");
		return -1;
	}
	DIR* dir;
	dir=opendir(argv[1]);//打开目录
	if(NULL==dir)
	{
		perror("opendir");
		return -1;
	}
	struct dirent* p;//创建目录信息结构体
	struct stat buf;//创建stat结构体
	struct passwd *my_info;//创建用户信息结构体
	struct group *gid_info;//创建组信息结构体
	char s[128];
	char t[25]={0};
	char perms[11]={0};
	int ret;
	int i;
	while((p=readdir(dir))!=NULL)
	{
		if(strcmp(p->d_name,".")==0||strcmp(p->d_name,"..")==0)
		{}
		else{
			memset(s,0,sizeof(s));
			memset(&buf,0,sizeof(buf));
			sprintf(s,"%s/%s",argv[1],p->d_name);//拼接目录
			ret=stat(s,&buf);
			if(-1==ret)
			{
				perror("stat");
				return -1;
			}
			strncpy(t,ctime(&buf.st_mtime),16);
			my_info=getpwuid(buf.st_uid);
			gid_info=getgrgid(buf.st_gid);
			switch(buf.st_mode&S_IFMT)
			{
				case S_IFREG:perms[0]=('-');break;
				case S_IFDIR:perms[0]=('d');break;
				case S_IFCHR:perms[0]=('c');break;
				case S_IFBLK:perms[0]=('l');break;
				case S_IFIFO:perms[0]=('l');break;  
				case S_IFLNK:perms[0]=('l');break;  
				case S_IFSOCK:perms[0]=('l');break;  
				default:perms[0]=('?');break;
			}
			/*用户权限以及处理调整用户id位*/
			perms[1]=((buf.st_mode&S_IRUSR)? 'r':'-');
			perms[2]=((buf.st_mode&S_IWUSR)? 'w':'-');
			if((buf.st_mode&S_ISUID)!=0)
					perms[3]='s';
					else   
					perms[3]=((buf.st_mode&S_IXUSR)? 'x':'-');
			/*组权限以及处理调整组id位*/
			perms[4]=((buf.st_mode&S_IRGRP)? 'r':'-');
			perms[5]=((buf.st_mode&S_IWGRP)? 'w':'-');
			if((buf.st_mode&S_ISGID)!=0)
					perms[6]='s';
					else 
					perms[6]=((buf.st_mode&S_IXGRP)? 'x':'-');
			/*其他人权限以及处理sticky位*/
			perms[7]=((buf.st_mode&S_IROTH)? 'r':'-');
			perms[8]=((buf.st_mode&S_IWOTH)? 'w':'-');
			if((buf.st_mode&S_ISVTX)!=0)
					perms[9]='t';
			perms[9]=((buf.st_mode&S_IXOTH)?'x':'-');
			perms[10]='\0';
			printf("%s %d %s %s %5ld %s %s\n",perms,buf.st_nlink,my_info->pw_name,gid_info->gr_name,buf.st_size,t+4,p->d_name);
		}
	}
	return 0;
}
