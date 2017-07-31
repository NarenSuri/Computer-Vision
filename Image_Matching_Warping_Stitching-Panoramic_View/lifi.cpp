#include <iostream>
#include <dirent.h>

using namespace std;

int main() {

    DIR*     dir;
    dirent*  pdir;

    dir = opendir("./a2-images/part1_images/");     // open current directory

    while (pdir = readdir(dir)) {
	
        cout << pdir->d_name << endl;}
   
    closedir(dir);
    return 0;
}
