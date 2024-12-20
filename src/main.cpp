#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace std;
int main(int argc, char* argv[])
{
    string device = "/dev/video11";
    /* 打开设备 */
    int fd = open("/dev/video11",O_RDWR);
    if(fd < 0){
        cout << "打开设备失败" << endl;
        return -1;
    }
    cout << "打开设备成功" << endl;

    /* 最后一步:关闭设备 */
    close(fd);
    return 0;
}
