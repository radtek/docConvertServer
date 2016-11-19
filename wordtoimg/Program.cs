using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;
using Aspose.Words;
using Aspose.Words.Saving;

namespace wordtoimg
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

        public const int WM_MSG_WORD2IMG_STATUS = USER + 0x12;

        static string sourcefile;
        static string outpath;
        static int threadid;
        static int fileid;
        static void Main(string[] args)
        {
            int len = args.Length;
            if (len != 4) return;
            foreach (string arg in args)
            {
                Console.WriteLine(arg);
            }

            sourcefile = args[0];
            outpath = args[1];
            threadid = int.Parse(args[2]);
            fileid = int.Parse(args[3]);

            Console.WriteLine("begin toimg time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));
            int status = ConvertFile();
            PostThreadMessage(threadid, WM_MSG_WORD2IMG_STATUS, status, 0);
            Console.WriteLine("end toimg time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));
//             Console.ReadKey();
        }

        static private int ConvertFile()
        {
            try
            {
                Document doc = new Document(sourcefile);
                Console.WriteLine("end open time:  " + DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss"));

                ImageSaveOptions iso = new ImageSaveOptions(SaveFormat.Png);
                iso.Resolution = 96;
                iso.PrettyFormat = true;
                int pages = doc.PageCount;
                if (pages <= 0) return 1;
                pages = pages > 1 ? 1 : pages;
                for (int i = 0; i < pages; i++)
                {
                    iso.PageIndex = i;
                    doc.Save((outpath + @"\" + fileid + ".png").Replace(@"\\", @"\"), iso);
                }
                return 0;
            }
            catch (Exception e)
            {
                return 1;
            }
        }
    }
}
