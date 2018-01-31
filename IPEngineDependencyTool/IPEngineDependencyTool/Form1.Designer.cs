namespace IPEngineDependencyTool
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.sfdSaveXML = new System.Windows.Forms.SaveFileDialog();
            this.ofdOpenXML = new System.Windows.Forms.OpenFileDialog();
            this.cmdOpenXML = new System.Windows.Forms.Button();
            this.cmdSave = new System.Windows.Forms.Button();
            this.lblFileName = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.depGraphControl1 = new IPEngineDependencyTool.DepGraphControl();
            this.panel2 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel4 = new System.Windows.Forms.Panel();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.listBox2 = new System.Windows.Forms.ListBox();
            this.splitter2 = new System.Windows.Forms.Splitter();
            this.listBox1 = new System.Windows.Forms.ListBox();
            this.moduleDetailView1 = new IPEngineDependencyTool.ModuleDetailView();
            this.splitter1 = new System.Windows.Forms.Splitter();
            this.dependencyDetailView1 = new IPEngineDependencyTool.DependencyDetailView();
            this.extensionDetailView1 = new IPEngineDependencyTool.ExtensionDetailView();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // cmdOpenXML
            // 
            this.cmdOpenXML.Location = new System.Drawing.Point(3, 3);
            this.cmdOpenXML.Name = "cmdOpenXML";
            this.cmdOpenXML.Size = new System.Drawing.Size(75, 23);
            this.cmdOpenXML.TabIndex = 0;
            this.cmdOpenXML.Text = "Open";
            this.cmdOpenXML.UseVisualStyleBackColor = true;
            this.cmdOpenXML.Click += new System.EventHandler(this.cmdOpenXML_Click);
            // 
            // cmdSave
            // 
            this.cmdSave.Location = new System.Drawing.Point(3, 32);
            this.cmdSave.Name = "cmdSave";
            this.cmdSave.Size = new System.Drawing.Size(75, 23);
            this.cmdSave.TabIndex = 1;
            this.cmdSave.Text = "Save";
            this.cmdSave.UseVisualStyleBackColor = true;
            this.cmdSave.Click += new System.EventHandler(this.cmdSave_Click);
            // 
            // lblFileName
            // 
            this.lblFileName.AutoSize = true;
            this.lblFileName.Location = new System.Drawing.Point(108, 13);
            this.lblFileName.Name = "lblFileName";
            this.lblFileName.Size = new System.Drawing.Size(0, 13);
            this.lblFileName.TabIndex = 2;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.depGraphControl1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(209, 400);
            this.panel1.TabIndex = 5;
            // 
            // depGraphControl1
            // 
            this.depGraphControl1.DepGraph = null;
            this.depGraphControl1.Dock = System.Windows.Forms.DockStyle.Left;
            this.depGraphControl1.Location = new System.Drawing.Point(0, 0);
            this.depGraphControl1.Name = "depGraphControl1";
            this.depGraphControl1.Size = new System.Drawing.Size(209, 400);
            this.depGraphControl1.TabIndex = 3;
            this.depGraphControl1.Enter += new System.EventHandler(this.depGraphControl1_Enter);
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.cmdOpenXML);
            this.panel2.Controls.Add(this.cmdSave);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(79, 429);
            this.panel2.TabIndex = 1;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.panel4);
            this.panel3.Controls.Add(this.splitter1);
            this.panel3.Controls.Add(this.panel1);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel3.Location = new System.Drawing.Point(79, 29);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(554, 400);
            this.panel3.TabIndex = 6;
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.splitContainer1);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel4.Location = new System.Drawing.Point(212, 0);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(342, 400);
            this.panel4.TabIndex = 7;
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.listBox2);
            this.splitContainer1.Panel1.Controls.Add(this.splitter2);
            this.splitContainer1.Panel1.Controls.Add(this.listBox1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.extensionDetailView1);
            this.splitContainer1.Panel2.Controls.Add(this.dependencyDetailView1);
            this.splitContainer1.Panel2.Controls.Add(this.moduleDetailView1);
            this.splitContainer1.Size = new System.Drawing.Size(342, 400);
            this.splitContainer1.SplitterDistance = 320;
            this.splitContainer1.TabIndex = 0;
            // 
            // listBox2
            // 
            this.listBox2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox2.FormattingEnabled = true;
            this.listBox2.Location = new System.Drawing.Point(153, 0);
            this.listBox2.Name = "listBox2";
            this.listBox2.Size = new System.Drawing.Size(189, 320);
            this.listBox2.TabIndex = 2;
            this.listBox2.SelectedIndexChanged += new System.EventHandler(this.listBox2_SelectedIndexChanged);
            this.listBox2.KeyUp += new System.Windows.Forms.KeyEventHandler(this.listBox2_KeyUp);
            // 
            // splitter2
            // 
            this.splitter2.Location = new System.Drawing.Point(150, 0);
            this.splitter2.Name = "splitter2";
            this.splitter2.Size = new System.Drawing.Size(3, 320);
            this.splitter2.TabIndex = 1;
            this.splitter2.TabStop = false;
            // 
            // listBox1
            // 
            this.listBox1.Dock = System.Windows.Forms.DockStyle.Left;
            this.listBox1.FormattingEnabled = true;
            this.listBox1.Location = new System.Drawing.Point(0, 0);
            this.listBox1.Name = "listBox1";
            this.listBox1.Size = new System.Drawing.Size(150, 320);
            this.listBox1.TabIndex = 0;
            this.listBox1.SelectedIndexChanged += new System.EventHandler(this.listBox1_SelectedIndexChanged);
            this.listBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.listBox1_KeyPress);
            this.listBox1.KeyUp += new System.Windows.Forms.KeyEventHandler(this.listBox1_KeyUp);
            // 
            // moduleDetailView1
            // 
            this.moduleDetailView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.moduleDetailView1.Location = new System.Drawing.Point(0, 0);
            this.moduleDetailView1.Name = "moduleDetailView1";
            this.moduleDetailView1.Size = new System.Drawing.Size(342, 76);
            this.moduleDetailView1.TabIndex = 0;
            // 
            // splitter1
            // 
            this.splitter1.Location = new System.Drawing.Point(209, 0);
            this.splitter1.Name = "splitter1";
            this.splitter1.Size = new System.Drawing.Size(3, 400);
            this.splitter1.TabIndex = 6;
            this.splitter1.TabStop = false;
            // 
            // dependencyDetailView1
            // 
            this.dependencyDetailView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.dependencyDetailView1.Location = new System.Drawing.Point(0, 0);
            this.dependencyDetailView1.Name = "dependencyDetailView1";
            this.dependencyDetailView1.Size = new System.Drawing.Size(342, 76);
            this.dependencyDetailView1.TabIndex = 1;
            this.dependencyDetailView1.Visible = false;
            // 
            // extensionDetailView1
            // 
            this.extensionDetailView1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.extensionDetailView1.Location = new System.Drawing.Point(0, 0);
            this.extensionDetailView1.Name = "extensionDetailView1";
            this.extensionDetailView1.Size = new System.Drawing.Size(342, 76);
            this.extensionDetailView1.TabIndex = 2;
            this.extensionDetailView1.Visible = false;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(633, 429);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.lblFileName);
            this.Name = "Form1";
            this.Text = "Form1";
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.SaveFileDialog sfdSaveXML;
        private System.Windows.Forms.OpenFileDialog ofdOpenXML;
        private System.Windows.Forms.Button cmdOpenXML;
        private System.Windows.Forms.Button cmdSave;
        private System.Windows.Forms.Label lblFileName;
        private DepGraphControl depGraphControl1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.Splitter splitter1;
        private System.Windows.Forms.ListBox listBox1;
        private ModuleDetailView moduleDetailView1;
        private System.Windows.Forms.ListBox listBox2;
        private System.Windows.Forms.Splitter splitter2;
        private ExtensionDetailView extensionDetailView1;
        private DependencyDetailView dependencyDetailView1;
    }
}

