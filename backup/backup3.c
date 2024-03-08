#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

void Gzip(char way[512]) {
  char command[512] = {"gzip "};
  strcat(command, way);
  system(command);
}

void ChangeTime(char way[512], long long time) {
  char command[512] = {"touch -m -a -t "};
  char date[16] = {};
  date[15] = '\0';
  date[14] = (char)((int)'0' + time % 10);
  time /= 10;
  date[13] = (char)((int)'0' + time % 10);
  time /= 10;
  date[12] = '.';
  for(int i = 11; i >= 0; i--) {
  	date[i] = (char)((int)'0' + time % 10);
  	time /= 10;
  }
  strcat(command, date);
  strcat(command, " ");
  strcat(command, way);
  system(command);
}

void FileWay(char way[512], char argv[512], char name_file[100]) {
  strcpy(way, argv);
  strcat(way, "/");
  strcat(way, name_file);
}

void RecRemove(char way[512]){
  char way_to_dir[512] = {};
  strcpy(way_to_dir, way);
  DIR *rm_dir = opendir(way_to_dir);
  struct dirent *list_rm_dir;
  struct stat info_rm_file;
  while((list_rm_dir = readdir(rm_dir))) {
  	if ( strcmp( ".", list_rm_dir->d_name ) == 0 || strcmp( "..", list_rm_dir->d_name ) == 0 ) {
  	    continue;
    } 
	char way_rm_file[512] = {};
	FileWay(way_rm_file, way_to_dir, list_rm_dir->d_name);
	stat(way_rm_file, &info_rm_file);
	if(S_ISDIR(info_rm_file.st_mode)) {
	  printf("Delete dir: %s\n", list_rm_dir->d_name);
	  RecRemove(way_rm_file);
	} else {
	  printf("Delete file: %s\n", list_rm_dir->d_name);
	}
	remove(way_rm_file);
  }
  remove(way);
}

void Compare(char source[512], char receiver[512]){
  char original_dir[512] = {};
  strcpy(original_dir, source);
  char copy_dir[512] = {}; 	  	
  strcpy(copy_dir, receiver);
  DIR *original = opendir(original_dir);
  DIR *copy = opendir(copy_dir);
  struct dirent *list_original_dir;
  struct dirent *list_copy_dir;
  struct stat info_original_file;
  struct stat info_copy_file;
  while((list_copy_dir = readdir(copy))) {
  	if ( strcmp( ".", list_copy_dir->d_name ) == 0 || strcmp( "..", list_copy_dir->d_name ) == 0 ) {
  	  continue;
    }
	char way_file_copy[512] = {};
  	FileWay(way_file_copy, copy_dir, list_copy_dir->d_name);
  	stat(way_file_copy, &info_copy_file); 
    int flag = 1;
	rewinddir(original);
    while((list_original_dir = readdir(original))) {
      if ( strcmp( ".", list_original_dir->d_name ) == 0 || strcmp( "..", list_original_dir->d_name ) == 0 ) {
  	    continue;
      } 
	  char way_file_original[512] = {};
	  char name_copy[100] = {};
	  FileWay(way_file_original, original_dir, list_original_dir->d_name);
	  strcpy(name_copy, list_original_dir->d_name);
	  stat(way_file_original, &info_original_file);
	  if(!(S_ISDIR(info_original_file.st_mode))) {
		strcat(name_copy, ".gz");
	  }
      flag = strcmp(name_copy, list_copy_dir->d_name);
      if(flag == 0){
      	if(S_ISDIR(info_original_file.st_mode)) {
  		  Compare(way_file_original, way_file_copy);
		}
        break;
      }
    }
    if(flag != 0) {
      if(S_ISDIR(info_copy_file.st_mode)) {
      	printf("Delete dir: %s\n", list_copy_dir->d_name);
  	    RecRemove(way_file_copy);
	  } else {
	  	printf("Delete file: %s\n", list_copy_dir->d_name);
        remove(way_file_copy);
      }
    }
  }
}

void CopyFile(char source[512], char receiver[512]) {
  FILE *in = fopen(source, "rb");
  FILE *out = fopen(receiver, "wb");
  char data[1024] = {};
  unsigned int realsize = 0;
  while (!feof(in)) {
    realsize = fread(data, sizeof(char), 1024, in);
    fwrite(data, sizeof(char), realsize, out);
  }
  fclose(in);
  fclose(out);	
}
  
