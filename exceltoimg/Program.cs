///EXCEL转img
///create by hanson 20161031
///
///
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Drawing;
using Aspose.Cells;
using System.Runtime.InteropServices;
using System.Reflection;


namespace exceltoimg
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

        public const int WM_MSG_EXCEL2IMG_STATUS = USER + 0x14;

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

            Crack();
            int status = ConvertFile();
            PostThreadMessage(threadid, WM_MSG_EXCEL2IMG_STATUS, status, 0);

        }


        static private int ConvertFile()
        {
            try
            {
                Workbook wb = new Workbook(sourcefile);

                string outtifffile = (outpath + @"\" + fileid + ".tiff").Replace(@"\\", @"\");

                wb.Save(outtifffile, SaveFormat.TIFF);
                wb.Dispose();

                Image img = Image.FromFile(outtifffile);
                for (int i = 0; i < 1; i++)
                {
                    img.SelectActiveFrame(System.Drawing.Imaging.FrameDimension.Page, i);
                    img.Save((outpath + @"\" + fileid + ".png").Replace(@"\\", @"\"), System.Drawing.Imaging.ImageFormat.Png);
                }

                img.Dispose();
                File.Delete(outtifffile);

                return 0;
            }
            catch (Exception e)
            {
                return 1;
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
        

    }
}
