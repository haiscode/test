#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
    #include <unistd.h>

#include <jpeglib.h>

int main(int argc, char const *argv[])
{
    //定义解压缩体和处理错误结构体并初始化
    struct jpeg_decompress_struct mydecom;
    jpeg_create_decompress(&mydecom);

    struct jpeg_error_mgr myerror;
    mydecom.err = jpeg_std_error(&myerror);

    //打开需要显示的jpg图片
    FILE * myjpg = fopen("1.jpg","r+");
        if(myjpg == NULL)
        {
            perror("myjpg");
            return -1;
        }
    //打开lcd
    int lcdfd = open("/dev/fb0",O_RDWR);
        if (lcdfd == -1)
        {
            perror("open");
            return -1;
        }
    //映射lcd
    int * p = mmap(NULL, 800*480*4, PROT_READ |  PROT_WRITE,  MAP_SHARED,
                  lcdfd, 0);
                  if (p == MAP_FAILED)
                  {
                      perror("mmap");
                      return -1 ;
                  }
                  


        


    
    //指定解压数据源
    jpeg_stdio_src(&mydecom,myjpg);

    //读取jpg的头信息
    jpeg_read_header(&mydecom,TRUE);

    //开始解压缩
    jpeg_start_decompress(&mydecom);


    //打印图片的宽高
    // printf("W:%d\n",mydecom.image_width);
    // printf("H:%d\n",mydecom.image_height);


    //定义数组来存放解压缩到的数据
    char *readbuf = malloc(mydecom.image_width*3);

    //定义数组来存放ARGB的数据
    int rgbbug[mydecom.image_width];


    for (int i = 0; i < mydecom.image_height; i++)
    {

        //读取一行的解压缩数据
        jpeg_read_scanlines(&mydecom,(JSAMPARRAY)(&readbuf),1);

        for (int j = 0; j < mydecom.image_width; j++)
        {
            
            rgbbug[j] = 0x00<<24 |readbuf[3*j]<<16 |readbuf[3*j+1]<<8 |readbuf[3*j+2];
            
        }

        memcpy(p+i*800,rgbbug,mydecom.image_width*4);

    }
    //收尾工作
    jpeg_finish_decompress(&mydecom);
    jpeg_destroy_decompress(&mydecom);
    fclose(myjpg);
    close(lcdfd);
    munmap(p, 800*480*3);

    


    






    return 0;
}
