#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

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

    /* 2. 获取摄像头支持的格式ioctl(文件描述符，命令，结构体指针)*/
    struct v4l2_fmtdesc v4fmt;
    v4fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    int i = 0;
    while(1){
        v4fmt.index = i++;
        int ret = ioctl(fd,VIDIOC_ENUM_FMT,&v4fmt);
        if(ret < 0){
            break;
        }

        cout << "index = " << v4fmt.index;
        cout << "flags = " << v4fmt.flags;
        cout << "desc = " << v4fmt.description;
        unsigned char *p = (unsigned char *)&v4fmt.pixelformat;
        cout << "pixelformat = " << p[0] << p[1] << p[2] << p[3];
        cout << "reserved = " << v4fmt.reserved[0];
    }

    /* 3. 设置采集格式 */
    /* 最后一步:关闭设备 */
    close(fd);
    return 0;
}
