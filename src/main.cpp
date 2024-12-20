#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <string.h>

using namespace std;
int main(int argc, char* argv[])
{
    int ret = 0;
    const char* device = "/dev/video0";
    /* 打开设备 */
    int fd = open(device,O_RDWR);
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
        ret = ioctl(fd,VIDIOC_ENUM_FMT,&v4fmt);
        if(ret < 0){
            cout << "read format Finished!" << endl;
            break;
        }

        cout << "index = " << v4fmt.index << endl;
        cout << "flags = " << v4fmt.flags << endl;
        cout << "desc = " << v4fmt.description << endl;
        unsigned char *p = (unsigned char *)&v4fmt.pixelformat;
        cout << "pixelformat = " << p[0] << p[1] << p[2] << p[3] << endl;
        cout << "reserved = " << v4fmt.reserved[0] << endl;
    }

    /* 3. 设置采集格式 */
    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //摄像头采集
    vfmt.fmt.pix.width = 1280; //设置采集宽度
    vfmt.fmt.pix.height = 720; //设置采集高度
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG; //设置采集格式
    vfmt.fmt.pix.field = V4L2_FIELD_ANY; //设置采集场
    ret = ioctl(fd,VIDIOC_S_FMT,&vfmt);
    if(ret < 0){
        cout << "设置采集格式失败" << endl;
        return -1;
    }
    cout << "设置采集格式成功" << endl;
    
    /* 4. 申请内核空间 */
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.count = 4; //申请4个缓冲区
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //设置缓冲区类型
    reqbuffer.memory = V4L2_MEMORY_MMAP; //设置缓冲区内存类型
    ret = ioctl(fd,VIDIOC_REQBUFS,&reqbuffer);
    if(ret < 0){
        cout << "申请缓冲区失败" << endl;
        return -1;
    }
    cout << "申请缓冲区成功" << endl;
    
    /* 5.把内核的缓冲区队列映射到用户空间 */
    unsigned char *buffer[reqbuffer.count];
    unsigned int size[reqbuffer.count];
    struct v4l2_buffer mapbuffer;
    //初始化type index
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    for(int i = 0; i < reqbuffer.count; i++){
        mapbuffer.index = i;
        ret = ioctl(fd,VIDIOC_QUERYBUF,&mapbuffer); //从内核空间中查询一个空间做映射
        if(ret < 0){
            cout << "映射缓冲区失败" << endl;
            return -1;
        }
        buffer[i] = (unsigned char *)mmap(NULL,mapbuffer.length,PROT_READ|PROT_WRITE,MAP_SHARED,fd,mapbuffer.m.offset);
        if(buffer[i] == MAP_FAILED){
            cout << "映射缓冲区失败" << endl;
            return -1;
        }
        size[i] = mapbuffer.length;

        /* 使用完毕 放回去*/
        ret = ioctl(fd,VIDIOC_QBUF,&mapbuffer);
        if(ret < 0){
            cout << "放回缓冲区失败" << endl;
            return -1;
        }
    }

    /* 6. 开始采集 */
    ret = ioctl(fd,VIDIOC_STREAMON,&vfmt.type);
    if(ret < 0){
        cout << "开始采集失败" << endl;
        return -1;
    }
    
    cout << "开始采集成功" << endl;
    /* 最后一步:关闭设备 */
    close(fd);
    return 0;
}
