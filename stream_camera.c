#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>              /* low-level i/o */
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <sys/mman.h>

#include <bits/types/struct_timespec.h>
#include <bits/types/struct_timeval.h>

#include <linux/videodev2.h>

/*socket*/
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define	IP	"192.168.170.197"
#define	PORT	(8000)
#define	MAX_UDP_PACKET_SIZE	(8192)
#define MAX_ORDER_ID (21)
#define	DEBUG	(1)

int main(int argc, char* argv[])
{
	// socket init
	int sockfd;
	char socket_receive_buffer[MAX_UDP_PACKET_SIZE];
	struct sockaddr_in servaddr;

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&servaddr, 0, sizeof(servaddr));
	
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr(IP);//INADDR_ANY
	// camera init
	int fd = open("/dev/video0", O_RDWR);
	if (DEBUG)
        {
		//输出所有支持的格式
		struct v4l2_fmtdesc fmtdesc;
		fmtdesc.index = 0;
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		printf("Support format:\n");
		while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
		{
			printf("\t%d.%s\n", fmtdesc.index + 1, fmtdesc.description);
			fmtdesc.index++;
		}
		printf("enum done\n");
	}
	if (DEBUG)
	{
		//查看当前的输出格式
		struct v4l2_format fmt;
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		ioctl(fd, VIDIOC_G_FMT, &fmt);

		printf("Current data format information : \n\twidth: % d\n\theight: % d\n", fmt.fmt.pix.width, fmt.fmt.pix.height);

		struct v4l2_fmtdesc fmtdesc2;
		fmtdesc2.index = 0;
		fmtdesc2.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc2) != -1)
		{
			if (fmtdesc2.pixelformat & fmt.fmt.pix.pixelformat)
			{
				printf("\tformat: %d --- %s\n", fmt.fmt.pix.pixelformat, fmtdesc2.description);
				break;
			}
			fmtdesc2.index++;
		}
	}
	{
		//设置视频格式
		struct v4l2_format fmt;
		memset(&fmt, 0, sizeof(fmt));
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = 1280;
		fmt.fmt.pix.height = 720;
		fmt.fmt.pix.pixelformat = 1196444237;//jpeg,0:default
		fmt.fmt.pix.field = V4L2_FIELD_ANY;
		//设置设备捕获视频的格式
		if (ioctl(fd, VIDIOC_S_FMT, &fmt) < 0)
		{
			printf("set format failed\n");
			close(fd);
			return 0;
		}
		//如果摄像头不支持我们设置的分辨率格式，则 fmt.fmt.pix.width 会被修改，所以此处建议再次检查 fmt.fmt.pix. 的各种信息
 
		//向驱动申请帧缓存
		int CAP_BUF_NUM = 4;//4
		struct v4l2_requestbuffers req;
		memset(&req, 0, sizeof(req));
		req.count = CAP_BUF_NUM;  //申请一个拥有四个缓冲帧的缓冲区
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = V4L2_MEMORY_MMAP;
		if (ioctl(fd, VIDIOC_REQBUFS, &req) < 0)
		{
			if (EINVAL == errno)
			{
				printf(" does not support memory mapping\n");
				close(fd);
				return 0;
			}
			else
			{
				printf("does not support memory mapping, unknow error\n");
				close(fd);
				return 0;
			}
		}
		else
		{
			printf("alloc success\n");
		}
		if (req.count < CAP_BUF_NUM)
		{
			printf("Insufficient buffer memory\n");
			close(fd);
			return 0;
		}
		else
		{
			printf("get %d bufs\n", req.count);
		}

		//将帧缓存与本地内存关联
		typedef struct VideoBuffer {   //定义一个结构体来映射每个缓冲帧
			void* start;
			size_t length;
		} VideoBuffer;
		VideoBuffer* buffers = calloc(req.count, sizeof(*buffers));
		struct v4l2_buffer buf;
		for (int numBufs = 0; numBufs < req.count; numBufs++) {//映射所有的缓存
			memset(&buf, 0, sizeof(buf));
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = numBufs;
			if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1) {//获取到对应index的缓存信息，此处主要利用length信息及offset信息来完成后面的mmap操作。
				printf("unexpect error %d\n", numBufs);
				free(buffers);
				close(fd);
				return 0;
			}

			buffers[numBufs].length = buf.length;
			// 转换成相对地址
			buffers[numBufs].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset); // #include <sys/mman.h>
			if (buffers[numBufs].start == MAP_FAILED) {
				printf("%d map failed errno %d\n", numBufs, errno);
				free(buffers);
				close(fd);
				return 0;
			}
			//addr 映射起始地址，一般为NULL ，让内核自动选择
			//prot 标志映射后能否被读写，其值为PROT_EXEC,PROT_READ,PROT_WRITE, PROT_NONE
			//flags 确定此内存映射能否被其他进程共享，MAP_SHARED,MAP_PRIVATE
			//fd,offset, 确定被映射的内存地址 返回成功映射后的地址，不成功返回MAP_FAILED ((void*)-1)
			//int munmap(void* addr, size_t length);// 最后记得断开映射

			//把缓冲帧加入缓冲队列
			if (ioctl(fd, VIDIOC_QBUF, &buf) < 0)
			{
				printf("add buf to queue failed %d\n", numBufs);
				free(buffers);
				close(fd);
				return 0;
			}
		}

		int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		/* 打开设备视频流 */
		if (ioctl(fd, VIDIOC_STREAMON, &type) < 0)
		{
			printf("stream open failed\n");
			free(buffers);
			close(fd);
			return 0;
		}

		char order_id = 0;
		int franeCount = 1;
		while (1)//franeCount--)
		{
			struct v4l2_buffer capture_buf;
			memset(&capture_buf, 0, sizeof(capture_buf));
			capture_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			capture_buf.memory = V4L2_MEMORY_MMAP;
			/* 将已经捕获好视频的内存拉出已捕获视频的队列 */
			if (ioctl(fd, VIDIOC_DQBUF, &capture_buf) < 0)
			{
				printf("get frame failed %d\n", franeCount);
				break;
			}
			else
			{
				{
					FILE* f = fopen("./test.jpeg", "wb");
					printf("used %d，buffer length %d.\n", capture_buf.bytesused, buffers[capture_buf.index].length);
					int sum_length = capture_buf.bytesused;
					int calc_length = 0;
					char send_buffer[MAX_UDP_PACKET_SIZE];
					int i = 0;
					while(calc_length < sum_length) {
						char more_slice_flag = 1;
						int a_packet_length = (MAX_UDP_PACKET_SIZE - 2);
						if ((sum_length - calc_length) < (MAX_UDP_PACKET_SIZE -2)) {
							a_packet_length = sum_length - calc_length;
							more_slice_flag = 0;
						}
						memset(send_buffer, 0, sizeof(send_buffer));
						send_buffer[0] = order_id;
						send_buffer[1] = more_slice_flag;
						int wt = fwrite(buffers[capture_buf.index].start+calc_length, 1, a_packet_length, f);
						memcpy(send_buffer+2, buffers[capture_buf.index].start+calc_length, a_packet_length);
						sendto(sockfd, send_buffer, a_packet_length + 2, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
						//sendto(sockfd, buffers[capture_buf.index].start+calc_length, a_packet_length, MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
						printf("i: %d, order_id: %d, a_packet_length: %d, %d bytes was be sent.\n", i, order_id, a_packet_length, calc_length + a_packet_length);
						calc_length += a_packet_length;
						order_id = (++order_id) % MAX_ORDER_ID;
						i++;
						//usleep(10);
					}
					fclose(f);
					//sleep(1);
				}
				printf("get %d frame success\n", franeCount);

				//把用完的帧重新插回队列
				if (ioctl(fd, VIDIOC_QBUF, &capture_buf) == -1) {
					printf("insert buf failed %d\n", franeCount);
					break;
				}
			}
		}

		//清理资源
		int ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
		for (int i = 0; i < CAP_BUF_NUM; i++)
		{
			munmap(buffers[i].start, buffers[i].length);
		}
		free(buffers);
		close(fd);
	}
    return 0;
}
