using System;
using System.Collections.Generic;
// using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
// using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.IO;
using System.Drawing;
using EPocalipse.IFilter;
using Aspose.Cells;
using System.Reflection;
using System.Diagnostics;

namespace exceltotxtpng
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

        public const int WM_MSG_EXCEL2TXT_STATUS = USER + 0x13;
        public const int WM_MSG_EXCEL2IMG_STATUS = USER + 0x14;

        #region global variable
        static string sourcefile;
        static string outtxtpath;
        static string outimgpath;
        static int threadid;
        static int fileid;
        static int minpage;
        static int maxtxtsize;
        static int istoimg;
        static int isoriginal;
        #endregion

        static void Main(string[] args)
        {
            int len = args.Length;
            if (len != 9) return;
//             foreach (string arg in args)
//             {
//                 Console.WriteLine(arg);
//             }

            sourcefile = args[0];
            outtxtpath = args[1];
            outimgpath = args[2];
            threadid = int.Parse(args[3]);
            fileid = int.Parse(args[4]);
            minpage = int.Parse(args[5]);
            maxtxtsize = int.Parse(args[6]);
            istoimg = int.Parse(args[7]);
            isoriginal = int.Parse(args[8]);

            if(istoimg == 1)
            {
                Crack();
            }

            Workbook wb = null;
            int pages = 0;
            //判断文件页数
            if (istoimg == 1)
            {
                try
                {
                    wb = new Workbook(sourcefile);
                    pages = wb.Worksheets.Count;
                    if (isoriginal == 0 && pages < minpage)
                    {
                        PostThreadMessage(threadid, WM_MSG_EXCEL2TXT_STATUS, (int)OutStatus.PageLess, pages);
                        return;
                    }
                }
                catch (Exception e)
                {
//                     PostThreadMessage(threadid, WM_MSG_EXCEL2TXT_STATUS, (int)OutStatus.TotxtFailed, pages);
//                     return;
                    istoimg = 0;
                }
            }


            int txtstatus = ConvertFile();
            PostThreadMessage(threadid, WM_MSG_EXCEL2TXT_STATUS, txtstatus, pages);

            //toimg
            int imgstatus = 0;
            if(istoimg == 1 && txtstatus == 0)
            {
                try
                {
                    string outtifffile = (outimgpath + @"\" + fileid + ".tiff").Replace(@"\\", @"\");

                    wb.Save(outtifffile, SaveFormat.TIFF);
                    wb.Dispose();

                    Image img = Image.FromFile(outtifffile);
                    img.SelectActiveFrame(System.Drawing.Imaging.FrameDimension.Page, 0);
                    string outimgfile = (outimgpath + @"\" + fileid + ".png").Replace(@"\\", @"\");
                    img.Save(outimgfile, System.Drawing.Imaging.ImageFormat.Png);

                    img.Dispose();
                    File.Delete(outtifffile);

                    //
                    ExecuteScaleImg(outimgfile, (outimgpath + @"\" + fileid + "_small" + ".png").Replace(@"\\", @"\"), 1);
                }
                catch(Exception e)
                {
                    imgstatus = 3;
                }
                PostThreadMessage(threadid, WM_MSG_EXCEL2IMG_STATUS, imgstatus, 0);
            }

        }
        #region private func
        private static int ConvertFile()
        {
            if (!File.Exists(sourcefile))
            {
                return 1;
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
                        string txtfile = (outtxtpath + @"\" + fileid + ".txt").Replace(@"\\", @"\");
                        using (StreamWriter writer = new StreamWriter(txtfile, false, Encoding.Default))
                        {
                            writer.Write(context);
                            writer.Close();
                        }
                        reader.Close();
                        ExecuteRegexTxt(txtfile);
                        return 0;
                    }
                    catch (Exception exception)
                    {
                        Console.WriteLine("保存txt文件发生异常" + exception);
                        return 3;
                    }

                }
            }
            catch (Exception e)
            {
                Console.WriteLine("打开文件失败" + e);
                return 2;
            }
        }


        private static void Crack()//使用前调用一次即可
        {
            string[] stModule = new string[8]
            {
                "\u000E\u2008\u2001\u2000",
                "\u000F\u2008\u2001\u2000",
                "\u0002\u2008\u200B\u2001",
                "\u000F",
                "\u0006",
                "\u000E",
                "\u0003",
                "\u0002"
            };
            Assembly assembly = Assembly.GetAssembly(typeof(Aspose.Cells.License));


            Type typeLic = null, typeIsTrial = null, typeHelper = null;

            foreach (Type type in assembly.GetTypes())
            {
                if ((typeLic == null) && (type.Name == stModule[0]))
                {
                    typeLic = type;
                }
                else if ((typeIsTrial == null) && (type.Name == stModule[1]))
                {
                    typeIsTrial = type;
                }
                else if ((typeHelper == null) && (type.Name == stModule[2]))
                {
                    typeHelper = type;
                }
            }
            if (typeLic == null || typeIsTrial == null || typeHelper == null)
            {
                throw new Exception();
            }
            object lic = Activator.CreateInstance(typeLic);
            int findCount = 0;

            foreach (FieldInfo field in typeLic.GetFields(BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Instance))
            {
                if (field.FieldType == typeLic && field.Name == stModule[3])
                {
                    field.SetValue(null, lic);
                    ++findCount;
                }
                else if (field.FieldType == typeof(DateTime) && field.Name == stModule[4])
                {
                    field.SetValue(lic, DateTime.MaxValue);
                    ++findCount;
                }
                else if (field.FieldType == typeIsTrial && field.Name == stModule[5])
                {
                    field.SetValue(lic, 1);
                    ++findCount;
                }

            }
            foreach (FieldInfo field in typeHelper.GetFields(BindingFlags.NonPublic | BindingFlags.Static | BindingFlags.Instance))
            {
                if (field.FieldType == typeof(bool) && field.Name == stModule[6])
                {
                    field.SetValue(null, false);
                    ++findCount;
                }
                if (field.FieldType == typeof(int) && field.Name == stModule[7])
                {
                    field.SetValue(null, 128);
                    ++findCount;
                }
            }
            if (findCount < 5)
            {
                throw new NotSupportedException("无效的版本");
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


        private static void ExecuteRegexTxt(string sourcefile)
        {
            if (sourcefile != null && !sourcefile.Equals(""))
            {
                string command = sourcefile + " 0";
                Process process = new Process();//创建进程对象  
                ProcessStartInfo startInfo = new ProcessStartInfo();
                startInfo.FileName = "regexTxt.exe";//设定需要执行文件  
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
