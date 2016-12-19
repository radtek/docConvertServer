using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Runtime.InteropServices;

namespace Downfile
{
    class Program
    {
        [DllImport("user32.dll", EntryPoint = "PostThreadMessage")]
        private static extern int PostThreadMessage(

                          int threadId, //线程标识

                          uint msg, //消息标识符

                          int wParam, //具体由消息决定

                          int lParam); //具体由消息决定

        public const int USER = 0x0400;
        public const int WM_MSG_DOWNFILE_STATUS = USER + 0x40;

        static void Main(string[] args)
        {
            if(args.Length != 3)
            {
                return;
            }
            int threadid = int.Parse(args[0]);
            string url = args[1];
            string path = args[2];

            if(File.Exists(path))
            {
                PostThreadMessage(threadid, WM_MSG_DOWNFILE_STATUS, 0, 0);
                return;
            }

            if(HttpDownloadFile(url, path) == 0)
            {
                PostThreadMessage(threadid,WM_MSG_DOWNFILE_STATUS, 0, 0);
            }
            else
            {
                PostThreadMessage(threadid, WM_MSG_DOWNFILE_STATUS, 1, 0);
            }
        }


        public static int HttpDownloadFile(string url, string path)
        {
            try
            {
                // 设置参数
                HttpWebRequest request = WebRequest.Create(url) as HttpWebRequest;
                //发送请求并获取相应回应数据
                HttpWebResponse response = request.GetResponse() as HttpWebResponse;
                //直到request.GetResponse()程序才开始向目标网页发送Post请求
                Stream responseStream = response.GetResponseStream();
                //创建本地文件写入流
                Stream stream = new FileStream(path, FileMode.Create);
                byte[] bArr = new byte[1024];
                int size = responseStream.Read(bArr, 0, (int)bArr.Length);
                while (size > 0)
                {
                    stream.Write(bArr, 0, size);
                    size = responseStream.Read(bArr, 0, (int)bArr.Length);
                }
                stream.Close();
                responseStream.Close();
                return 0;
            }
            catch(Exception e)
            {
                return 1;
            }
        }
    }
}
