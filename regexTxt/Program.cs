using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.IO;
using System.Runtime.InteropServices;


namespace regexTxt
{
    class Program
    {
        [DllImport("kernel32")]//返回取得字符串缓冲区的长度
        private static extern long GetPrivateProfileString(string section, string key,
            string def, StringBuilder retVal, int size, string filePath);

        private static int maxTxtSize = 0;
        private static Dictionary<string, string> FilterList = new Dictionary<string, string>();
        private static string filepath;
        private static int type;//0-office; 1-pdf
        static void Main(string[] args)
        {
            if (args.Count<string>() != 2)
            {
                return;
            }
            filepath = args[0];
            type = int.Parse(args[1]);

            string inifile = System.AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "docConvertServer.ini";
            maxTxtSize = int.Parse(ReadIniData("CONFIG", "TXTMAXSIZE", "0", inifile)) * 1024;
            int regNum = int.Parse(ReadIniData("REGEX", "COUNT", "0", inifile));
            for(int i=0; i<regNum;i++)
            {
                string strkey = "REGEX" + i.ToString();
                string strvalue = ReadIniData("REGEX", strkey, "", inifile);
                if(strvalue.IndexOf('╳')>0)
                {
                    string[] strreg = strvalue.Split('╳');
                    FilterList.Add(strreg[0], strreg[1]);
                }
                
            }
            string outstr = "";
            if(File.Exists(filepath))
            {
                bool bok = ConvertTxt(filepath, ref outstr);
                if (bok)
                {
                    if (GetOutStr(ref outstr))
                    {
                        SaveFile(outstr, filepath);
                    }
                }
            }
        }

        private static bool ConvertTxt(string fileName, ref string OutStr)
        {
            try
            {
                if(type == 0)
                {
                    using (StreamReader reader = new StreamReader(fileName, Encoding.Default))
                    {
                        OutStr = reader.ReadToEnd();
                        reader.Close();
                    }
                }
                else
                {
                    using (StreamReader reader = new StreamReader(fileName, Encoding.UTF8))
                    {
                        OutStr = reader.ReadToEnd();
                        reader.Close();
                    }
                }
                return true;
            }
            catch (Exception exception)
            {
                Console.WriteLine("读取txt【" + fileName + "】出错", exception);
                return false;
            }
        }

        public static bool SaveFile(string Context, string SavePath)
        {
            try
            {
                using (StreamWriter writer = new StreamWriter(SavePath, false, Encoding.Default))
                {
                    writer.Write(Context);
                    writer.Close();
                }
                return true;
            }
            catch (Exception exception)
            {
                Console.WriteLine("保存txt文件发生异常", exception);
                return false;
            }
        }





        private static bool GetOutStr(ref string OutPutStr)
        {
            try
            {
                OutPutStr = FilterStr(OutPutStr, FilterList);
                if (OutPutStr.Length > maxTxtSize)
                {
                    OutPutStr = OutPutStr.Substring(0, maxTxtSize);
                }
                return true;
            }
            catch (Exception exception)
            {
                System.Console.WriteLine("正则/截取字符串时出错", exception);
                return false;
            }
        }




        private static string FilterStr(string str, Dictionary<string, string> FilterList)
        {
            if (FilterList == null)
            {
                return str;
            }
            try
            {
                foreach (KeyValuePair<string, string> pair in FilterList)
                {
                    str = Regex.Replace(str, pair.Key, pair.Value, RegexOptions.IgnoreCase);
                }
                return str;
            }
            catch
            {
                return str;
            }
        }



        private static string ReadIniData(string Section, string Key, string NoText, string iniFilePath)
        {
            if (File.Exists(iniFilePath))
            {
                StringBuilder temp = new StringBuilder(1024);
                GetPrivateProfileString(Section, Key, NoText, temp, 1024, iniFilePath);
                return temp.ToString();
            }
            else
            {
                return String.Empty;
            }
        }




    }
}
