using System;
using System.Collections.Generic;
// using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
// using System.Threading.Tasks;
using EPocalipse.IFilter;
using System.IO;
using System.Runtime.InteropServices;
using Aspose.Words;
using Aspose.Words.Saving;
using System.Diagnostics;


namespace wordtotxtpng
{
    class Program
    {
        enum OutStatus
        {
            ConvertSuccess,//0:转换成功
            FileLoss,//1:下载失败/文件不存在
            NoText,//2:扫描版或者纯图片
            TotxtFailed,//3:totxt转换失败
            PageLess//4:页数低于指定值
        };

        [DllImport("user32.dll", EntryPoint = "PostThreadMessage")]
        private static extern int PostThreadMessage(

                          int threadId, //线程标识

                          uint msg, //消息标识符

                          int wParam, //具体由消息决定

                          int lParam); //具体由消息决定

        public const int USER = 0x0400;

        public const int WM_MSG_WORD2TXT_STATUS = USER + 0x11;
        public const int WM_MSG_WORD2PNG_STATUS = USER + 0x12;

        static string sourcefile;
        static string outtxtpath;
        static string outimgpath;
        static int threadid;
        static int fileid;
        static int minpage;
        static int maxtxtsize;
        static int istoimg;
        static int isoriginal;

        static void Main(string[] args)
        {
            int len = args.Length;
            if (len != 9) return;
            foreach (string arg in args)
            {
                Console.WriteLine(arg);
            }

            sourcefile = args[0];
            outtxtpath = args[1];
            outimgpath = args[2];
            threadid = int.Parse(args[3]);
            fileid = int.Parse(args[4]);
            minpage = int.Parse(args[5]);
            maxtxtsize = int.Parse(args[6]);
            istoimg = int.Parse(args[7]);
            isoriginal = int.Parse(args[8]);

            Document doc = null;
            int pages = 0;
            //判断文件页数
            if (istoimg == 1)
            {
                try
                {
                    doc = new Document(sourcefile);
                    pages = doc.PageCount;
                    if (isoriginal == 0 && pages < minpage)
                    {
                        PostThreadMessage(threadid, WM_MSG_WORD2TXT_STATUS, (int)OutStatus.PageLess, pages);
                        return;
                    }
                }
                catch (Exception e)
                {
//                     PostThreadMessage(threadid, WM_MSG_WORD2TXT_STATUS, (int)OutStatus.TotxtFailed, pages);
//                     return;
                    istoimg = 0;
                }
            }

            int txtstatus = ConvertFile();
            PostThreadMessage(threadid, WM_MSG_WORD2TXT_STATUS, txtstatus, pages);

            int pngstatus = 3;
            if (txtstatus == 0 && istoimg == 1)
            {
                //to png
                try
                {
                    ImageSaveOptions iso = new ImageSaveOptions(SaveFormat.Png);
                    iso.Resolution = 96;
                    iso.PrettyFormat = true;
                    iso.PageIndex = 0;
                    string outimgfile = (outimgpath + @"\" + fileid + ".png").Replace(@"\\", @"\");
                    doc.Save(outimgfile, iso);
                    pngstatus = 0;
                    
                    //
                    ExecuteScaleImg(outimgfile, (outimgpath + @"\" + fileid + "_small" + ".png").Replace(@"\\", @"\"), 1);
                }
                catch(Exception e)
                {
                    pngstatus = 3;
                }
                PostThreadMessage(threadid, WM_MSG_WORD2PNG_STATUS, pngstatus, 0);
            }

        }

        #region private func
        private static int ConvertFile()
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
                        string txtpath = (outtxtpath + @"\" + fileid + ".txt").Replace(@"\\", @"\");
                        using (StreamWriter writer = new StreamWriter(txtpath, false, Encoding.Default))
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

        //缩小图片，type=0 jpeg格式  1 png格式 
        private static void ExecuteScaleImg(string sourcefile, string outfile, int type)
        {
            if (sourcefile != null && !sourcefile.Equals("") && outfile != null && !outfile.Equals(""))
            {
                string command = sourcefile + " " + outfile + " " + type;
                Process process = new Process();//创建进程对象  
                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = "ScaleImg.exe";//设定需要执行文件  
                startInfo.Arguments = command;//执行参数  
                startInfo.UseShellExecute = false;//不使用系统外壳程序启动  
                startInfo.RedirectStandardInput = false;//不重定向输入  
                startInfo.RedirectStandardOutput = true; //重定向输出  
                startInfo.CreateNoWindow = true;//不创建窗口  
                process.StartInfo = startInfo;
                try
                {
                    if (process.Start())//开始进程  
                    {
                        //                         process.WaitForExit(10); //等待进程结束，等待时间为指定的毫秒  
                    }
                }
                catch
                {
                }
                finally
                {
                }
            }
        }
        #endregion

    }
}
