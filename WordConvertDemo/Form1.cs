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
                    if (file.Name.IndexOf(".ppt") > -1 || file.Name.IndexOf(".doc") > -1 || file.Name.IndexOf(".xls") > -1)
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
    }
}
