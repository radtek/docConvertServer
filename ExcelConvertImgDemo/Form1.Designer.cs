namespace ExcelConvertImgDemo
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
            this.bt_source = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.tb_sourcepath = new System.Windows.Forms.TextBox();
            this.bt_out = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.tb_outpath = new System.Windows.Forms.TextBox();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.bt_start = new System.Windows.Forms.Button();
            this.folderBrowserDialog1 = new System.Windows.Forms.FolderBrowserDialog();
            this.folderBrowserDialog2 = new System.Windows.Forms.FolderBrowserDialog();
            this.SuspendLayout();
            // 
            // bt_source
            // 
            this.bt_source.Location = new System.Drawing.Point(288, 45);
            this.bt_source.Name = "bt_source";
            this.bt_source.Size = new System.Drawing.Size(75, 23);
            this.bt_source.TabIndex = 0;
            this.bt_source.Text = "浏览";
            this.bt_source.UseVisualStyleBackColor = true;
            this.bt_source.Click += new System.EventHandler(this.bt_source_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 50);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(53, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "源文件夹";
            // 
            // tb_sourcepath
            // 
            this.tb_sourcepath.Location = new System.Drawing.Point(86, 45);
            this.tb_sourcepath.Name = "tb_sourcepath";
            this.tb_sourcepath.Size = new System.Drawing.Size(185, 21);
            this.tb_sourcepath.TabIndex = 2;
            // 
            // bt_out
            // 
            this.bt_out.Location = new System.Drawing.Point(288, 82);
            this.bt_out.Name = "bt_out";
            this.bt_out.Size = new System.Drawing.Size(75, 23);
            this.bt_out.TabIndex = 0;
            this.bt_out.Text = "浏览";
            this.bt_out.UseVisualStyleBackColor = true;
            this.bt_out.Click += new System.EventHandler(this.button2_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 87);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(65, 12);
            this.label2.TabIndex = 1;
            this.label2.Text = "目标文件夹";
            // 
            // tb_outpath
            // 
            this.tb_outpath.Location = new System.Drawing.Point(86, 82);
            this.tb_outpath.Name = "tb_outpath";
            this.tb_outpath.Size = new System.Drawing.Size(185, 21);
            this.tb_outpath.TabIndex = 2;
            // 
            // listBox1
            // 
            this.listBox1.FormattingEnabled = true;
            this.listBox1.ItemHeight = 12;
            this.listBox1.Location = new System.Drawing.Point(14, 125);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(349, 136);
            this.listBox1.TabIndex = 3;
            // 
            // bt_start
            // 
            this.bt_start.Location = new System.Drawing.Point(288, 285);
            this.bt_start.Name = "bt_start";
            this.bt_start.Size = new System.Drawing.Size(75, 23);
            this.bt_start.TabIndex = 4;
            this.bt_start.Text = "开始转换";
            this.bt_start.UseVisualStyleBackColor = true;
            this.bt_start.Click += new System.EventHandler(this.bt_start_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(370, 320);
            this.Controls.Add(this.bt_start);
            this.Controls.Add(this.listBox1);
            this.Controls.Add(this.tb_outpath);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tb_sourcepath);
            this.Controls.Add(this.bt_out);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.bt_source);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button bt_source;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tb_sourcepath;
        private System.Windows.Forms.Button bt_out;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tb_outpath;
        private System.Windows.Forms.ListBox listBox1;
        private System.Windows.Forms.Button bt_start;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog1;
        private System.Windows.Forms.FolderBrowserDialog folderBrowserDialog2;
    }
}

