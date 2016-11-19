namespace WordConvertDemo
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.bt_input = new System.Windows.Forms.Button();
            this.tb_sourcepath = new System.Windows.Forms.TextBox();
            this.bt_output = new System.Windows.Forms.Button();
            this.tb_outpath = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.start_convert = new System.Windows.Forms.Button();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.openFileDialog2 = new System.Windows.Forms.OpenFileDialog();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.folderBrowserDialog2 = new System.Windows.Forms.FolderBrowserDialog();
            this.SuspendLayout();
            // 
            // bt_input
            // 
            this.bt_input.Location = new System.Drawing.Point(304, 36);
            this.bt_input.Name = "bt_input";
            this.bt_input.Size = new System.Drawing.Size(75, 23);
            this.bt_input.TabIndex = 0;
            this.bt_input.Text = "浏览";
            this.bt_input.UseVisualStyleBackColor = true;
            this.bt_input.Click += new System.EventHandler(this.bt_input_Click);
            // 
            // tb_sourcepath
            // 
            this.tb_sourcepath.Location = new System.Drawing.Point(83, 36);
            this.tb_sourcepath.Name = "tb_sourcepath";
            this.tb_sourcepath.Size = new System.Drawing.Size(215, 21);
            this.tb_sourcepath.TabIndex = 1;
            // 
            // bt_output
            // 
            this.bt_output.Location = new System.Drawing.Point(304, 75);
            this.bt_output.Name = "bt_output";
            this.bt_output.Size = new System.Drawing.Size(75, 23);
            this.bt_output.TabIndex = 0;
            this.bt_output.Text = "浏览";
            this.bt_output.UseVisualStyleBackColor = true;
            this.bt_output.Click += new System.EventHandler(this.bt_output_Click);
            // 
            // tb_outpath
            // 
            this.tb_outpath.Location = new System.Drawing.Point(83, 75);
            this.tb_outpath.Name = "tb_outpath";
            this.tb_outpath.Size = new System.Drawing.Size(215, 21);
            this.tb_outpath.TabIndex = 1;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(10, 39);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 2;
            this.label1.Text = "源文件夹";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 80);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "目标文件夹";
            // 
            // start_convert
            // 
            this.start_convert.Location = new System.Drawing.Point(304, 237);
            this.start_convert.Name = "start_convert";
            this.start_convert.Size = new System.Drawing.Size(75, 23);
            this.start_convert.TabIndex = 0;
            this.start_convert.Text = "开始转换";
            this.start_convert.UseVisualStyleBackColor = true;
            this.start_convert.Click += new System.EventHandler(this.start_convert_Click);
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(24, 112);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(355, 112);
            this.listBox1.TabIndex = 3;
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.FileName = "openFileDialog1";
            // 
            // openFileDialog2
            // 
            this.openFileDialog2.FileName = "openFileDialog2";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(411, 281);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.tb_outpath);
            this.Controls.Add(this.tb_sourcepath);
            this.Controls.Add(this.start_convert);
            this.Controls.Add(this.bt_output);
            this.Controls.Add(this.bt_input);
            this.Name = "Form1";
            this.Text = "wordppt2txtDemo";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button bt_input;
        private System.Windows.Forms.TextBox tb_sourcepath;
        private System.Windows.Forms.Button bt_output;
        private System.Windows.Forms.TextBox tb_outpath;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button start_convert;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.OpenFileDialog openFileDialog2;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog2;
    }
}

