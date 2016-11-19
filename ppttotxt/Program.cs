///PPT转txt
///create by hanson 20161031
///用索引方式获取ppt文件中文本信息
///
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using EPocalipse.IFilter;
using Aspose.Slides;

namespace ppttotxt
{
    enum OutStatus
    {
        ConvertSuccess,//0:转换成功
        FileLoss,//1:下载失败/文件不存在
        NoText,//2:扫描版或者纯图片
        TotxtFailed,//3:totxt转换失败
        PageLess//4:页数低于指定值
    };

    class Program
    {
        [DllImport("user32.dll", EntryPoint = "PostThreadMessage")]
        private static extern int PostThreadMessage(

                          int threadId, //线程标识

                          uint msg, //消息标识符

                          int wParam, //具体由消息决定

                          int lParam); //具体由消息决定

        public const int USER = 0x0400;

        public const int WM_MSG_PPT2TXT_STATUS = USER + 0x15;

        static string sourcefile;
        static string outpath;
        static int threadid;
        static int fileid;
        static int minpage;
        static int maxtxtsize;
        static void Main(string[] args)
        {
            int len = args.Length;
            if (len != 6) return;
            foreach (string arg in args)
            {
                Console.WriteLine(arg);
            }

            sourcefile = args[0];
            outpath = args[1];
            threadid = int.Parse(args[2]);
            fileid = int.Parse(args[3]);
            minpage = int.Parse(args[4]);
            maxtxtsize = int.Parse(args[5]);

            Console.WriteLine("begin get page time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));
            //判断文件页数
            Presentation ppt = new Presentation(sourcefile);
            Console.WriteLine("end open time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));
            int pages = ppt.Slides.Count;
            ppt.Dispose();
            if (pages < minpage)
            {
                PostThreadMessage(threadid, WM_MSG_PPT2TXT_STATUS, (int)OutStatus.PageLess, pages);
                return;
            }
            Console.WriteLine("end get page time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));

            int status = ConvertFile();
            PostThreadMessage(threadid, WM_MSG_PPT2TXT_STATUS, status, 0);
            Console.WriteLine("end totxt time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));
//             Console.ReadKey();
        }
        public static int ConvertFile()
        {
            if (!File.Exists(sourcefile))
            {
                return (int)OutStatus.FileLoss;
            }
            try
            {
                TextReader reader = new FilterReader(sourcefile);
                using (reader)
                {
                    //                 char[] buffer = new char[0x5000];
                    //                 reader.Read(buffer, 0, 0x5000);
                    //                 string context = new string(buffer);
                    string context = reader.ReadToEnd();
                    context = Regex.Replace(context, "\n\r", " ", RegexOptions.IgnoreCase);

                    try
                    {
                        using (StreamWriter writer = new StreamWriter((outpath + @"\" + fileid + ".txt").Replace(@"\\", @"\"), false, Encoding.Default))
                        {
                            writer.Write(context);
                            writer.Close();
                        }
                        reader.Close();
                        return (int)OutStatus.ConvertSuccess;
                    }
                    catch (Exception exception)
                    {
                        Console.WriteLine("保存txt文件发生异常" + exception);
                        return (int)OutStatus.TotxtFailed;
                    }

                }
            }
            catch (Exception e)
            {
                Console.WriteLine("打开文件失败" + e);
                return (int)OutStatus.TotxtFailed;
            }
        }
    }
}
