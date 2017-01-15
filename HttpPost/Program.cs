using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.IO;
using System.Runtime.InteropServices;
using MyJson;

namespace HttpPost
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
        public const int WM_MSG_HTTPPOST = USER + 0x41;
        static void Main(string[] args)
        {
            if (args.Length != 9)
            {
                return;
            }
            int threadid = int.Parse(args[0]);
            string url = args[1];
            int ntype = int.Parse(args[2]);
            int nid = int.Parse(args[3]);
            string node = args[4];
            int txtstatus = int.Parse(args[5]);
            int pagenumber = int.Parse(args[6]);
            string txturl = args[7];
            string imgurl = args[8];
            txturl = txturl == "null" ? "" : txturl;
            imgurl = imgurl == "null" ? "" : imgurl;

            string context = "";
            if(ntype == 1)
            {
                ConvertSuccessData succ = new ConvertSuccessData();
                succ.id = nid;
                succ.node = node;
                succ.txtstatus = txtstatus;
                succ.pagenumber = pagenumber;
                succ.txturl = txturl;
                succ.imgurl = imgurl;
                context = JSON.stringify(succ);
            }
            else
            {
                ConvertFailData succ = new ConvertFailData();
                succ.id = nid;
                succ.node = node;
                succ.txtstatus = txtstatus;
                succ.pagenumber = pagenumber;
                succ.txturl = txturl;
                context = JSON.stringify(succ);
            }
            context = "data=" + context;
            Console.WriteLine(context);

            int status = Post(url, context);
            PostThreadMessage(threadid, WM_MSG_HTTPPOST, status, 0);

//             Console.ReadLine();
        }

        /// <summary>  
        /// 指定Post地址使用Get 方式获取全部字符串  
        /// </summary>  
        /// <param name="url">请求后台地址</param>  
        /// <param name="content">Post提交数据内容(utf-8编码的)</param>  
        /// <returns></returns>  
        public static int Post(string url, string content)
        {
            try
            {
                string result = "";
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
                req.Method = "POST";
                req.ContentType = "application/x-www-form-urlencoded";

                #region 添加Post 参数
                byte[] data = Encoding.UTF8.GetBytes(content);
                req.ContentLength = data.Length;
                using (Stream reqStream = req.GetRequestStream())
                {
                    reqStream.Write(data, 0, data.Length);
                    reqStream.Close();
                }
                #endregion

                HttpWebResponse resp = (HttpWebResponse)req.GetResponse();
                Stream stream = resp.GetResponseStream();
                //获取响应内容  
                using (StreamReader reader = new StreamReader(stream, Encoding.UTF8))
                {
                    result = reader.ReadToEnd();
                }
                if (result != "")
                {
                    ResponseData outdata = JSON.parse<ResponseData>(result);
                    return outdata.status;
                }
            }
            catch(Exception e)
            {
                return 1;
            }
            return 1;
        }

    }
}
