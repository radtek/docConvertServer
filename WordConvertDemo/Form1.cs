using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
//using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
//using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using System.Collections.Generic;
using EPocalipse.IFilter;
using System.Runtime.InteropServices;
using System.Threading;

namespace WordConvertDemo
{
    public partial class Form1 : Form
    {
        private List<string> sourcefiles = new List<string>();
        private string outpath;
        public Form1()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void bt_input_Click(object sender, EventArgs e)
        {
            if (folderBrowserDialog1.ShowDialog() == DialogResult.OK)
            {
                tb_sourcepath.Text = folderBrowserDialog1.SelectedPath;

                DirectoryInfo theFolder = new DirectoryInfo(folderBrowserDialog1.SelectedPath);
                FileInfo[] files = theFolder.GetFiles();
                foreach(FileInfo file in files)
                {
                    if (file.Name.IndexOf(".ppt") > -1 || file.Name.IndexOf(".doc") > -1 || file.Name.IndexOf(".xls") > -1 || file.Name.IndexOf(".pdf") > -1)
                    {
                        sourcefiles.Add(file.DirectoryName + "\\" + file.Name);
                    }
                }
            }
        }

        private void bt_output_Click(object sender, EventArgs e)
        {
            if(folderBrowserDialog2.ShowDialog()==DialogResult.OK)
            {
                tb_outpath.Text = folderBrowserDialog2.SelectedPath;
                outpath = folderBrowserDialog2.SelectedPath;
            }
        }

        private void start_convert_Click(object sender, EventArgs e)
        {
            if(sourcefiles.Count>0)
            {
                DateTime now = DateTime.Now;
                listBox1.Items.Add(now.ToString("yyyy-MM-dd hh:mm:ss") + "开始转换");
                foreach(string sourcefile in sourcefiles)
                {
                    if(ConvertFile(sourcefile)!=0)
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
            name = filepath.Substring(len+1);
            len = name.LastIndexOf(".");
            name = name.Substring(0, len);
            return name;
        }

        private int ConvertFile(string sourcefile)
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

                    calcWords(context);

                    context = Regex.Replace(context, "\n\r", " ", RegexOptions.IgnoreCase);

                    try
                    {
                        string name = GetFileName(sourcefile);
                        using (StreamWriter writer = new StreamWriter((outpath + @"\" + name + ".txt").Replace(@"\\", @"\"), false, Encoding.Default))
                        {
                            writer.Write(context);
                            writer.Close();
                        }
                        reader.Close();

                        return 0;
                    }
                    catch (Exception exception)
                    {
                        reader.Close();
                        return 3;
                    }

                }
            }
            catch (Exception e)
            {
                return 2;
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
