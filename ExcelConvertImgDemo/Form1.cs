using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using System.Reflection;
using Aspose.Cells;

namespace ExcelConvertImgDemo
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private List<string> sourcefiles = new List<string>();
        private string outpath;

        private void button2_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog2.ShowDialog() == DialogResult.OK)
            {
                tb_outpath.Text = folderBrowserDialog2.SelectedPath;
                outpath = folderBrowserDialog2.SelectedPath;
            }
        }

        private void bt_source_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                tb_sourcepath.Text = folderBrowserDialog1.SelectedPath;

                DirectoryInfo theFolder = new DirectoryInfo(folderBrowserDialog1.SelectedPath);
                FileInfo[] files = theFolder.GetFiles();
                foreach (FileInfo file in files)
                {
                    if (file.Name.IndexOf(".xls") > -1)
                    {
                        sourcefiles.Add(file.DirectoryName + "\\" + file.Name);
                    }
                }
            }

        }

        private void bt_start_Click(object sender, EventArgs e)
        {
            Crack();
            if (sourcefiles.Count > 0)
            {
                DateTime now = DateTime.Now;
                listBox1.Items.Add(now.ToString("yyyy-MM-dd hh:mm:ss") + "开始转换");
                foreach (string sourcefile in sourcefiles)
                {
                    if (ConvertFile(sourcefile) != 0)
                    {
                        listBox1.Items.Add(sourcefile + "--转换失败");
                    }
                    Thread.Sleep(50);
                }
                listBox1.Items.Add(DateTime.Now.ToString("yyyy-MM-dd hh:mm:ss") + "转换结束");
            }
        }

        private string GetFileName(string filepath)
        {
            string name;
            filepath = filepath.Replace("/", "\\");
            int len = filepath.LastIndexOf("\\");
            name = filepath.Substring(len + 1);
            len = name.LastIndexOf(".");
            name = name.Substring(0, len);
            return name;
        }

        private int ConvertFile(string sourcefile)
        {
            try
            {
                Workbook wb = new Workbook(sourcefile);

//                 int ncount = wb.Worksheets.Count;
//                 ncount = ncount > 1 ? 1 : ncount;
//                 int[] pages = new int[ncount];
//                 for (int j = 0; j < ncount; j++ )
//                 {
//                     pages[j] = j + 1;
//                 }

                string outtifffile = (outpath+@"\"+GetFileName(sourcefile)+".tiff").Replace(@"\\",@"\");

                wb.Save(outtifffile,SaveFormat.TIFF);
                wb.Dispose();

                Image img = Image.FromFile(outtifffile);
                for (int i = 0; i < 1; i++)
                {
                    img.SelectActiveFrame(System.Drawing.Imaging.FrameDimension.Page, i);
                    img.Save((outpath + @"\" + GetFileName(sourcefile) + i + ".png").Replace(@"\\", @"\"), System.Drawing.Imaging.ImageFormat.Png);
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
