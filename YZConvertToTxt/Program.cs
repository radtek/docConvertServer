using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Net;
using System.Collections.Specialized;
using ICSharpCode.SharpZipLib.Core;
using ICSharpCode.SharpZipLib.Zip;
using System.Runtime.InteropServices;
using MyJson;
using System.Diagnostics;
using System.Threading;


namespace YZConvertToTxt
{
    class Program
    {
        #region define
        enum OutStatus
        {
            ConvertSuccess,//0:转换成功
            FileLoss,//1:下载失败/文件不存在
            NoText,//2:扫描版或者纯图片
            TotxtFailed,//3:totxt转换失败
            PageLess//4:页数低于指定值
        };

        [DllImport("kernel32")]//返回取得字符串缓冲区的长度
        private static extern long GetPrivateProfileString(string section, string key,
            string def, StringBuilder retVal, int size, string filePath);


        [DllImport("user32.dll", EntryPoint = "PostThreadMessage")]
        private static extern int PostThreadMessage(

                          int threadId, //线程标识

                          uint msg, //消息标识符

                          int wParam, //具体由消息决定

                          int lParam); //具体由消息决定

        public const int USER = 0x0400;

        public const int WM_MSG_YZ2TXT_STATUS = USER + 0x21;
        public const int WM_MSG_YZ2IMG_STATUS = USER + 0x22;

        static string fileurl;
        static string outtxtpath;
        static int filetype;
        static int threadid;
        static int fileid;
        static int minpage;
        static string url;

        const int MS_TXT = 2;
        const int PDF_TXT = 24;
        const int MS_PAGE = 27;
        const int PDF_PAGE = 28;

        #endregion
        static void Main(string[] args)
        {
            if (args.Count<string>() != 6)
            {
//                 Console.WriteLine("args < 6");
//                 Console.ReadKey();
                return;
            }
            fileurl = args[0];
            outtxtpath = args[1];
            filetype = int.Parse(args[2]);
            threadid = int.Parse(args[3]);
            fileid = int.Parse(args[4]);
            minpage = int.Parse(args[5]);

//             Console.WriteLine(fileid);

            string inifile = System.AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "docConvertServer.ini";
            url = ReadIniData("CONFIG", "YZAPI", "", inifile);

            //获取页码
            int converttype = 0;
            if (filetype < 3)
                converttype = MS_PAGE;
            else
                converttype = PDF_PAGE;
            string strparams = "downloadUrl=" + fileurl + "&convertType=" + converttype.ToString();
            string content = HttpPost(url + "onlinefile", strparams);
//             Console.WriteLine(content);

            int pages = 0;
            try
            {
                ResponseDataPage outpage = JSON.parse<ResponseDataPage>(content);
                pages = outpage.pagecount;
            }
            catch
            {
                PostThreadMessage(threadid, WM_MSG_YZ2TXT_STATUS, 3, 0);
                return;
            }
            if(pages < minpage)
            {
                PostThreadMessage(threadid, WM_MSG_YZ2TXT_STATUS, (int)OutStatus.PageLess, pages);
//                 Console.ReadKey();
                return;
            }

            //转txt
            int txtstatus = convertTxt();
            PostThreadMessage(threadid, WM_MSG_YZ2TXT_STATUS, txtstatus, pages);

//             Console.ReadKey();
        }

        public static int convertTxt()
        {
            int converttype = 0;
            if (filetype < 3)
                converttype = MS_TXT;
            else
                converttype = PDF_TXT;
            string strparams = "downloadUrl=" + fileurl + "&convertType=" + converttype.ToString();
            string content = HttpPost(url + "onlinefile", strparams);
            string outtxtfile = outtxtpath + @"\" + fileid.ToString() + ".txt";
//             Console.WriteLine(content);

            try
            {
                ResponseData outdata = JSON.parse<ResponseData>(content);
                if (outdata.result == 0)
                {
                    List<string> downurls = outdata.data;
                    foreach (string downurl in downurls)
                    {
                        int err = HttpDownloadFile(downurl, outtxtfile);
                        if (err == 0)
                        {
                            ExecuteRegexTxt(outtxtfile);
                        }
                    }
                    return 0;
                }
                else
                {
                    return 3;
                }
            }
            catch
            {
                return 3;
            }

        }

        public static string HttpPost(string Url, string postDataStr)
        {
            try
            {
                HttpWebRequest request = (HttpWebRequest)WebRequest.Create(Url);
                request.Method = "POST";
                request.ContentType = "application/x-www-form-urlencoded";
                request.ContentLength = Encoding.UTF8.GetByteCount(postDataStr);
                //             request.CookieContainer = cookie;
                Stream myRequestStream = request.GetRequestStream();
                StreamWriter myStreamWriter = new StreamWriter(myRequestStream, Encoding.GetEncoding("gb2312"));
                myStreamWriter.Write(postDataStr);
                myStreamWriter.Close();
                HttpWebResponse response = (HttpWebResponse)request.GetResponse();
                //             response.Cookies = cookie.GetCookies(response.ResponseUri);
                Stream myResponseStream = response.GetResponseStream();
                StreamReader myStreamReader = new StreamReader(myResponseStream, Encoding.GetEncoding("utf-8"));
                string retString = myStreamReader.ReadToEnd();
                myStreamReader.Close();
                myResponseStream.Close();
                return retString;
            }
            catch
            {
                return "";
            }
        }

        public static int HttpDownloadFile(string url, string path)
        {
            try
            {
                // 设置参数
                HttpWebRequest request = WebRequest.Create(url) as HttpWebRequest;
                //发送请求并获取相应回应数据
                HttpWebResponse response = request.GetResponse() as HttpWebResponse;
                //直到request.GetResponse()程序才开始向目标网页发送Post请求
                Stream responseStream = response.GetResponseStream();
                //创建本地文件写入流
                Stream stream = new FileStream(path, FileMode.Create);
                byte[] bArr = new byte[1024];
                int size = responseStream.Read(bArr, 0, (int)bArr.Length);
                while (size > 0)
                {
                    stream.Write(bArr, 0, size);
                    size = responseStream.Read(bArr, 0, (int)bArr.Length);
                }
                stream.Close();
                responseStream.Close();
                return 0;
            }
            catch (Exception e)
            {
                return 1;
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

        private static void ExecuteRegexTxt(string sourcefile)
        {
            if (sourcefile != null && !sourcefile.Equals(""))
            {
                string command = sourcefile + " 1";
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


    }
}
