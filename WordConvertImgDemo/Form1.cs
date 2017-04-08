using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
//using System.Threading.Tasks;
using System.Windows.Forms;
using Aspose.Words;
using Aspose.Words.Saving;
using System.IO;
using System.Threading;

namespace WordConvertImgDemo
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private List<string> sourcefiles = new List<string>();
        private string outpath;

        private void bt_source_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                tb_sourcepath.Text = folderBrowserDialog1.SelectedPath;

                DirectoryInfo theFolder = new DirectoryInfo(folderBrowserDialog1.SelectedPath);
                FileInfo[] files = theFolder.GetFiles();
                foreach (FileInfo file in files)
                {
                    if (file.Name.IndexOf(".doc") > -1)
                    {
                        sourcefiles.Add(file.DirectoryName + "\\" + file.Name);
                    }
                }
            }

        }

        private void bt_out_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog2.ShowDialog() == DialogResult.OK)
            {
                tb_outpath.Text = folderBrowserDialog2.SelectedPath;
                outpath = folderBrowserDialog2.SelectedPath;
            }
        }

        private void bt_start_Click(object sender, EventArgs e)
        {
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
                Document doc = new Document(sourcefile);

                string alltext = doc.GetText();

                calcWords(alltext);

                ImageSaveOptions iso = new ImageSaveOptions(SaveFormat.Png);
                iso.Resolution = 96;
                iso.PrettyFormat = true;
                int pages = doc.PageCount;
                if (pages <= 0) return 1;
                pages = pages > 1 ? 1 : pages;
                for (int i = 0; i < pages; i++)
                {
                    iso.PageIndex = i;
                    doc.Save((outpath + @"\" + GetFileName(sourcefile) + i + ".png").Replace(@"\\", @"\"), iso);
                }
                return 0;
            }
            catch(Exception e)
            {
                return 1;   
            }
        }

        private void calcWords(string words)
        {
            int iAllChr = 0; //字符总数：不计字符'\n'和'\r'
            int iChineseChr = 0; //中文字符计数
            int iChinesePnct = 0;//中文标点计数
            int iEnglishChr = 0; //英文字符计数
            int iEnglishPnct = 0;//中文标点计数
            int iNumber = 0;  //数字字符：0-9
            foreach (char ch in words)
            {
                if (ch != '\n' && ch != '\r') iAllChr++;
                if ("～！＠＃￥％…＆（）—＋－＝".IndexOf(ch) != -1 ||
                 "｛｝【】：“”；‘'《》，。、？｜＼".IndexOf(ch) != -1) iChinesePnct++;
                if (ch >= 0x4e00 && ch <= 0x9fbb) iChineseChr++;
                if ("`~!@#$%^&*()_+-={}[]:\";'<>,.?/\\|".IndexOf(ch) != -1) iEnglishPnct++;
                if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) iEnglishChr++;
                if (ch >= '0' && ch <= '9') iNumber++;
            }
            string sStats = string.Format(string.Concat(
             "字符总数：{0}\r\n", "中文字符数：{1}\r\n", "中文标点数：{2}\r\n",
             "英文字符数：{3}\r\n", "英文标点数：{4}\r\n", "数字字符数：{5}\r\n"),
             iAllChr.ToString(), iChineseChr.ToString(), iEnglishChr.ToString(),
             iEnglishChr.ToString(), iEnglishPnct.ToString(), iNumber.ToString());
            MessageBox.Show(sStats);
        }
    }
}