int Execution(char source[512], char receiver[512]) {
  char original_dir[512] = {};
  strcpy(original_dir, source);
  char copy_dir[512] = {}; 	  	
  strcpy(copy_dir, receiver);
  DIR *original = opendir(original_dir);
  DIR *copy = opendir(copy_dir);
  struct dirent *list_original_dir;
  struct dirent *list_copy_dir;
  struct stat info_original_file;
  struct stat info_copy_file;
  struct tm *date;
  if(!original) {
  	return 2;
  }
  if(!copy) {
  	return 3;
  }
  while((list_original_dir = readdir(original))) {
  	if ( strcmp( ".", list_original_dir->d_name ) == 0 || strcmp( "..", list_original_dir->d_name ) == 0 ) {
  	  continue;
    } 
    int flag = 1;
    char way_file_original[512] = {};
    char name_copy[100] = {};
    FileWay(way_file_original, original_dir, list_original_dir->d_name);
    strcpy(name_copy, list_original_dir->d_name);
	stat(way_file_original, &info_original_file);
	date = localtime(&info_original_file.st_atime);
	long long time = date->tm_sec + date->tm_min * 100 + date->tm_hour * 10000 + date->tm_mday * 1000000 + (date->tm_mon+1) * 100000000 + (date->tm_year+1900) * 10000000000;
	if(!(S_ISDIR(info_original_file.st_mode))) {
	  strcat(name_copy, ".gz");
	}
	rewinddir(copy);
    while((list_copy_dir = readdir(copy))) {
      if ( strcmp( ".", list_copy_dir->d_name ) == 0 || strcmp( "..", list_copy_dir->d_name ) == 0 ) {
  	    continue;
      } 
      flag = strcmp(name_copy, list_copy_dir->d_name);
      if(flag == 0){
      	char way_file_copy[512] = {};
  	  	FileWay(way_file_copy, copy_dir, list_copy_dir->d_name);
  		stat(way_file_copy, &info_copy_file);
  		if(S_ISDIR(info_original_file.st_mode)) {
  		  Execution(way_file_original, way_file_copy);
		} else {
		  if(((long long)info_original_file.st_atime) != ((long long)info_copy_file.st_atime)) {
      	  	remove(way_file_copy);
      	  	FileWay(way_file_copy, copy_dir, list_original_dir->d_name);
      	  	printf("Copying file: %s\n", list_original_dir->d_name);
		  	CopyFile(way_file_original, way_file_copy);
		  	Gzip(way_file_copy);
		  	strcat(way_file_copy, ".gz");
		    ChangeTime(way_file_copy, time);
          }	
		}
        break;
      }
    }
    if(flag != 0) {
      char way_file_copy[512] = {};
  	  FileWay(way_file_copy, copy_dir, list_original_dir->d_name);
      if(S_ISDIR(info_original_file.st_mode)) {
      	printf("Create dir: %s\n", list_original_dir->d_name);
      	mkdir(way_file_copy, 0400 | 0200 | 0100 | 0040 | 0020 | 0010 | 0004 | 0002 | 0001);
  		Execution(way_file_original, way_file_copy);
      } else {
      	printf("Copying file: %s\n", list_original_dir->d_name);
		CopyFile(way_file_original, way_file_copy);
		Gzip(way_file_copy);
		strcat(way_file_copy, ".gz");
		ChangeTime(way_file_copy, time);
	  }
    }
  }
  return 0;
}

int Directory_Presence(char way_receiver_dir[512], char name_receiver_dir[100]) {
  DIR *receiver_dir = opendir(way_receiver_dir);
  struct dirent *list_receiver_dir;
  struct stat info_receiver_file;
  int flag = 1;
  if(!receiver_dir) {
  	return 2;
  }
  while((list_receiver_dir = readdir(receiver_dir))) {
  	if ( strcmp( ".", list_receiver_dir->d_name ) == 0 || strcmp( "..", list_receiver_dir->d_name ) == 0 ) {
  	  continue;
    }
    flag = strcmp(name_receiver_dir, list_receiver_dir->d_name);
    if (flag == 0) {
      char way_maybe_dir[512] = {};
      FileWay(way_maybe_dir, way_receiver_dir, list_receiver_dir->d_name);
      stat(way_maybe_dir, &info_receiver_file);
      if(S_ISDIR(info_receiver_file.st_mode)) {
        return 0;	
	  } else {
	  	return 3;
	  }
	}
  }
  printf("Create dir: %s\n", name_receiver_dir);
  return 1;
}

int Start(char source[512], char receiver[512]) {
  int i = strlen(receiver);
  char name_receiver_dir[100] = {};
  char way_receiver_dir[512] = {};
  while(receiver[i] != '/') {
  	i--;
  }
  int j = 0;
  for(; j < i; j++) {
  	way_receiver_dir[j] = receiver[j];
  }
  i = 0;
  j++;
  way_receiver_dir[j] = '\0';
  while(receiver[j] != '\0') {
  	name_receiver_dir[i] = receiver[j];
    i++;
    j++;
  }
  name_receiver_dir[i] = '\0';
  switch(Directory_Presence(way_receiver_dir, name_receiver_dir)) {
  	case 0:
  	  Compare(source, receiver);
  	  return Execution(source, receiver);
  	  break;
  	case 1:
  	  mkdir(receiver, 0400 | 0200 | 0100 | 0040 | 0020 | 0010 | 0004 | 0002 | 0001);
  	  return Execution(source, receiver);
  	  break;
  	case 2:
  	  return 3;
  	  break;
	case 3:
  	  return 4;
  	  break; 
  }
  return 5;
}

void Upshot(int result) {
  switch(result) {
  	case 0:
  	  printf("Copy completed successfully\n");
  	  break;
  	case 1:
  	  printf("Parameters are incorrect\n Correct: [Parameter 1 (Source directory)] [Parameter 2 (Destination directory)]\n");
  	  break;
  	case 2:
  	  printf("Failed to open source directory");
  	  break;
  	case 3:
  	  printf("Receiver directory path error\n");
  	  break;
  	case 4:
  	  printf("The source directory path points to a file that is not a directory.\n");
  	  break;
  	case 5:
  	  printf("Undefined error.\n");
  	  break;
  }
}

int main(int argc, char *argv[]) {
  time_t start = time(NULL);
  if(argc != 3){
  	Upshot(1);
  	return 0;
  }
  int result = Start(argv[1], argv[2]);
  Upshot(result);
  time_t end = time(NULL);
  printf("Elapsed time: %lf sec.\n", difftime(end, start));
  return 0;
}
