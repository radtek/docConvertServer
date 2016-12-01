using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;

namespace ScaleImg
{
    class Program
    {
        static string sourcefile;
        static string outfile;
        static int filetype;

        static void Main(string[] args)
        {
            int len = args.Length;
            if (len != 3) return;
#if DEBUG
            foreach (string arg in args)
            {
                Console.WriteLine(arg);
            }
#endif
            sourcefile = args[0];
            outfile = args[1];
            filetype = int.Parse(args[2]);

            if(!File.Exists(sourcefile))
            {
                return;
            }

            int outwidth = 141;
            int outheight = 200;
            Bitmap bi = new Bitmap(sourcefile);
            int swidth = bi.Width;
            int sheight = bi.Height;

            Bitmap outmap = new Bitmap(outwidth, outheight);
            Graphics gr = Graphics.FromImage(outmap);
            gr.Clear(Color.Transparent);
            //设置画布描绘质量
            gr.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighQuality;
            gr.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            gr.InterpolationMode = System.Drawing.Drawing2D.InterpolationMode.HighQualityBicubic;
            gr.DrawImage(bi, new Rectangle(0, 0, outwidth, outheight), 0, 0, swidth, sheight, GraphicsUnit.Pixel);
            gr.Dispose();
            //以下代码为保存图片时，设置压缩质量
            EncoderParameters encoderParams = new EncoderParameters();
            long[] quality = new long[1];
            quality[0] = 100;
            EncoderParameter encoderParam = new EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality);
            encoderParams.Param[0] = encoderParam;
            if(filetype==0)
            {
                outmap.Save(outfile, System.Drawing.Imaging.ImageFormat.Jpeg);
            }
            else
            {
                outmap.Save(outfile, System.Drawing.Imaging.ImageFormat.Png);
            }
            bi.Dispose();
            outmap.Dispose();
        }
    }
}
